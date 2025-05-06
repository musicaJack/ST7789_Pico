#include "st7789_hal.hpp"
#include "hardware/gpio.h"
#include "hardware/dma.h"
#include "hardware/irq.h"
#include "pico/stdlib.h"
#include <cstring>
#include <cstdio>
#include <cstdlib>

namespace st7789 {

// 定义全局变量用于DMA中断处理
static HAL* current_hal_instance = nullptr;

// DMA传输完成的处理函数
void dma_complete_handler() {
    // 确保有有效实例
    if (current_hal_instance) {
        // 清除中断标志
        dma_hw->ints0 = 1u << current_hal_instance->_dma_tx_channel;
        
        // 更新状态
        current_hal_instance->_dma_busy = false;
    }
}

HAL::HAL() : 
    _initialized(false),
    _dma_tx_channel(-1),
    _dma_buffer(nullptr),
    _dma_buffer_size(0),
    _dma_enabled(false),
    _dma_busy(false) {
}

HAL::~HAL() {
    cleanupDma();
}

bool HAL::init(const Config& config) {
    _config = config;
    
    // 初始化GPIO 
    gpio_init(_config.pin_cs);
    gpio_init(_config.pin_dc);
    gpio_init(_config.pin_reset);
    gpio_init(_config.pin_bl);
    
    // 设置方向
    gpio_set_dir(_config.pin_cs, GPIO_OUT);
    gpio_set_dir(_config.pin_dc, GPIO_OUT);
    gpio_set_dir(_config.pin_reset, GPIO_OUT);
    gpio_set_dir(_config.pin_bl, GPIO_OUT);
    
    // 初始状态
    gpio_put(_config.pin_cs, 1);     // 不选中
    gpio_put(_config.pin_dc, 1);     // 数据模式
    gpio_put(_config.pin_reset, 1);  // 不复位
    gpio_put(_config.pin_bl, 0);     // 背光关闭
    
    // 初始化SPI
    spi_init(_config.spi_inst, _config.spi_speed_hz);
    gpio_set_function(_config.pin_sck, GPIO_FUNC_SPI);
    gpio_set_function(_config.pin_din, GPIO_FUNC_SPI);
    
    // 复位显示屏
    reset();
    
    // 初始化DMA (如果启用)
    if (_config.dma.enabled) {
        initDma();
    }
    
    _initialized = true;
    return true;
}

void HAL::initDma() {
    // 保存当前实例以供中断使用
    current_hal_instance = this;
    
    // 分配DMA通道
    _dma_tx_channel = dma_claim_unused_channel(true);
    if (_dma_tx_channel < 0) {
        printf("无法获取DMA通道\n");
        _dma_enabled = false;
        return;
    }
    
    // 分配DMA缓冲区
    _dma_buffer_size = _config.dma.buffer_size;
    _dma_buffer = (uint16_t*)malloc(_dma_buffer_size);
    if (!_dma_buffer) {
        printf("无法分配DMA缓冲区\n");
        dma_channel_unclaim(_dma_tx_channel);
        _dma_tx_channel = -1;
        _dma_enabled = false;
        return;
    }
    
    // 配置DMA
    dma_channel_config dma_config = dma_channel_get_default_config(_dma_tx_channel);
    channel_config_set_transfer_data_size(&dma_config, DMA_SIZE_16);
    channel_config_set_dreq(&dma_config, spi_get_dreq(_config.spi_inst, true));
    
    // 设置DMA
    dma_channel_configure(
        _dma_tx_channel,
        &dma_config,
        &spi_get_hw(_config.spi_inst)->dr,  // 写入到SPI数据寄存器
        NULL,                                // 源地址将在传输时设置
        0,                                   // 传输数量将在传输时设置
        false                                // 不立即启动
    );
    
    // 配置DMA完成中断
    dma_channel_set_irq0_enabled(_dma_tx_channel, true);
    irq_set_exclusive_handler(DMA_IRQ_0, dma_complete_handler);
    irq_set_enabled(DMA_IRQ_0, true);
    
    _dma_enabled = true;
    printf("DMA初始化成功，通道: %d\n", _dma_tx_channel);
}

void HAL::cleanupDma() {
    if (_dma_tx_channel >= 0) {
        // 停止任何正在进行的传输
        dma_channel_abort(_dma_tx_channel);
        
        // 禁用中断
        dma_channel_set_irq0_enabled(_dma_tx_channel, false);
        
        // 释放通道
        dma_channel_unclaim(_dma_tx_channel);
        _dma_tx_channel = -1;
    }
    
    // 释放缓冲区
    if (_dma_buffer) {
        free(_dma_buffer);
        _dma_buffer = nullptr;
    }
    
    _dma_enabled = false;
    _dma_busy = false;
}

void HAL::writeCommand(uint8_t cmd) {
    gpio_put(_config.pin_cs, 0);  // 选中芯片
    gpio_put(_config.pin_dc, 0);  // 命令模式
    spi_write_blocking(_config.spi_inst, &cmd, 1);
    gpio_put(_config.pin_cs, 1);  // 取消选中
}

void HAL::writeData(uint8_t data) {
    gpio_put(_config.pin_cs, 0);  // 选中芯片
    gpio_put(_config.pin_dc, 1);  // 数据模式
    spi_write_blocking(_config.spi_inst, &data, 1);
    gpio_put(_config.pin_cs, 1);  // 取消选中
}

void HAL::writeDataBulk(const uint8_t* data, size_t len) {
    if (len == 0) return;
    
    gpio_put(_config.pin_cs, 0);  // 选中芯片
    gpio_put(_config.pin_dc, 1);  // 数据模式
    spi_write_blocking(_config.spi_inst, data, len);
    gpio_put(_config.pin_cs, 1);  // 取消选中
}

bool HAL::writeDataDma(const uint16_t* data, size_t len) {
    if (!_dma_enabled || !_dma_buffer || _dma_tx_channel < 0) {
        // 如果DMA不可用，回退到普通方式
        writeDataBulk((const uint8_t*)data, len * 2);
        return false;
    }
    
    // 如果DMA忙，等待完成
    if (_dma_busy) {
        if (!waitForDmaComplete()) {
            printf("DMA超时，终止操作\n");
            abortDma();
            return false;
        }
    }
    
    // 设置数据/命令引脚为数据模式
    gpio_put(_config.pin_dc, 1);
    // 设置片选引脚
    gpio_put(_config.pin_cs, 0);
    
    // 处理过大的传输
    size_t remaining = len;
    const uint16_t* src_ptr = data;
    
    while (remaining > 0) {
        // 确定本次传输量
        size_t transfer_size = (remaining > _dma_buffer_size/2) ? _dma_buffer_size/2 : remaining;
        
        // 准备数据(需要拷贝数据是因为可能需要进行字节序转换)
        for (size_t i = 0; i < transfer_size; i++) {
            _dma_buffer[i] = src_ptr[i];
        }
        
        // 标记DMA忙
        _dma_busy = true;
        
        // 配置并启动DMA传输
        dma_channel_set_read_addr(_dma_tx_channel, _dma_buffer, false);
        dma_channel_set_trans_count(_dma_tx_channel, transfer_size, true); // 启动传输
        
        // 等待传输完成
        if (!waitForDmaComplete()) {
            printf("DMA传输超时\n");
            abortDma();
            gpio_put(_config.pin_cs, 1); // 释放片选
            return false;
        }
        
        // 更新指针和剩余量
        src_ptr += transfer_size;
        remaining -= transfer_size;
    }
    
    // 释放片选
    gpio_put(_config.pin_cs, 1);
    return true;
}

bool HAL::waitForDmaComplete(uint32_t timeout_ms) {
    uint32_t start = to_ms_since_boot(get_absolute_time());
    while (_dma_busy) {
        // 检查超时
        if (to_ms_since_boot(get_absolute_time()) - start > timeout_ms) {
            return false;
        }
        // 让出CPU时间
        tight_loop_contents();
    }
    return true;
}

void HAL::abortDma() {
    if (_dma_tx_channel >= 0) {
        dma_channel_abort(_dma_tx_channel);
    }
    _dma_busy = false;
}

void HAL::reset() {
    // 复位序列
    gpio_put(_config.pin_reset, 0);  // 复位状态
    delay(20);
    gpio_put(_config.pin_reset, 1);  // 正常状态
    delay(120);  // 等待复位完成
}

void HAL::setBacklight(bool on) {
    gpio_put(_config.pin_bl, on ? 1 : 0);
}

void HAL::setBrightness(uint8_t brightness) {
    // 如果硬件支持PWM控制背光，可以在这里实现
    // 简单实现：开关背光
    setBacklight(brightness > 0);
}

void HAL::delay(uint32_t ms) {
    sleep_ms(ms);
}

} // namespace st7789 
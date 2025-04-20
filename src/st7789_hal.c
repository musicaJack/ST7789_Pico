/**
 * @file st7789_hal.c
 * @brief ST7789 LCD硬件抽象层实现
 */

#include "st7789_hal.h"
#include <stdio.h>

// 静态配置
static struct {
    spi_inst_t *spi_inst;
    uint8_t pin_cs;
    uint8_t pin_dc;
    uint8_t pin_reset;
    uint8_t pin_bl;
    bool is_initialized;
} st7789_hal_config = {0};

/**
 * @brief 初始化硬件接口
 */
bool st7789_hal_init(const st7789_config_t *config) {
    if (config == NULL) {
        printf("错误: HAL初始化配置为空\n");
        return false;
    }
    
    printf("初始化SPI和GPIO...\n");
    
    // 保存配置参数
    st7789_hal_config.spi_inst = config->spi_inst;
    st7789_hal_config.pin_cs = config->pin_cs;
    st7789_hal_config.pin_dc = config->pin_dc;
    st7789_hal_config.pin_reset = config->pin_reset;
    st7789_hal_config.pin_bl = config->pin_bl;
    
    // 初始化复位引脚并执行硬件复位
    gpio_init(config->pin_reset);
    gpio_set_dir(config->pin_reset, GPIO_OUT);
    gpio_put(config->pin_reset, 0);  // 复位状态
    sleep_ms(100);
    gpio_put(config->pin_reset, 1);  // 释放复位
    sleep_ms(100);
    
    // 初始化控制引脚
    gpio_init(config->pin_dc);
    gpio_init(config->pin_bl);
    gpio_init(config->pin_cs);
    
    // 设置GPIO方向
    gpio_set_dir(config->pin_dc, GPIO_OUT);
    gpio_set_dir(config->pin_bl, GPIO_OUT);
    gpio_set_dir(config->pin_cs, GPIO_OUT);
    
    // 初始状态 - 背光关闭，CS高电平（未选中）
    gpio_put(config->pin_cs, 1);
    gpio_put(config->pin_dc, 1);
    gpio_put(config->pin_bl, 0);
    
    // 初始化SPI
    printf("配置SPI...\n");
    spi_init(config->spi_inst, config->spi_speed_hz);
    
    // 设置SPI格式 - Mode 0
    spi_set_format(config->spi_inst, 8, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);
    
    // 设置SPI引脚功能
    gpio_set_function(config->pin_sck, GPIO_FUNC_SPI);
    gpio_set_function(config->pin_din, GPIO_FUNC_SPI);
    
    st7789_hal_config.is_initialized = true;
    printf("SPI和GPIO初始化完成\n");
    
    return true;
}

/**
 * @brief 发送命令到LCD
 */
void st7789_hal_write_cmd(uint8_t cmd) {
    if (!st7789_hal_config.is_initialized) return;
    
    gpio_put(st7789_hal_config.pin_cs, 0);  // 选中芯片
    gpio_put(st7789_hal_config.pin_dc, 0);  // 命令模式
    spi_write_blocking(st7789_hal_config.spi_inst, &cmd, 1);
    gpio_put(st7789_hal_config.pin_cs, 1);  // 取消选中
    sleep_us(10);  // 短暂延时
}

/**
 * @brief 发送单字节数据到LCD
 */
void st7789_hal_write_data(uint8_t data) {
    if (!st7789_hal_config.is_initialized) return;
    
    gpio_put(st7789_hal_config.pin_cs, 0);  // 选中芯片
    gpio_put(st7789_hal_config.pin_dc, 1);  // 数据模式
    spi_write_blocking(st7789_hal_config.spi_inst, &data, 1);
    gpio_put(st7789_hal_config.pin_cs, 1);  // 取消选中
}

/**
 * @brief 发送多字节数据到LCD
 */
void st7789_hal_write_data_bulk(const uint8_t *data, size_t len) {
    if (!st7789_hal_config.is_initialized || data == NULL) return;
    
    gpio_put(st7789_hal_config.pin_cs, 0);  // 选中芯片
    gpio_put(st7789_hal_config.pin_dc, 1);  // 数据模式
    spi_write_blocking(st7789_hal_config.spi_inst, data, len);
    gpio_put(st7789_hal_config.pin_cs, 1);  // 取消选中
}

/**
 * @brief 控制LCD复位引脚
 */
void st7789_hal_reset(bool state) {
    if (!st7789_hal_config.is_initialized) return;
    
    gpio_put(st7789_hal_config.pin_reset, state ? 1 : 0);
}

/**
 * @brief 控制LCD背光引脚
 */
void st7789_hal_set_backlight(bool state) {
    if (!st7789_hal_config.is_initialized) return;
    
    gpio_put(st7789_hal_config.pin_bl, state ? 1 : 0);
}

/**
 * @brief 延时函数(毫秒)
 */
void st7789_hal_delay_ms(uint32_t ms) {
    sleep_ms(ms);
}

/**
 * @brief 延时函数(微秒)
 */
void st7789_hal_delay_us(uint32_t us) {
    sleep_us(us);
} 
#pragma once

#include <cstdint>
#include "hardware/spi.h"
#include "hardware/dma.h"
#include "st7789_config.hpp"

namespace st7789 {

// 硬件抽象层类 - 处理所有与硬件相关的操作
class HAL {
private:
    Config _config;
    bool _initialized;
    
    // DMA相关成员
    int _dma_tx_channel;
    uint16_t* _dma_buffer;
    size_t _dma_buffer_size;
    bool _dma_enabled;
    bool _dma_busy;
    
    // 私有方法
    void initDma();
    void cleanupDma();
    bool waitForDmaComplete(uint32_t timeout_ms = 1000);
    
public:
    HAL();
    virtual ~HAL();
    
    // 初始化硬件
    bool init(const Config& config);
    
    // 基本IO操作
    void writeCommand(uint8_t cmd);
    void writeData(uint8_t data);
    void writeDataBulk(const uint8_t* data, size_t len);
    
    // DMA操作
    bool writeDataDma(const uint16_t* data, size_t len);
    bool isDmaBusy() const { return _dma_busy; }
    bool isDmaEnabled() const { return _dma_enabled; }
    void abortDma();
    
    // 硬件控制
    void reset();
    void setBacklight(bool on);
    void setBrightness(uint8_t brightness); // 如果硬件支持PWM
    
    // 自旋锁和延时
    void delay(uint32_t ms);
    
    // 获取配置
    const Config& getConfig() const { return _config; }
    
    // 旋转和尺寸设置
    void setWidth(uint16_t width) { _config.width = width; }
    void setHeight(uint16_t height) { _config.height = height; }
    void setRotation(Rotation rotation) { _config.rotation = rotation; }
    
    // 友元声明 - 允许中断处理程序访问私有成员
    friend void dma_complete_handler();
};

} // namespace st7789 
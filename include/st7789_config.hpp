#pragma once

#include <cstdint>
#include "hardware/spi.h"
#include "hardware/dma.h"

namespace st7789 {

// 颜色定义 (RGB565格式)
enum Color {
    BLACK     = 0x0000,
    WHITE     = 0xFFFF,
    RED       = 0xF800,
    GREEN     = 0x07E0,
    BLUE      = 0x001F,
    YELLOW    = 0xFFE0,
    CYAN      = 0x07FF,
    MAGENTA   = 0xF81F
};

// 旋转方向
enum Rotation {
    ROTATION_0   = 0,
    ROTATION_90  = 1,
    ROTATION_180 = 2,
    ROTATION_270 = 3
};

// DMA配置
struct DmaConfig {
    bool enabled;           // 是否启用DMA
    uint dma_tx_channel;    // DMA发送通道
    size_t buffer_size;     // DMA缓冲区大小
    
    // 构造函数，提供默认值
    DmaConfig() :
        enabled(true),      // 默认启用DMA
        dma_tx_channel(0),  // 使用通道0，实际初始化时会自动分配
        buffer_size(4096)   // 默认4KB缓冲区
    {}
};

// 配置结构体
struct Config {
    spi_inst_t* spi_inst;     // SPI实例
    uint32_t spi_speed_hz;    // SPI速度
    
    uint8_t pin_din;          // MOSI
    uint8_t pin_sck;          // SCK
    uint8_t pin_cs;           // 片选
    uint8_t pin_dc;           // 数据/命令
    uint8_t pin_reset;        // 复位
    uint8_t pin_bl;           // 背光
    
    uint16_t width;           // 宽度
    uint16_t height;          // 高度
    Rotation rotation;        // 旋转方向
    
    // DMA配置
    DmaConfig dma;
    
    // 构造函数，提供默认值
    Config() : 
        spi_inst(spi0),
        spi_speed_hz(40 * 1000 * 1000),  // 40MHz
        pin_din(19),
        pin_sck(18),
        pin_cs(17),
        pin_dc(20),
        pin_reset(15),
        pin_bl(10),
        width(240),
        height(320),
        rotation(ROTATION_90),  // 默认设置为90度旋转
        dma() {}
};

} // namespace st7789 
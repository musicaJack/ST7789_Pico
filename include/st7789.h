/**
 * @file st7789.h
 * @brief ST7789 LCD驱动头文件
 */

#ifndef _ST7789_H_
#define _ST7789_H_

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"

/**
 * @brief LCD配置结构体
 */
typedef struct {
    spi_inst_t *spi_inst;    // SPI实例
    uint32_t spi_speed_hz;   // SPI速度（Hz）
    
    // 引脚定义
    uint8_t pin_din;         // MOSI
    uint8_t pin_sck;         // SCK
    uint8_t pin_cs;          // CS
    uint8_t pin_dc;          // 数据/命令
    uint8_t pin_reset;       // 复位
    uint8_t pin_bl;          // 背光
    
    // 屏幕参数
    uint16_t width;          // 宽度
    uint16_t height;         // 高度
    
    // 方向相关
    uint8_t rotation;        // 旋转方向
} st7789_config_t;

/**
 * @brief 颜色定义 (RGB565格式)
 */
#define ST7789_BLACK       0x0000
#define ST7789_WHITE       0xFFFF
#define ST7789_RED         0xF800
#define ST7789_GREEN       0x07E0
#define ST7789_BLUE        0x001F
#define ST7789_YELLOW      0xFFE0
#define ST7789_CYAN        0x07FF
#define ST7789_MAGENTA     0xF81F

/**
 * @brief 初始化ST7789驱动
 * 
 * @param config LCD配置参数
 * @return bool 初始化是否成功
 */
bool st7789_init(const st7789_config_t *config);

/**
 * @brief 设置LCD背光
 * 
 * @param on 背光状态(true为开,false为关)
 */
void st7789_set_backlight(bool on);

/**
 * @brief 填充整个屏幕为指定颜色
 * 
 * @param color 填充颜色(RGB565格式)
 */
void st7789_fill_screen(uint16_t color);

/**
 * @brief 设置绘图区域
 * 
 * @param x0 起始X坐标
 * @param y0 起始Y坐标
 * @param x1 结束X坐标
 * @param y1 结束Y坐标
 */
void st7789_set_window(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);

/**
 * @brief 绘制单个像素
 * 
 * @param x X坐标
 * @param y Y坐标
 * @param color 像素颜色
 */
void st7789_draw_pixel(uint16_t x, uint16_t y, uint16_t color);

/**
 * @brief 发送块数据到LCD
 * 
 * @param data 数据指针
 * @param len 数据长度
 */
void st7789_write_data_buffer(const uint8_t *data, size_t len);

/**
 * @brief 设置显示方向
 * 
 * @param rotation 旋转值(0-3)
 */
void st7789_set_rotation(uint8_t rotation);

#endif /* _ST7789_H_ */ 
/**
 * @file st7789.c
 * @brief ST7789 LCD驱动实现
 */

#include "st7789.h"
#include "st7789_hal.h"
#include <stdio.h>

// 静态配置
static struct {
    uint16_t width;
    uint16_t height;
    uint8_t rotation;
    bool is_initialized;
} st7789_config = {0};

/**
 * @brief ST7789初始化序列
 */
static void st7789_init_sequence(void) {
    printf("运行ST7789初始化序列...\n");
    
    // 复位后等待
    st7789_hal_delay_ms(120);
    
    // 基本初始化
    st7789_hal_write_cmd(0x01);    // 软件复位
    st7789_hal_delay_ms(120);      // 复位后必须等待
    
    st7789_hal_write_cmd(0x11);    // 退出睡眠模式
    st7789_hal_delay_ms(120);      // 等待退出睡眠
    
    // Memory Data Access Control
    st7789_hal_write_cmd(0x36);    // MADCTL
    st7789_hal_write_data(0x00);   // 设置为正常方向 (可能需要调整)
    
    // Interface Pixel Format
    st7789_hal_write_cmd(0x3A);
    st7789_hal_write_data(0x55);   // 65K of RGB interface (16bit/pixel)
    
    // 显示反转控制
    st7789_hal_write_cmd(0x21);    // 开启显示反转
    
    // 设置行列地址范围
    st7789_hal_write_cmd(0x2A);    // 列地址设置
    st7789_hal_write_data(0x00);
    st7789_hal_write_data(0x00);
    st7789_hal_write_data((st7789_config.width >> 8) & 0xFF);
    st7789_hal_write_data(st7789_config.width & 0xFF);
    
    st7789_hal_write_cmd(0x2B);    // 行地址设置
    st7789_hal_write_data(0x00);
    st7789_hal_write_data(0x00);
    st7789_hal_write_data((st7789_config.height >> 8) & 0xFF);
    st7789_hal_write_data(st7789_config.height & 0xFF);
    
    // 附加设置 - 根据不同型号可能需要调整
    st7789_hal_write_cmd(0xB2);    // Porch Setting
    st7789_hal_write_data(0x0C);
    st7789_hal_write_data(0x0C);
    st7789_hal_write_data(0x00);
    st7789_hal_write_data(0x33);
    st7789_hal_write_data(0x33);
    
    st7789_hal_write_cmd(0xB7);    // Gate Control
    st7789_hal_write_data(0x35);
    
    st7789_hal_write_cmd(0xBB);    // VCOM Setting
    st7789_hal_write_data(0x20);   // 0.9V
    
    st7789_hal_write_cmd(0xC0);    // LCM Control
    st7789_hal_write_data(0x2C);
    
    st7789_hal_write_cmd(0xC2);    // VDV and VRH Command Enable
    st7789_hal_write_data(0x01);
    
    st7789_hal_write_cmd(0xC3);    // VRH Set
    st7789_hal_write_data(0x12);
    
    st7789_hal_write_cmd(0xC4);    // VDV Set
    st7789_hal_write_data(0x20);
    
    st7789_hal_write_cmd(0xC6);    // Frame Rate Control in Normal Mode
    st7789_hal_write_data(0x0F);   // 60Hz
    
    st7789_hal_write_cmd(0xD0);    // Power Control 1
    st7789_hal_write_data(0xA4);
    st7789_hal_write_data(0xA1);
    
    st7789_hal_write_cmd(0xE0);    // Positive Voltage Gamma Control
    st7789_hal_write_data(0xD0);
    st7789_hal_write_data(0x08);
    st7789_hal_write_data(0x11);
    st7789_hal_write_data(0x08);
    st7789_hal_write_data(0x0C);
    st7789_hal_write_data(0x15);
    st7789_hal_write_data(0x39);
    st7789_hal_write_data(0x33);
    st7789_hal_write_data(0x50);
    st7789_hal_write_data(0x36);
    st7789_hal_write_data(0x13);
    st7789_hal_write_data(0x14);
    st7789_hal_write_data(0x29);
    st7789_hal_write_data(0x2D);
    
    st7789_hal_write_cmd(0xE1);    // Negative Voltage Gamma Control
    st7789_hal_write_data(0xD0);
    st7789_hal_write_data(0x08);
    st7789_hal_write_data(0x10);
    st7789_hal_write_data(0x08);
    st7789_hal_write_data(0x06);
    st7789_hal_write_data(0x06);
    st7789_hal_write_data(0x39);
    st7789_hal_write_data(0x44);
    st7789_hal_write_data(0x51);
    st7789_hal_write_data(0x0B);
    st7789_hal_write_data(0x16);
    st7789_hal_write_data(0x14);
    st7789_hal_write_data(0x2F);
    st7789_hal_write_data(0x31);
    
    st7789_hal_delay_ms(10);
    
    // 打开显示
    st7789_hal_write_cmd(0x29);    // 开启显示
    st7789_hal_delay_ms(10);
    
    printf("ST7789初始化序列完成\n");
}

/**
 * @brief 初始化ST7789驱动
 */
bool st7789_init(const st7789_config_t *config) {
    if (config == NULL) {
        printf("错误: 驱动初始化配置为空\n");
        return false;
    }
    
    printf("开始LCD初始化...\n");
    
    // 保存屏幕参数
    st7789_config.width = config->width;
    st7789_config.height = config->height;
    st7789_config.rotation = config->rotation;
    
    // 初始化硬件抽象层
    if (!st7789_hal_init(config)) {
        printf("错误: 硬件抽象层初始化失败\n");
        return false;
    }
    
    // 硬件复位
    printf("执行硬件复位...\n");
    st7789_hal_reset(false);   // 复位状态
    st7789_hal_delay_ms(200);  // 保持低电平200ms
    st7789_hal_reset(true);    // 释放复位
    st7789_hal_delay_ms(200);  // 复位后等待
    
    // 运行初始化序列
    st7789_init_sequence();
    
    // 设置旋转方向
    st7789_set_rotation(config->rotation);
    
    st7789_config.is_initialized = true;
    printf("LCD初始化完成\n");
    
    return true;
}

/**
 * @brief 设置LCD背光
 */
void st7789_set_backlight(bool on) {
    if (!st7789_config.is_initialized) return;
    
    st7789_hal_set_backlight(on);
}

/**
 * @brief 设置显示方向
 */
void st7789_set_rotation(uint8_t rotation) {
    if (!st7789_config.is_initialized) return;
    
    uint8_t madctl = 0;
    
    switch (rotation & 0x03) {
        case 0:  // 0度旋转
            madctl = 0x00;
            break;
        case 1:  // 90度旋转
            madctl = 0x60;
            break;
        case 2:  // 180度旋转
            madctl = 0xC0;
            break;
        case 3:  // 270度旋转
            madctl = 0xA0;
            break;
    }
    
    st7789_hal_write_cmd(0x36);
    st7789_hal_write_data(madctl);
    
    st7789_config.rotation = rotation & 0x03;
}

/**
 * @brief 设置绘图区域
 */
void st7789_set_window(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
    if (!st7789_config.is_initialized) return;
    
    // 边界检查
    if (x0 >= st7789_config.width) x0 = st7789_config.width - 1;
    if (y0 >= st7789_config.height) y0 = st7789_config.height - 1;
    if (x1 >= st7789_config.width) x1 = st7789_config.width - 1;
    if (y1 >= st7789_config.height) y1 = st7789_config.height - 1;
    
    // 设置列地址
    st7789_hal_write_cmd(0x2A);
    st7789_hal_write_data(x0 >> 8);
    st7789_hal_write_data(x0 & 0xFF);
    st7789_hal_write_data(x1 >> 8);
    st7789_hal_write_data(x1 & 0xFF);
    
    // 设置行地址
    st7789_hal_write_cmd(0x2B);
    st7789_hal_write_data(y0 >> 8);
    st7789_hal_write_data(y0 & 0xFF);
    st7789_hal_write_data(y1 >> 8);
    st7789_hal_write_data(y1 & 0xFF);
    
    // 准备写入RAM
    st7789_hal_write_cmd(0x2C);
}

/**
 * @brief 绘制单个像素
 */
void st7789_draw_pixel(uint16_t x, uint16_t y, uint16_t color) {
    if (!st7789_config.is_initialized) return;
    
    // 边界检查
    if (x >= st7789_config.width || y >= st7789_config.height) return;
    
    // 设置绘图窗口为单个像素
    st7789_set_window(x, y, x, y);
    
    // 写入颜色数据
    uint8_t data[2] = {color >> 8, color & 0xFF};
    st7789_hal_write_data_bulk(data, 2);
}

/**
 * @brief 填充整个屏幕为指定颜色
 */
void st7789_fill_screen(uint16_t color) {
    if (!st7789_config.is_initialized) return;
    
    // 设置绘图窗口为整个屏幕
    st7789_set_window(0, 0, st7789_config.width - 1, st7789_config.height - 1);
    
    // 准备颜色数据
    uint8_t hi = color >> 8;
    uint8_t lo = color & 0xFF;
    uint8_t data[2] = {hi, lo};
    
    // 填充像素
    for (uint32_t i = 0; i < (uint32_t)st7789_config.width * st7789_config.height; i++) {
        st7789_hal_write_data_bulk(data, 2);
    }
}

/**
 * @brief 发送块数据到LCD
 */
void st7789_write_data_buffer(const uint8_t *data, size_t len) {
    if (!st7789_config.is_initialized || data == NULL) return;
    
    st7789_hal_write_data_bulk(data, len);
} 
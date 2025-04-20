/**
 * @file lcd_demo.c
 * @brief ST7789 LCD驱动演示程序
 */

#include <stdio.h>
#include "pico/stdlib.h"
#include "st7789.h"
#include "st7789_gfx.h"

// 引脚定义
#define PIN_DIN   19  // SPI MOSI
#define PIN_SCK   18  // SPI SCK
#define PIN_CS    17  // SPI CS - 需要手动控制
#define PIN_DC    20  // Data/Command
#define PIN_RESET 15  // Reset
#define PIN_BL    10  // Backlight

// 屏幕参数
#define SCREEN_WIDTH  240
#define SCREEN_HEIGHT 320

// 演示静态图形
static void demo_static_graphics(void) {
    printf("绘制静态图形演示...\n");
    
    // 清屏为黑色
    st7789_fill_screen(ST7789_BLACK);
    sleep_ms(500);
    
    // 绘制一些简单图形
    printf("绘制矩形...\n");
    st7789_draw_rect(20, 20, 200, 120, ST7789_WHITE);
    st7789_fill_rect(50, 50, 140, 60, ST7789_RED);
    sleep_ms(1000);
    
    printf("绘制线条...\n");
    st7789_draw_line(20, 160, 220, 220, ST7789_GREEN);
    st7789_draw_line(220, 160, 20, 220, ST7789_GREEN);
    sleep_ms(1000);
    
    printf("绘制圆形...\n");
    st7789_draw_circle(120, 280, 30, ST7789_BLUE);
    st7789_fill_circle(120, 280, 15, ST7789_YELLOW);
    sleep_ms(1000);
    
    // 绘制文本
    printf("绘制文本...\n");
    st7789_draw_string(40, 150, "ST7789 LCD Test", ST7789_CYAN, ST7789_BLACK, 2);
    st7789_draw_string(40, 180, "Raspberry Pi Pico", ST7789_MAGENTA, ST7789_BLACK, 2);
    
    sleep_ms(2000);
}

// 演示动态彩色图案
static void demo_color_animation(void) {
    printf("绘制动态彩色动画...\n");
    
    uint32_t t = 0;
    uint32_t start_time = to_ms_since_boot(get_absolute_time());
    uint32_t frames = 0;
    
    // 运行动画10秒
    while (to_ms_since_boot(get_absolute_time()) - start_time < 10000) {
        uint8_t r, g, b;
        uint16_t color;
        
        // 设置绘图区域为整个屏幕
        st7789_set_window(0, 0, SCREEN_WIDTH - 1, SCREEN_HEIGHT - 1);
        
        // 绘制动态彩色图案
        for (uint16_t y = 0; y < SCREEN_HEIGHT; y++) {
            for (uint16_t x = 0; x < SCREEN_WIDTH; x++) {
                r = (x + t) & 0x1F;
                g = (y - t) & 0x3F;
                b = (x + y + (t >> 1)) & 0x1F;
                
                color = ((r & 0x1F) << 11) | ((g & 0x3F) << 5) | (b & 0x1F);
                uint8_t data[2] = {color >> 8, color & 0xFF};
                st7789_write_data_buffer(data, 2);
            }
        }
        
        t++;
        frames++;
        
        if (frames % 10 == 0) {
            printf("已完成%lu帧\n", frames);
        }
    }
    
    float fps = (float)frames / 10.0f;
    printf("动画完成：%lu帧，平均%.1f FPS\n", frames, fps);
}

int main() {
    // 初始化标准库
    stdio_init_all();
    sleep_ms(3000);  // 等待串口初始化完成
    printf("\n\n\n开始ST7789 LCD演示程序...\n");
    
    // 配置LCD
    st7789_config_t config = {
        .spi_inst = spi0,
        .spi_speed_hz = 40 * 1000 * 1000,  // 40MHz
        
        .pin_din = PIN_DIN,
        .pin_sck = PIN_SCK,
        .pin_cs = PIN_CS,
        .pin_dc = PIN_DC,
        .pin_reset = PIN_RESET,
        .pin_bl = PIN_BL,
        
        .width = SCREEN_WIDTH,
        .height = SCREEN_HEIGHT,
        .rotation = 0,  // 0度旋转
    };
    
    // 初始化LCD
    if (!st7789_init(&config)) {
        printf("错误: LCD初始化失败\n");
        return -1;
    }
    
    // 开启背光
    printf("打开背光...\n");
    st7789_set_backlight(true);
    sleep_ms(500);
    
    // 运行静态图形演示
    demo_static_graphics();
    
    // 运行动态彩色动画演示
    demo_color_animation();
    
    printf("演示完成。\n");
    
    // 保持屏幕显示最后一帧
    while (true) {
        sleep_ms(1000);
    }
    
    return 0;
} 
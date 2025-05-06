#include <stdio.h>
#include "pico/stdlib.h"
#include "st7789.hpp"

// 定义一个颜色渐变图像
#define IMAGE_WIDTH 100
#define IMAGE_HEIGHT 100
uint16_t gradient_image[IMAGE_WIDTH * IMAGE_HEIGHT];

void generate_gradient() {
    for (int y = 0; y < IMAGE_HEIGHT; y++) {
        for (int x = 0; x < IMAGE_WIDTH; x++) {
            uint8_t r = (uint8_t)((float)x / IMAGE_WIDTH * 255);
            uint8_t g = (uint8_t)((float)y / IMAGE_HEIGHT * 255);
            uint8_t b = 255 - ((r + g) / 2);
            gradient_image[y * IMAGE_WIDTH + x] = st7789::ST7789::color565(r, g, b);
        }
    }
}

int main() {
    stdio_init_all();
    printf("ST7789 LCD DMA演示程序\n");
    
    // 生成渐变图像
    generate_gradient();
    
    // 创建LCD对象
    st7789::ST7789 lcd;
    
    // 配置并初始化LCD
    st7789::Config config;
    config.spi_inst = spi0;
    config.pin_din = 19;    // MOSI
    config.pin_sck = 18;    // SCK
    config.pin_cs = 17;     // CS
    config.pin_dc = 20;     // DC
    config.pin_reset = 15;  // RESET
    config.pin_bl = 10;     // 背光
    config.width = 240;
    config.height = 320;
    config.rotation = st7789::ROTATION_0;
    
    // DMA配置
    config.dma.enabled = true;
    config.dma.buffer_size = 4096;
    
    // 初始化LCD
    if (!lcd.begin(config)) {
        printf("LCD初始化失败！\n");
        return -1;
    }
    
    printf("LCD初始化成功！DMA状态: %s\n", 
           lcd.isDmaEnabled() ? "已启用" : "未启用");
    
    // 使用DMA填充屏幕
    printf("使用DMA填充屏幕...\n");
    uint32_t start_time = to_ms_since_boot(get_absolute_time());
    lcd.fillRectDMA(0, 0, config.width, config.height, st7789::BLUE);
    uint32_t end_time = to_ms_since_boot(get_absolute_time());
    printf("DMA填充耗时: %ldms\n", end_time - start_time);
    
    // 绘制一些常规图形
    lcd.drawRect(10, 10, 100, 50, st7789::RED);
    lcd.fillRect(120, 10, 100, 50, st7789::GREEN);
    lcd.drawCircle(60, 100, 30, st7789::YELLOW);
    lcd.fillCircle(170, 100, 30, st7789::MAGENTA);
    
    // 绘制一些文本
    lcd.drawString(50, 150, "ST7789 LCD With DMA", st7789::WHITE, st7789::BLACK, 1);
    lcd.drawString(50, 170, "Performance Boost!", st7789::WHITE, st7789::BLACK, 1);
    
    // DMA绘制渐变图像
    printf("使用DMA绘制图像...\n");
    start_time = to_ms_since_boot(get_absolute_time());
    lcd.drawImageDMA(70, 200, IMAGE_WIDTH, IMAGE_HEIGHT, gradient_image);
    end_time = to_ms_since_boot(get_absolute_time());
    printf("DMA图像绘制耗时: %ldms\n", end_time - start_time);
    
    // 主循环 - 演示DMA动画
    int frame_count = 0;
    start_time = to_ms_since_boot(get_absolute_time());
    
    while (true) {
        // 每帧更新渐变色
        for (int i = 0; i < IMAGE_WIDTH * IMAGE_HEIGHT; i++) {
            uint16_t color = gradient_image[i];
            // 旋转颜色分量
            uint8_t r = (color >> 11) & 0x1F;
            uint8_t g = (color >> 5) & 0x3F;
            uint8_t b = color & 0x1F;
            
            // 调整颜色
            r = (r + 1) % 32;
            b = (b + 1) % 32;
            
            gradient_image[i] = (r << 11) | (g << 5) | b;
        }
        
        // 使用DMA绘制渐变
        lcd.drawImageDMA(70, 200, IMAGE_WIDTH, IMAGE_HEIGHT, gradient_image);
        
        frame_count++;
        // 每5秒输出帧率
        if (frame_count >= 100) {
            uint32_t current_time = to_ms_since_boot(get_absolute_time());
            float fps = frame_count * 1000.0f / (current_time - start_time);
            printf("帧率: %.2f FPS\n", fps);
            
            frame_count = 0;
            start_time = current_time;
        }
        
        sleep_ms(10);
    }

    return 0;
} 
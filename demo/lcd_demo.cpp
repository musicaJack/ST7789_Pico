#include <stdio.h>
#include "pico/stdlib.h"
#include "st7789.hpp"

// 函数声明
void drawTestPattern(st7789::ST7789* lcd);

int main() {
    stdio_init_all();
    printf("ST7789 LCD 演示程序\n");
    
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
    config.rotation = st7789::ROTATION_0;  // 默认90度旋转
    
    // 初始化LCD
    if (!lcd.begin(config)) {
        printf("LCD初始化失败！\n");
        return -1;
    }
    
    printf("LCD初始化成功！\n");
    
    // 多次清空屏幕以确保完全黑色
    for (int i = 0; i < 3; i++) {
        lcd.fillScreen(st7789::BLACK);
        sleep_ms(50);
    }
    
    // 绘制基本界面
    drawTestPattern(&lcd);
    
    // 主循环 - 每5秒切换一次旋转方向
    uint8_t current_rotation = st7789::ROTATION_90;
    uint32_t last_rotation_change = to_ms_since_boot(get_absolute_time());
    
    // 动画变量
    int prev_x = 0;
    int x = 0;
    
    while (true) {
        // 检查是否需要更改旋转方向
        uint32_t current_time = to_ms_since_boot(get_absolute_time());
        if (current_time - last_rotation_change > 5000) { // 每5秒
            // 切换到下一个旋转方向
            current_rotation = (current_rotation + 1) % 4;
            printf("切换到旋转方向: %d\n", current_rotation);
            
            // 先设置回原始分辨率（240x320）进行彻底清屏
            lcd.setRotation(st7789::ROTATION_0);
            lcd.fillScreen(st7789::BLACK);
            
            // 重新设置多次清屏以确保完全清除
            for (int i = 0; i < 3; i++) {
                lcd.fillScreen(st7789::BLACK);
                sleep_ms(5);
            }
            
            // 分段清屏以确保全部区域都被清除
            for (int y = 0; y < 320; y += 20) {
                lcd.fillRect(0, y, 240, 20, st7789::BLACK);
            }
            
            // 设置新的旋转方向
            lcd.setRotation((st7789::Rotation)current_rotation);
            
            // 重新绘制测试图案
            drawTestPattern(&lcd);
            
            // 更新时间戳
            last_rotation_change = current_time;
        }
        
        // 绘制动画效果
        // 每帧都清除动画区域
        if (x % 10 == 0) {
            lcd.fillRect(0, 220, 240, 20, st7789::BLACK);
        }
        
        // 绘制新位置的方块
        lcd.fillRect(x, 220, 20, 20, st7789::RED);
        
        // 清除之前位置的方块
        if (prev_x != x) {
            lcd.fillRect(prev_x, 220, 20, 20, st7789::BLACK);
        }
        
        prev_x = x;
        x = (x + 1) % 240;
        
        sleep_ms(10);
    }

    return 0;
}

// 绘制测试图案
void drawTestPattern(st7789::ST7789* lcd) {
    // 绘制一些图形，使用红色、绿色、蓝色、黄色
    lcd->drawRect(10, 10, 100, 50, st7789::RED);     // 红色矩形框
    lcd->fillRect(120, 10, 100, 50, st7789::GREEN);  // 绿色填充矩形
    
    lcd->drawCircle(60, 100, 30, st7789::BLUE);      // 蓝色圆形
    lcd->fillCircle(170, 100, 30, st7789::YELLOW);   // 黄色填充圆形
    
    // 绘制方向指示器 - 使用drawTriangle替代fillTriangle
    lcd->drawTriangle(120, 160, 100, 190, 140, 190, st7789::MAGENTA);  // 上箭头
    
    // 绘制一些文本
    lcd->drawString(50, 150, "ST7789 LCD", st7789::WHITE, st7789::BLACK, 2);
    
    // 显示当前旋转角度文本
    char rotation_text[20];
    snprintf(rotation_text, sizeof(rotation_text), "Rotation: %d", lcd->getRotation());
    lcd->drawString(50, 180, rotation_text, st7789::WHITE, st7789::BLACK, 1);
} 
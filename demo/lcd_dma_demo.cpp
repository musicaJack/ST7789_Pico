#include <stdio.h>
#include "pico/stdlib.h"
#include "st7789.hpp"


int main() {
    stdio_init_all();
    printf("ST7789 LCD DMA演示程序\n");
    
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
    
    // 1. 清屏
    lcd.clearScreen();
    
    // 旋转180度
    uint8_t current_rotation = st7789::ROTATION_180;
    lcd.setRotation((st7789::Rotation)current_rotation);

    // 2. 显示文字 "ST7789 with DMA transfer"
    lcd.drawString(20, 150, "ST7789 with DMA transfer", st7789::WHITE, st7789::BLACK, 2);
    sleep_ms(5000);  // 停留5秒
    
    // 3. 清屏
    lcd.clearScreen();
    
    // 4. 在屏幕中央画红色圆圈
    int center_x = config.width / 2;
    int center_y = config.height / 2;
    lcd.fillCircle(center_x, center_y, 15, st7789::RED);
    sleep_ms(3000);  // 停留3秒
    
    // 5. 红色圆圈变成蓝色圆圈
    lcd.fillCircle(center_x, center_y, 15, st7789::BLUE);
    sleep_ms(3000);  // 停留3秒
    
    // 6. 蓝色圆圈变成绿色圆圈
    lcd.fillCircle(center_x, center_y, 15, st7789::GREEN);
    sleep_ms(3000);  // 停留3秒
    
    // 7. 清屏
    lcd.clearScreen();
    
    // 8. 显示文字 "DEMO Finished"
    lcd.drawString(50, 150, "DEMO Finished", st7789::WHITE, st7789::BLACK, 2);
    sleep_ms(5000);  // 停留5秒
    
    // 程序结束
    printf("演示程序结束\n");
    
    return 0;
} 
#include <stdio.h>
#include "pico/stdlib.h"
#include "st7789.hpp"

// 绘制老式电视机测试图案
void drawTVTestPattern(st7789::ST7789& lcd, uint16_t width, uint16_t height) {
    // 1. 绘制彩色条纹
    const int stripe_height = height / 8;  // 将屏幕分成8个水平条纹
    uint16_t colors[] = {
        st7789::WHITE,    // 白色
        st7789::YELLOW,   // 黄色
        st7789::CYAN,     // 青色
        st7789::GREEN,    // 绿色
        st7789::MAGENTA,  // 品红
        st7789::RED,      // 红色
        st7789::BLUE,     // 蓝色
        st7789::BLACK     // 黑色
    };
    
    // 绘制水平条纹
    for (int i = 0; i < 8; i++) {
        lcd.fillRect(0, i * stripe_height, width, stripe_height, colors[i]);
    }
    
    // 2. 绘制中心十字线
    int center_x = width / 2;
    int center_y = height / 2;
    int line_length = width / 12;  // 十字线长度根据屏幕宽度自适应
    
    // 水平线
    lcd.fillRect(center_x - line_length, center_y, line_length * 2, 2, st7789::WHITE);
    // 垂直线
    lcd.fillRect(center_x, center_y - line_length, 2, line_length * 2, st7789::WHITE);
    
    // 3. 绘制边框
    int border_width = width / 60;  // 边框宽度根据屏幕宽度自适应
    // 外边框
    lcd.drawRect(border_width, border_width, 
                width - border_width * 2, height - border_width * 2, 
                st7789::WHITE);
    
    // 4. 绘制四个角落的方块
    int corner_size = width / 12;  // 角落方块大小根据屏幕宽度自适应
    // 左上角
    lcd.fillRect(border_width * 2, border_width * 2, 
                corner_size, corner_size, st7789::WHITE);
    // 右上角
    lcd.fillRect(width - border_width * 2 - corner_size, border_width * 2, 
                corner_size, corner_size, st7789::WHITE);
    // 左下角
    lcd.fillRect(border_width * 2, height - border_width * 2 - corner_size, 
                corner_size, corner_size, st7789::WHITE);
    // 右下角
    lcd.fillRect(width - border_width * 2 - corner_size, 
                height - border_width * 2 - corner_size, 
                corner_size, corner_size, st7789::WHITE);
    
    // 5. 在底部显示文字和旋转角度
    char text[32];
    snprintf(text, sizeof(text), "LCD TEST - ROT: %d", lcd.getRotation());
    lcd.drawString(width/2 - 60, height - 30, text, st7789::WHITE, st7789::BLACK, 2);
}

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
    config.rotation = st7789::ROTATION_0;
    
    // 初始化LCD
    if (!lcd.begin(config)) {
        printf("LCD初始化失败！\n");
        return -1;
    }
    
    printf("LCD初始化成功！\n");
    
    // 初始清屏
    lcd.clearScreen();
    sleep_ms(1000);
    
    // 主循环
    uint8_t current_rotation = st7789::ROTATION_0;
    uint32_t last_rotation_change = to_ms_since_boot(get_absolute_time());
    
    while (true) {
        // 检查是否需要更改旋转方向
        uint32_t current_time = to_ms_since_boot(get_absolute_time());
        if (current_time - last_rotation_change > 5000) { // 每5秒
            // 切换到下一个旋转方向
            current_rotation = (current_rotation + 1) % 4;
            printf("切换到旋转方向: %d\n", current_rotation);
            
            // 设置新的旋转方向
            lcd.setRotation((st7789::Rotation)current_rotation);
            
            // 清屏
            lcd.clearScreen();
            
            // 获取当前旋转后的实际尺寸
            uint16_t current_width = (current_rotation % 2 == 0) ? config.width : config.height;
            uint16_t current_height = (current_rotation % 2 == 0) ? config.height : config.width;
            
            // 重新绘制测试图案
            drawTVTestPattern(lcd, current_width, current_height);
            
            // 更新时间戳
            last_rotation_change = current_time;
        }
        
        sleep_ms(100);  // 短暂延时，减少CPU使用
    }
    
    return 0;
} 
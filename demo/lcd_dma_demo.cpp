#include <stdio.h>
#include "pico/stdlib.h"
#include "st7789.hpp"

int main() {
    stdio_init_all();
    printf("ST7789 LCD DMA Text Demo\n");
    
    // Create LCD object
    st7789::ST7789 lcd;
    
    // Configure and initialize LCD
    st7789::Config config;
    config.spi_inst = spi0;
    config.pin_din = 19;    // MOSI
    config.pin_sck = 18;    // SCK
    config.pin_cs = 17;     // CS
    config.pin_dc = 20;     // DC
    config.pin_reset = 15;  // RESET
    config.pin_bl = 10;     // Backlight
    config.width = 240;
    config.height = 320;
    config.rotation = st7789::ROTATION_0;
    
    // DMA
    config.dma.enabled = true;
    config.dma.buffer_size = 480; // 一行像素的字节数
    
    // Initialize LCD
    if (!lcd.begin(config)) {
        printf("LCD initialization failed!\n");
        return -1;
    }
    
    printf("LCD initialization successful! DMA status: %s\n",
        lcd.isDmaEnabled() ? "Enabled" : "Disabled");
    
    // 旋转
    uint8_t current_rotation = st7789::ROTATION_180;
    lcd.setRotation((st7789::Rotation)current_rotation);
    
    // Clear screen
    printf("Clearing screen...\n");
    lcd.clearScreen();
    sleep_ms(1000);
    
    // 显示欢迎信息
    lcd.drawString(10, 10, "ST7789 LCD Demo", st7789::WHITE, st7789::BLACK, 2);
    lcd.drawString(10, 40, "DMA Text Display", st7789::GREEN, st7789::BLACK, 2);
    
    // 显示一些示例文字
    lcd.drawString(10, 80, "Hello World!", st7789::RED, st7789::BLACK, 2);
    lcd.drawString(10, 110, "Raspberry Pi Pico", st7789::BLUE, st7789::BLACK, 2);
    lcd.drawString(10, 140, "ST7789 Driver", st7789::YELLOW, st7789::BLACK, 2);
    
    // 显示DMA状态
    char dma_status[32];
    snprintf(dma_status, sizeof(dma_status), "DMA: %s", 
             lcd.isDmaEnabled() ? "Enabled" : "Disabled");
    lcd.drawString(10, 180, dma_status, st7789::CYAN, st7789::BLACK, 2);
    
    // 显示一些动态文字
    uint32_t last_update = to_ms_since_boot(get_absolute_time());
    uint8_t counter = 0;
    
    while (true) {
        // 每秒更新一次计数器
        if (to_ms_since_boot(get_absolute_time()) - last_update >= 1000) {
            char counter_text[32];
            snprintf(counter_text, sizeof(counter_text), "Counter: %d", counter++);
            lcd.drawString(10, 220, counter_text, st7789::MAGENTA, st7789::BLACK, 2);
            last_update = to_ms_since_boot(get_absolute_time());

        }
        
        sleep_ms(10);
    }
    
    return 0;
} 
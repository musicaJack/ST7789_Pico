/**
 * @file lcd_demo.c
 * @brief ST7789 LCD Driver Demo Program
 */

#include <stdio.h>
#include "pico/stdlib.h"
#include "st7789.h"
#include "st7789_gfx.h"
#include "imgs/mount.h"  // 包含图像数据

// Pin definitions
#define PIN_DIN   19  // SPI MOSI
#define PIN_SCK   18  // SPI SCK
#define PIN_CS    17  // SPI CS - manually controlled
#define PIN_DC    20  // Data/Command
#define PIN_RESET 15  // Reset
#define PIN_BL    10  // Backlight

// Screen parameters
#define SCREEN_WIDTH  240
#define SCREEN_HEIGHT 320

// Demo for static graphics
static void demo_static_graphics(void) {
    printf("Running static graphics demo...\n");
    
    // Clear screen to black
    st7789_fill_screen(ST7789_BLACK);
    sleep_ms(500);
    
    // Draw some simple shapes
    printf("Drawing rectangles...\n");
    st7789_draw_rect(20, 20, 200, 120, ST7789_WHITE);
    st7789_fill_rect(50, 50, 140, 60, ST7789_RED);
    sleep_ms(1000);
    
    printf("Drawing lines...\n");
    st7789_draw_line(20, 160, 220, 220, ST7789_GREEN);
    st7789_draw_line(220, 160, 20, 220, ST7789_GREEN);
    sleep_ms(1000);
    
    printf("Drawing circles...\n");
    st7789_draw_circle(120, 280, 30, ST7789_BLUE);
    st7789_fill_circle(120, 280, 15, ST7789_YELLOW);
    sleep_ms(1000);
    
    // Draw text
    printf("Drawing text...\n");
    st7789_draw_string(40, 150, "ST7789 LCD Test", ST7789_CYAN, ST7789_BLACK, 2);
    st7789_draw_string(40, 180, "Raspberry Pi Pico", ST7789_MAGENTA, ST7789_BLACK, 2);
    
    sleep_ms(5000);
}

// Demo for image display
static void demo_image_display(void) {
    printf("Running image display demo...\n");
    
    // Clear screen to black
    st7789_fill_screen(ST7789_BLACK);
    sleep_ms(500);
    
    printf("Displaying image...\n");
    
    // 设置显示窗口为整个屏幕
    st7789_set_window(0, 0, SCREEN_WIDTH - 1, SCREEN_HEIGHT - 1);
    
    // 直接访问图像数据
    const uint16_t* img_ptr = image_data;
    uint8_t buffer[2];
    
    // 从mount.h获取图像原始尺寸
    #define IMAGE_WIDTH 240
    #define IMAGE_HEIGHT 320
    
    // 发送图像数据到LCD
    printf("Sending image data to LCD...\n");
    
    // 对于90度旋转，需要调整遍历顺序
    for (uint16_t x = 0; x < SCREEN_WIDTH; x++) {
        for (uint16_t y = 0; y < SCREEN_HEIGHT; y++) {
            // 对于90度旋转，新坐标(x,y)对应原图像的(y,IMAGE_WIDTH-1-x)
            uint32_t orig_x = y;
            uint32_t orig_y = IMAGE_WIDTH - 1 - x;
            uint32_t index = orig_y * IMAGE_WIDTH + orig_x;
            
            // 获取色彩数据
            uint16_t color = img_ptr[index];
            
            // 字节序转换
            uint16_t color_fixed = ((color & 0xFF) << 8) | ((color & 0xFF00) >> 8);
            
            // 发送到LCD
            buffer[0] = color_fixed >> 8;
            buffer[1] = color_fixed & 0xFF;
            
            st7789_write_data_buffer(buffer, 2);
        }
    }
    
    printf("Image display completed.\n");
    sleep_ms(8000);  // 显示8秒
}

// Demo for dynamic color animation
static void demo_color_animation(void) {
    printf("Running color animation demo...\n");
    
    uint32_t t = 0;
    uint32_t start_time = to_ms_since_boot(get_absolute_time());
    uint32_t frames = 0;
    
    // Run animation for 10 seconds
    while (to_ms_since_boot(get_absolute_time()) - start_time < 10000) {
        uint8_t r, g, b;
        uint16_t color;
        
        // Set drawing window to entire screen
        st7789_set_window(0, 0, SCREEN_WIDTH - 1, SCREEN_HEIGHT - 1);
        
        // Draw dynamic color pattern
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
            printf("Completed %lu frames\n", frames);
        }
    }
    
    float fps = (float)frames / 10.0f;
    printf("Animation completed: %lu frames, average %.1f FPS\n", frames, fps);
}

int main() {
    // Initialize stdlib
    stdio_init_all();
    sleep_ms(3000);  // Wait for serial to initialize
    printf("\n\n\nStarting ST7789 LCD Demo Program...\n");
    
    // Configure LCD
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
        .rotation = 0,  // 90 degree rotation
    };
    
    // Initialize LCD
    if (!st7789_init(&config)) {
        printf("Error: LCD initialization failed\n");
        return -1;
    }
    
    // Turn on backlight
    printf("Turning on backlight...\n");
    st7789_set_backlight(true);
    sleep_ms(500);

    // Rotate the screen 180 degrees
    printf("Rotating screen 180 degrees...\n");
    st7789_set_rotation(2);
    
    // Run static graphics demo
    printf("Running static graphics demo...\n");
    demo_static_graphics();
    
    // Run image display demo
    printf("Running image display demo...\n");
    demo_image_display();
    
    // Run dynamic color animation demo
    printf("Running dynamic color animation demo...\n");
    demo_color_animation();
    
    st7789_fill_screen(ST7789_BLACK);
    st7789_draw_string(40, 150, "Demo completed.", ST7789_WHITE, ST7789_BLACK, 2);
    printf("Demo completed.\n");
    
    // Keep displaying the last frame
    while (true) {
        sleep_ms(1000);
    }
    
    return 0;
} 
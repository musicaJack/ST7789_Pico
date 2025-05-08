#include <stdio.h>
#include "pico/stdlib.h"
#include "st7789.hpp"

// Draw old TV test pattern
void drawTVTestPattern(st7789::ST7789& lcd, uint16_t width, uint16_t height) {
    // 1. Draw color stripes
    const int stripe_height = height / 8;  // Divide screen into 8 horizontal stripes
    uint16_t colors[] = {
        st7789::WHITE,    // White
        st7789::YELLOW,   // Yellow
        st7789::CYAN,     // Cyan
        st7789::GREEN,    // Green
        st7789::MAGENTA,  // Magenta
        st7789::RED,      // Red
        st7789::BLUE,     // Blue
        st7789::BLACK     // Black
    };
    
    // Draw horizontal stripes
    for (int i = 0; i < 8; i++) {
        lcd.fillRect(0, i * stripe_height, width, stripe_height, colors[i]);
    }
    
    // 2. Draw center crosshair
    int center_x = width / 2;
    int center_y = height / 2;
    int line_length = width / 12;  // Crosshair length adapts to screen width
    
    // Horizontal line
    lcd.fillRect(center_x - line_length, center_y, line_length * 2, 2, st7789::WHITE);
    // Vertical line
    lcd.fillRect(center_x, center_y - line_length, 2, line_length * 2, st7789::WHITE);
    
    // 3. Draw border
    int border_width = width / 60;  // Border width adapts to screen width
    // Outer border
    lcd.drawRect(border_width, border_width, 
                width - border_width * 2, height - border_width * 2, 
                st7789::WHITE);
    
    // 4. Draw corner squares
    int corner_size = width / 12;  // Corner square size adapts to screen width
    // Top-left corner
    lcd.fillRect(border_width * 2, border_width * 2, 
                corner_size, corner_size, st7789::WHITE);
    // Top-right corner
    lcd.fillRect(width - border_width * 2 - corner_size, border_width * 2, 
                corner_size, corner_size, st7789::WHITE);
    // Bottom-left corner
    lcd.fillRect(border_width * 2, height - border_width * 2 - corner_size, 
                corner_size, corner_size, st7789::WHITE);
    // Bottom-right corner
    lcd.fillRect(width - border_width * 2 - corner_size, 
                height - border_width * 2 - corner_size, 
                corner_size, corner_size, st7789::WHITE);
    
    // 5. Display text and rotation angle at bottom
    char text[32];
    snprintf(text, sizeof(text), "LCD TEST - ROT: %d", lcd.getRotation());
    lcd.drawString(width/2 - 60, height - 30, text, st7789::WHITE, st7789::BLACK, 2);
}

int main() {
    stdio_init_all();
    printf("ST7789 LCD Demo Program\n");
    
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
    
    // Initialize LCD
    if (!lcd.begin(config)) {
        printf("LCD initialization failed!\n");
        return -1;
    }
    
    printf("LCD initialization successful!\n");
    
    // Initial clear screen
    lcd.clearScreen();
    sleep_ms(200);
    
    // Main loop
    uint8_t current_rotation = st7789::ROTATION_0;
    uint32_t last_rotation_change = to_ms_since_boot(get_absolute_time());
    
    while (true) {
        // Check if rotation needs to be changed
        uint32_t current_time = to_ms_since_boot(get_absolute_time());
        if (current_time - last_rotation_change > 5000) { // Every 5 seconds
            // Switch to next rotation
            current_rotation = (current_rotation + 1) % 4;
            printf("Switching to rotation: %d\n", current_rotation);
            
            // Set new rotation
            lcd.setRotation((st7789::Rotation)current_rotation);
            
            // Clear screen
            lcd.clearScreen();
            
            // Get current dimensions after rotation
            uint16_t current_width = (current_rotation % 2 == 0) ? config.width : config.height;
            uint16_t current_height = (current_rotation % 2 == 0) ? config.height : config.width;
            
            // Redraw test pattern
            drawTVTestPattern(lcd, current_width, current_height);
            
            // Update timestamp
            last_rotation_change = current_time;
        }
        
        sleep_ms(100);  // Short delay to reduce CPU usage
    }
    
    return 0;
} 
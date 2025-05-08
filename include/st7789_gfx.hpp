#pragma once

#include <cstdint>
#include "st7789_config.hpp"

namespace st7789 {

// Forward declaration
class ST7789;

// Graphics class - handles drawing operations
class Graphics {
private:
    ST7789* _lcd; // Reference to main LCD class
    
public:
    Graphics(ST7789* lcd);
    virtual ~Graphics();
    
    // Basic drawing functions
    void drawPixel(int16_t x, int16_t y, uint16_t color);
    void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color);
    void drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
    void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
    void drawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color);
    void fillCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color);
    void drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color);
    
    // Text functions
    void drawChar(int16_t x, int16_t y, char c, uint16_t color, uint16_t bg, uint8_t size);
    void drawString(int16_t x, int16_t y, const char* str, uint16_t color, uint16_t bg, uint8_t size);
    
    // Image drawing
    void drawImage(int16_t x, int16_t y, int16_t w, int16_t h, const uint16_t* data);
    
    // Clear screen function
    void clearScreen(uint16_t width, uint16_t height, uint16_t color = BLACK);
    
    // Helper functions
    static uint16_t color565(uint8_t r, uint8_t g, uint8_t b);
};

} // namespace st7789 
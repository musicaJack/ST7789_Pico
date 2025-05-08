#pragma once

#include "st7789_config.hpp"
#include "st7789_hal.hpp"
#include "st7789_gfx.hpp"

namespace st7789 {

// Main LCD control class
class ST7789 {
private:
    HAL _hal;                   // Hardware abstraction layer
    Graphics _gfx;              // Graphics functionality
    bool _initialized;          // Initialization flag
    
    // Internal functions
    void initializeDisplay();
    void setAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
    
public:
    ST7789();
    virtual ~ST7789();
    
    // Display initialization
    bool begin(const Config& config = Config());
    bool begin(spi_inst_t* spi, uint8_t cs_pin, uint8_t dc_pin, 
              uint8_t rst_pin, uint8_t bl_pin = 10,
              uint16_t width = 240, uint16_t height = 320);
    
    // Display control
    void setRotation(Rotation rotation);
    Rotation getRotation() { return _hal.getConfig().rotation; }  // Get current rotation angle
    void invertDisplay(bool invert);
    void fillScreen(uint16_t color);
    void sleepDisplay(bool sleep);
    
    // Screen clearing
    void clearScreen(uint16_t color = BLACK) { _gfx.clearScreen(_hal.getConfig().width, _hal.getConfig().height, color); }
    
    // DMA related functions
    bool isDmaEnabled() const { return _hal.isDmaEnabled(); }
    bool isDmaBusy() const { return _hal.isDmaBusy(); }
    
    // Efficient drawing functions using DMA
    bool drawImageDMA(int16_t x, int16_t y, int16_t w, int16_t h, const uint16_t* data);
    bool fillRectDMA(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
    
    // Hardware control
    void setBacklight(bool on);
    void setBrightness(uint8_t brightness);
    void reset();
    
    // Access to other components
    Graphics& graphics() { return _gfx; }
    HAL& hal() { return _hal; }
    
    // Convenient drawing functions (passed to graphics class)
    void drawPixel(int16_t x, int16_t y, uint16_t color) { _gfx.drawPixel(x, y, color); }
    void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color) { _gfx.drawLine(x0, y0, x1, y1, color); }
    void drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) { _gfx.drawRect(x, y, w, h, color); }
    void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) { _gfx.fillRect(x, y, w, h, color); }
    void drawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color) { _gfx.drawCircle(x0, y0, r, color); }
    void fillCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color) { _gfx.fillCircle(x0, y0, r, color); }
    void drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color) { _gfx.drawTriangle(x0, y0, x1, y1, x2, y2, color); }
    void drawChar(int16_t x, int16_t y, char c, uint16_t color, uint16_t bg, uint8_t size) { _gfx.drawChar(x, y, c, color, bg, size); }
    void drawString(int16_t x, int16_t y, const char* str, uint16_t color, uint16_t bg, uint8_t size) { _gfx.drawString(x, y, str, color, bg, size); }
    void drawImage(int16_t x, int16_t y, int16_t w, int16_t h, const uint16_t* data) { _gfx.drawImage(x, y, w, h, data); }
    
    // Static helper functions
    static uint16_t color565(uint8_t r, uint8_t g, uint8_t b) { return Graphics::color565(r, g, b); }
    
    // Friend declarations
    friend class Graphics;
};

} // namespace st7789 
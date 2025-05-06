#pragma once

#include "st7789_config.hpp"
#include "st7789_hal.hpp"
#include "st7789_gfx.hpp"

namespace st7789 {

// 主LCD控制类
class ST7789 {
private:
    HAL _hal;                   // 硬件抽象层
    Graphics _gfx;              // 图形功能
    bool _initialized;          // 初始化标志
    
    // 内部函数
    void initializeDisplay();
    void setAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
    
public:
    ST7789();
    virtual ~ST7789();
    
    // 初始化显示屏
    bool begin(const Config& config = Config());
    bool begin(spi_inst_t* spi, uint8_t cs_pin, uint8_t dc_pin, 
              uint8_t rst_pin, uint8_t bl_pin = 10,
              uint16_t width = 240, uint16_t height = 320);
    
    // 显示控制
    void setRotation(Rotation rotation);
    Rotation getRotation() { return _hal.getConfig().rotation; }  // 获取当前旋转角度
    void invertDisplay(bool invert);
    void fillScreen(uint16_t color);
    void sleepDisplay(bool sleep);
    
    // DMA相关功能
    bool isDmaEnabled() const { return _hal.isDmaEnabled(); }
    bool isDmaBusy() const { return _hal.isDmaBusy(); }
    
    // 使用DMA的高效绘图函数
    bool drawImageDMA(int16_t x, int16_t y, int16_t w, int16_t h, const uint16_t* data);
    bool fillRectDMA(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
    
    // 硬件控制
    void setBacklight(bool on);
    void setBrightness(uint8_t brightness);
    void reset();
    
    // 访问其他组件
    Graphics& graphics() { return _gfx; }
    HAL& hal() { return _hal; }
    
    // 方便的绘图函数 (传递给图形类)
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
    
    // 静态辅助函数
    static uint16_t color565(uint8_t r, uint8_t g, uint8_t b) { return Graphics::color565(r, g, b); }
    
    // 友元声明
    friend class Graphics;
};

} // namespace st7789 
#pragma once

#include <cstdint>
#include "st7789_config.hpp"

namespace st7789 {

// 前向声明
class ST7789;

// 图形功能类 - 处理绘图操作
class Graphics {
private:
    ST7789* _lcd; // 引用主LCD类
    
public:
    Graphics(ST7789* lcd);
    virtual ~Graphics();
    
    // 基本绘图函数
    void drawPixel(int16_t x, int16_t y, uint16_t color);
    void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color);
    void drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
    void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
    void drawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color);
    void fillCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color);
    void drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color);
    
    // 文本功能
    void drawChar(int16_t x, int16_t y, char c, uint16_t color, uint16_t bg, uint8_t size);
    void drawString(int16_t x, int16_t y, const char* str, uint16_t color, uint16_t bg, uint8_t size);
    
    // 图像绘制
    void drawImage(int16_t x, int16_t y, int16_t w, int16_t h, const uint16_t* data);
    
    // 辅助函数
    static uint16_t color565(uint8_t r, uint8_t g, uint8_t b);
};

} // namespace st7789 
#include "st7789_gfx.hpp"
#include "st7789.hpp"
#include <cstdlib>
#include <cmath>

// 包含字体数据的前向声明
extern const unsigned char font[];

namespace st7789 {

Graphics::Graphics(ST7789* lcd) : _lcd(lcd) {
}

Graphics::~Graphics() {
}

// 将RGB值转换为16位颜色
uint16_t Graphics::color565(uint8_t r, uint8_t g, uint8_t b) {
    return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}

// 绘制单个像素
void Graphics::drawPixel(int16_t x, int16_t y, uint16_t color) {
    // 访问主LCD类来设置绘图窗口和发送数据
    _lcd->setAddrWindow(x, y, x, y);
    
    // 将单个像素的数据转换为字节并发送
    uint8_t data[2];
    data[0] = color >> 8;
    data[1] = color & 0xFF;
    
    _lcd->hal().writeDataBulk(data, 2);
}

// 绘制直线
void Graphics::drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color) {
    // 使用Bresenham算法绘制直线
    int16_t steep = abs(y1 - y0) > abs(x1 - x0);
    
    if (steep) {
        std::swap(x0, y0);
        std::swap(x1, y1);
    }
    
    if (x0 > x1) {
        std::swap(x0, x1);
        std::swap(y0, y1);
    }
    
    int16_t dx = x1 - x0;
    int16_t dy = abs(y1 - y0);
    int16_t err = dx / 2;
    int16_t ystep = (y0 < y1) ? 1 : -1;
    
    for (; x0 <= x1; x0++) {
        if (steep) {
            drawPixel(y0, x0, color);
        } else {
            drawPixel(x0, y0, color);
        }
        
        err -= dy;
        if (err < 0) {
            y0 += ystep;
            err += dx;
        }
    }
}

// 绘制矩形框
void Graphics::drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
    // 绘制四条边
    drawLine(x, y, x + w - 1, y, color);         // 顶边
    drawLine(x, y + h - 1, x + w - 1, y + h - 1, color); // 底边
    drawLine(x, y, x, y + h - 1, color);         // 左边
    drawLine(x + w - 1, y, x + w - 1, y + h - 1, color); // 右边
}

// 填充矩形
void Graphics::fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
    // 边界检查
    if (w <= 0 || h <= 0) {
        return;
    }
    
    // 设置绘图窗口
    _lcd->setAddrWindow(x, y, x + w - 1, y + h - 1);
    
    // 准备颜色数据
    uint8_t colorHi = color >> 8;
    uint8_t colorLo = color & 0xFF;
    
    // 计算需要填充的像素总数
    uint32_t total = w * h;
    
    // 如果数据量很大，分批次发送
    const uint32_t batch_size = 128; // 每批次发送的像素数
    uint8_t buffer[batch_size * 2];  // 每个像素2字节
    
    // 初始化缓冲区
    for (uint32_t i = 0; i < batch_size; i++) {
        buffer[i * 2] = colorHi;
        buffer[i * 2 + 1] = colorLo;
    }
    
    // 分批次发送数据
    uint32_t remaining = total;
    while (remaining > 0) {
        uint32_t current_batch = (remaining > batch_size) ? batch_size : remaining;
        _lcd->hal().writeDataBulk(buffer, current_batch * 2);
        remaining -= current_batch;
    }
}

// 绘制圆
void Graphics::drawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color) {
    // 使用Bresenham圆算法
    int16_t f = 1 - r;
    int16_t ddF_x = 1;
    int16_t ddF_y = -2 * r;
    int16_t x = 0;
    int16_t y = r;
    
    // 绘制8个点
    drawPixel(x0, y0 + r, color);
    drawPixel(x0, y0 - r, color);
    drawPixel(x0 + r, y0, color);
    drawPixel(x0 - r, y0, color);
    
    while (x < y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        
        x++;
        ddF_x += 2;
        f += ddF_x;
        
        drawPixel(x0 + x, y0 + y, color);
        drawPixel(x0 - x, y0 + y, color);
        drawPixel(x0 + x, y0 - y, color);
        drawPixel(x0 - x, y0 - y, color);
        drawPixel(x0 + y, y0 + x, color);
        drawPixel(x0 - y, y0 + x, color);
        drawPixel(x0 + y, y0 - x, color);
        drawPixel(x0 - y, y0 - x, color);
    }
}

// 填充圆
void Graphics::fillCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color) {
    // 绘制垂直线段来填充圆
    drawLine(x0, y0 - r, x0, y0 + r, color);
    
    int16_t f = 1 - r;
    int16_t ddF_x = 1;
    int16_t ddF_y = -2 * r;
    int16_t x = 0;
    int16_t y = r;
    
    while (x < y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        
        x++;
        ddF_x += 2;
        f += ddF_x;
        
        drawLine(x0 + x, y0 - y, x0 + x, y0 + y, color);
        drawLine(x0 - x, y0 - y, x0 - x, y0 + y, color);
        drawLine(x0 + y, y0 - x, x0 + y, y0 + x, color);
        drawLine(x0 - y, y0 - x, x0 - y, y0 + x, color);
    }
}

// 绘制三角形
void Graphics::drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color) {
    // 绘制三角形的三条边
    drawLine(x0, y0, x1, y1, color);
    drawLine(x1, y1, x2, y2, color);
    drawLine(x2, y2, x0, y0, color);
}

// 绘制字符
void Graphics::drawChar(int16_t x, int16_t y, char c, uint16_t color, uint16_t bg, uint8_t size) {
    if ((x >= _lcd->hal().getConfig().width) ||   // 超出右边界
        (y >= _lcd->hal().getConfig().height) ||  // 超出下边界
        ((x + 6 * size - 1) < 0) || // 超出左边界
        ((y + 8 * size - 1) < 0))   // 超出上边界
        return;
    
    // 确保字符在可打印范围内
    if (c < ' ' || c > '~')
        c = '?';
    
    // 字体数据从font中获取
    for (int8_t i = 0; i < 6; i++) {
        uint8_t line;
        if (i == 5) {
            line = 0x0;
        } else {
            line = font[(c - ' ') * 5 + i];
        }
        
        for (int8_t j = 0; j < 8; j++) {
            if (line & 0x1) {
                if (size == 1) {
                    drawPixel(x + i, y + j, color);
                } else {
                    fillRect(x + i * size, y + j * size, size, size, color);
                }
            } else if (bg != color) {
                if (size == 1) {
                    drawPixel(x + i, y + j, bg);
                } else {
                    fillRect(x + i * size, y + j * size, size, size, bg);
                }
            }
            line >>= 1;
        }
    }
}

// 绘制字符串
void Graphics::drawString(int16_t x, int16_t y, const char* str, uint16_t color, uint16_t bg, uint8_t size) {
    int16_t cursor_x = x;
    int16_t cursor_y = y;
    
    while (*str) {
        // 处理换行
        if (*str == '\n') {
            cursor_x = x;
            cursor_y += 8 * size;
        } 
        // 处理回车
        else if (*str == '\r') {
            cursor_x = x;
        } 
        else {
            drawChar(cursor_x, cursor_y, *str, color, bg, size);
            cursor_x += 6 * size;
            
            // 如果即将超出屏幕右边界，自动换行
            if (cursor_x > (_lcd->hal().getConfig().width - 6 * size)) {
                cursor_x = x;
                cursor_y += 8 * size;
            }
        }
        str++;
    }
}

// 绘制图像
void Graphics::drawImage(int16_t x, int16_t y, int16_t w, int16_t h, const uint16_t* data) {
    // 边界检查
    if (x >= _lcd->hal().getConfig().width || y >= _lcd->hal().getConfig().height)
        return;
    
    // 裁剪坐标
    int16_t x1 = x + w - 1;
    int16_t y1 = y + h - 1;
    
    if (x < 0) {
        x = 0;
    }
    if (y < 0) {
        y = 0;
    }
    if (x1 >= _lcd->hal().getConfig().width) {
        x1 = _lcd->hal().getConfig().width - 1;
    }
    if (y1 >= _lcd->hal().getConfig().height) {
        y1 = _lcd->hal().getConfig().height - 1;
    }
    
    w = x1 - x + 1;
    h = y1 - y + 1;
    
    if (w <= 0 || h <= 0) {
        return;
    }
    
    // 设置绘图窗口
    _lcd->setAddrWindow(x, y, x1, y1);
    
    // 发送图像数据
    _lcd->hal().writeDataBulk((const uint8_t*)data, w * h * 2);
}

void Graphics::clearScreen(uint16_t width, uint16_t height, uint16_t color) {
    const int segment_height = 20;  // 每次清除的高度
    for (int y = 0; y < height; y += segment_height) {
        fillRect(0, y, width, segment_height, color);
    }
}

} // namespace st7789 
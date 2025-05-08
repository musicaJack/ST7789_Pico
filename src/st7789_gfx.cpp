#include "st7789_gfx.hpp"
#include "st7789.hpp"
#include <cstdlib>
#include <cmath>

// Forward declaration of font data
extern const unsigned char font[];

namespace st7789 {

Graphics::Graphics(ST7789* lcd) : _lcd(lcd) {
}

Graphics::~Graphics() {
}

// Convert RGB values to 16-bit color
uint16_t Graphics::color565(uint8_t r, uint8_t g, uint8_t b) {
    return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}

// Draw a single pixel
void Graphics::drawPixel(int16_t x, int16_t y, uint16_t color) {
    // Access main LCD class to set drawing window and send data
    _lcd->setAddrWindow(x, y, x, y);
    
    // Convert single pixel data to bytes and send
    uint8_t data[2];
    data[0] = color >> 8;
    data[1] = color & 0xFF;
    
    _lcd->hal().writeDataBulk(data, 2);
}

// Draw a line
void Graphics::drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color) {
    // Use Bresenham's algorithm to draw line
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

// Draw rectangle outline
void Graphics::drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
    // Draw four edges
    drawLine(x, y, x + w - 1, y, color);         // Top edge
    drawLine(x, y + h - 1, x + w - 1, y + h - 1, color); // Bottom edge
    drawLine(x, y, x, y + h - 1, color);         // Left edge
    drawLine(x + w - 1, y, x + w - 1, y + h - 1, color); // Right edge
}

// Fill rectangle
void Graphics::fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
    // Boundary check
    if (w <= 0 || h <= 0) {
        return;
    }
    
    // Set drawing window
    _lcd->setAddrWindow(x, y, x + w - 1, y + h - 1);
    
    // Prepare color data
    uint8_t colorHi = color >> 8;
    uint8_t colorLo = color & 0xFF;
    
    // Calculate total pixels to fill
    uint32_t total = w * h;
    
    // If data amount is large, send in batches
    const uint32_t batch_size = 128; // Pixels per batch
    uint8_t buffer[batch_size * 2];  // 2 bytes per pixel
    
    // Initialize buffer
    for (uint32_t i = 0; i < batch_size; i++) {
        buffer[i * 2] = colorHi;
        buffer[i * 2 + 1] = colorLo;
    }
    
    // Send data in batches
    uint32_t remaining = total;
    while (remaining > 0) {
        uint32_t current_batch = (remaining > batch_size) ? batch_size : remaining;
        _lcd->hal().writeDataBulk(buffer, current_batch * 2);
        remaining -= current_batch;
    }
}

// Draw circle
void Graphics::drawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color) {
    // Use Bresenham's circle algorithm
    int16_t f = 1 - r;
    int16_t ddF_x = 1;
    int16_t ddF_y = -2 * r;
    int16_t x = 0;
    int16_t y = r;
    
    // Draw 8 points
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

// Fill circle
void Graphics::fillCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color) {
    // Draw vertical lines to fill circle
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

// Draw triangle
void Graphics::drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color) {
    // Draw three edges of triangle
    drawLine(x0, y0, x1, y1, color);
    drawLine(x1, y1, x2, y2, color);
    drawLine(x2, y2, x0, y0, color);
}

// Draw character
void Graphics::drawChar(int16_t x, int16_t y, char c, uint16_t color, uint16_t bg, uint8_t size) {
    if ((x >= _lcd->hal().getConfig().width) ||   // Beyond right boundary
        (y >= _lcd->hal().getConfig().height) ||  // Beyond bottom boundary
        ((x + 6 * size - 1) < 0) || // Beyond left boundary
        ((y + 8 * size - 1) < 0))   // Beyond top boundary
        return;
    
    // Ensure character is in printable range
    if (c < ' ' || c > '~')
        c = '?';
    
    // Font data obtained from font
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

// Draw string
void Graphics::drawString(int16_t x, int16_t y, const char* str, uint16_t color, uint16_t bg, uint8_t size) {
    int16_t cursor_x = x;
    int16_t cursor_y = y;
    
    while (*str) {
        // Process line break
        if (*str == '\n') {
            cursor_x = x;
            cursor_y += 8 * size;
        } 
        // Process carriage return
        else if (*str == '\r') {
            cursor_x = x;
        } 
        else {
            drawChar(cursor_x, cursor_y, *str, color, bg, size);
            cursor_x += 6 * size;
            
            // If about to exceed right boundary, auto line break
            if (cursor_x > (_lcd->hal().getConfig().width - 6 * size)) {
                cursor_x = x;
                cursor_y += 8 * size;
            }
        }
        str++;
    }
}

// Draw image
void Graphics::drawImage(int16_t x, int16_t y, int16_t w, int16_t h, const uint16_t* data) {
    // Boundary check
    if (x >= _lcd->hal().getConfig().width || y >= _lcd->hal().getConfig().height)
        return;
    
    // Clip coordinates
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
    
    // Set drawing window
    _lcd->setAddrWindow(x, y, x1, y1);
    
    // Send image data
    _lcd->hal().writeDataBulk((const uint8_t*)data, w * h * 2);
}

void Graphics::clearScreen(uint16_t width, uint16_t height, uint16_t color) {
    const int segment_height = 20;  // Height per clear
    for (int y = 0; y < height; y += segment_height) {
        fillRect(0, y, width, segment_height, color);
    }
}

} // namespace st7789 
/**
 * @file st7789_gfx.c
 * @brief ST7789 LCD图形库实现
 */

#include "st7789_gfx.h"
#include "st7789.h"
#include <stdlib.h>

/**
 * @brief 绘制水平线
 */
void st7789_draw_hline(uint16_t x, uint16_t y, uint16_t w, uint16_t color) {
    // 设置绘图窗口
    st7789_set_window(x, y, x + w - 1, y);
    
    // 准备颜色数据
    uint8_t hi = color >> 8;
    uint8_t lo = color & 0xFF;
    uint8_t data[2] = {hi, lo};
    
    // 绘制像素
    for (uint16_t i = 0; i < w; i++) {
        st7789_write_data_buffer(data, 2);
    }
}

/**
 * @brief 绘制垂直线
 */
void st7789_draw_vline(uint16_t x, uint16_t y, uint16_t h, uint16_t color) {
    // 设置绘图窗口
    st7789_set_window(x, y, x, y + h - 1);
    
    // 准备颜色数据
    uint8_t hi = color >> 8;
    uint8_t lo = color & 0xFF;
    uint8_t data[2] = {hi, lo};
    
    // 绘制像素
    for (uint16_t i = 0; i < h; i++) {
        st7789_write_data_buffer(data, 2);
    }
}

/**
 * @brief 绘制线段 (Bresenham算法)
 */
void st7789_draw_line(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color) {
    int16_t steep = abs(y1 - y0) > abs(x1 - x0);
    
    if (steep) {
        // 交换x0和y0
        uint16_t temp = x0;
        x0 = y0;
        y0 = temp;
        
        // 交换x1和y1
        temp = x1;
        x1 = y1;
        y1 = temp;
    }
    
    if (x0 > x1) {
        // 交换x0和x1
        uint16_t temp = x0;
        x0 = x1;
        x1 = temp;
        
        // 交换y0和y1
        temp = y0;
        y0 = y1;
        y1 = temp;
    }
    
    int16_t dx = x1 - x0;
    int16_t dy = abs(y1 - y0);
    int16_t err = dx / 2;
    int16_t ystep;
    
    if (y0 < y1) {
        ystep = 1;
    } else {
        ystep = -1;
    }
    
    for (; x0 <= x1; x0++) {
        if (steep) {
            st7789_draw_pixel(y0, x0, color);
        } else {
            st7789_draw_pixel(x0, y0, color);
        }
        
        err -= dy;
        if (err < 0) {
            y0 += ystep;
            err += dx;
        }
    }
}

/**
 * @brief 绘制矩形
 */
void st7789_draw_rect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color) {
    // 绘制水平线
    st7789_draw_hline(x, y, w, color);
    st7789_draw_hline(x, y + h - 1, w, color);
    
    // 绘制垂直线
    st7789_draw_vline(x, y, h, color);
    st7789_draw_vline(x + w - 1, y, h, color);
}

/**
 * @brief 绘制填充矩形
 */
void st7789_fill_rect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color) {
    // 设置绘图窗口
    st7789_set_window(x, y, x + w - 1, y + h - 1);
    
    // 准备颜色数据
    uint8_t hi = color >> 8;
    uint8_t lo = color & 0xFF;
    uint8_t data[2] = {hi, lo};
    
    // 填充像素
    for (uint32_t i = 0; i < (uint32_t)w * h; i++) {
        st7789_write_data_buffer(data, 2);
    }
}

/**
 * @brief 绘制圆形 (Bresenham算法)
 */
void st7789_draw_circle(uint16_t x0, uint16_t y0, uint16_t r, uint16_t color) {
    int16_t f = 1 - r;
    int16_t ddF_x = 1;
    int16_t ddF_y = -2 * r;
    int16_t x = 0;
    int16_t y = r;
    
    st7789_draw_pixel(x0, y0 + r, color);
    st7789_draw_pixel(x0, y0 - r, color);
    st7789_draw_pixel(x0 + r, y0, color);
    st7789_draw_pixel(x0 - r, y0, color);
    
    while (x < y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        
        x++;
        ddF_x += 2;
        f += ddF_x;
        
        st7789_draw_pixel(x0 + x, y0 + y, color);
        st7789_draw_pixel(x0 - x, y0 + y, color);
        st7789_draw_pixel(x0 + x, y0 - y, color);
        st7789_draw_pixel(x0 - x, y0 - y, color);
        st7789_draw_pixel(x0 + y, y0 + x, color);
        st7789_draw_pixel(x0 - y, y0 + x, color);
        st7789_draw_pixel(x0 + y, y0 - x, color);
        st7789_draw_pixel(x0 - y, y0 - x, color);
    }
}

/**
 * @brief 绘制填充圆形
 */
void st7789_fill_circle(uint16_t x0, uint16_t y0, uint16_t r, uint16_t color) {
    // 使用水平线绘制填充圆
    int16_t f = 1 - r;
    int16_t ddF_x = 1;
    int16_t ddF_y = -2 * r;
    int16_t x = 0;
    int16_t y = r;
    
    // 绘制中间水平线
    st7789_draw_hline(x0 - r, y0, 2 * r + 1, color);
    
    while (x < y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        
        x++;
        ddF_x += 2;
        f += ddF_x;
        
        // 绘制上下水平线对
        st7789_draw_hline(x0 - x, y0 + y, 2 * x + 1, color);
        st7789_draw_hline(x0 - x, y0 - y, 2 * x + 1, color);
        
        // 绘制左右水平线对
        st7789_draw_hline(x0 - y, y0 + x, 2 * y + 1, color);
        st7789_draw_hline(x0 - y, y0 - x, 2 * y + 1, color);
    }
} 
/**
 * @file st7789_gfx.h
 * @brief ST7789 LCD图形库头文件
 */

#ifndef _ST7789_GFX_H_
#define _ST7789_GFX_H_

#include <stdint.h>
#include <stdbool.h>
#include "st7789.h"

/**
 * @brief 颜色转换宏(R,G,B转RGB565)
 */
#define ST7789_COLOR_RGB(r, g, b) ((((r) & 0xF8) << 8) | (((g) & 0xFC) << 3) | (((b) & 0xF8) >> 3))

/**
 * @brief 绘制水平线
 * 
 * @param x 起始X坐标
 * @param y Y坐标
 * @param w 宽度
 * @param color 颜色
 */
void st7789_draw_hline(uint16_t x, uint16_t y, uint16_t w, uint16_t color);

/**
 * @brief 绘制垂直线
 * 
 * @param x X坐标
 * @param y 起始Y坐标
 * @param h 高度
 * @param color 颜色
 */
void st7789_draw_vline(uint16_t x, uint16_t y, uint16_t h, uint16_t color);

/**
 * @brief 绘制线段
 * 
 * @param x0 起始X坐标
 * @param y0 起始Y坐标
 * @param x1 结束X坐标
 * @param y1 结束Y坐标
 * @param color 颜色
 */
void st7789_draw_line(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color);

/**
 * @brief 绘制矩形
 * 
 * @param x 左上角X坐标
 * @param y 左上角Y坐标
 * @param w 宽度
 * @param h 高度
 * @param color 颜色
 */
void st7789_draw_rect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);

/**
 * @brief 绘制填充矩形
 * 
 * @param x 左上角X坐标
 * @param y 左上角Y坐标
 * @param w 宽度
 * @param h 高度
 * @param color 颜色
 */
void st7789_fill_rect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);

/**
 * @brief 绘制圆形
 * 
 * @param x0 圆心X坐标
 * @param y0 圆心Y坐标
 * @param r 半径
 * @param color 颜色
 */
void st7789_draw_circle(uint16_t x0, uint16_t y0, uint16_t r, uint16_t color);

/**
 * @brief 绘制填充圆形
 * 
 * @param x0 圆心X坐标
 * @param y0 圆心Y坐标
 * @param r 半径
 * @param color 颜色
 */
void st7789_fill_circle(uint16_t x0, uint16_t y0, uint16_t r, uint16_t color);

/**
 * @brief 绘制单个字符
 * 
 * @param x 左上角X坐标
 * @param y 左上角Y坐标
 * @param c 字符
 * @param color 字符颜色
 * @param bg 背景颜色
 * @param size 字体大小(1=8x8, 2=16x16, 等)
 */
void st7789_draw_char(uint16_t x, uint16_t y, char c, uint16_t color, uint16_t bg, uint8_t size);

/**
 * @brief 绘制字符串
 * 
 * @param x 左上角X坐标
 * @param y 左上角Y坐标
 * @param str 字符串指针
 * @param color 字符颜色
 * @param bg 背景颜色
 * @param size 字体大小
 */
void st7789_draw_string(uint16_t x, uint16_t y, const char *str, uint16_t color, uint16_t bg, uint8_t size);

#endif /* _ST7789_GFX_H_ */ 
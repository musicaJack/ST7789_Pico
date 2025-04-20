/**
 * @file st7789_hal.h
 * @brief ST7789 LCD硬件抽象层头文件
 */

#ifndef _ST7789_HAL_H_
#define _ST7789_HAL_H_

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "st7789.h"

/**
 * @brief 初始化硬件接口
 * 
 * @param config LCD配置参数
 * @return bool 初始化是否成功
 */
bool st7789_hal_init(const st7789_config_t *config);

/**
 * @brief 发送命令到LCD
 * 
 * @param cmd 命令值
 */
void st7789_hal_write_cmd(uint8_t cmd);

/**
 * @brief 发送单字节数据到LCD
 * 
 * @param data 数据值
 */
void st7789_hal_write_data(uint8_t data);

/**
 * @brief 发送多字节数据到LCD
 * 
 * @param data 数据缓冲区指针
 * @param len 数据长度
 */
void st7789_hal_write_data_bulk(const uint8_t *data, size_t len);

/**
 * @brief 控制LCD复位引脚
 * 
 * @param state 复位状态(true为高电平，false为低电平)
 */
void st7789_hal_reset(bool state);

/**
 * @brief 控制LCD背光引脚
 * 
 * @param state 背光状态(true为开，false为关)
 */
void st7789_hal_set_backlight(bool state);

/**
 * @brief 延时函数(毫秒)
 * 
 * @param ms 延时毫秒数
 */
void st7789_hal_delay_ms(uint32_t ms);

/**
 * @brief 延时函数(微秒)
 * 
 * @param us 延时微秒数
 */
void st7789_hal_delay_us(uint32_t us);

#endif /* _ST7789_HAL_H_ */ 
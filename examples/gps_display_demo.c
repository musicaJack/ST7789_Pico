/**
 * @file gps_display.c
 * @brief GPS坐标接收模块 - UI显示及模拟数据
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include "pico/stdlib.h"
#include "st7789.h"
#include "st7789_gfx.h"

// 屏幕参数定义
#define SCREEN_WIDTH  240
#define SCREEN_HEIGHT 320

// 颜色定义 - 除标准颜色外的扩展配色
#define COLOR_BACKGROUND   0x0841  // 深蓝色背景
#define COLOR_TITLE        0xFFFF  // 白色标题
#define COLOR_LABEL        0xAD55  // 浅灰色标签
#define COLOR_VALUE        0x07FF  // 青色数值
#define COLOR_WARNING      0xF800  // 红色警告
#define COLOR_GOOD         0x07E0  // 绿色正常状态
#define COLOR_BORDER       0x6B5A  // 边框颜色
#define COLOR_GRID         0x39C7  // 网格线颜色

// GPS数据结构
typedef struct {
    float latitude;     // 纬度
    float longitude;    // 经度
    float altitude;     // 海拔高度(米)
    float speed;        // 地面速度(km/h)
    float course;       // 航向角(度)
    int satellites;     // 卫星数量
    float hdop;         // 水平精度因子
    bool fix;           // 定位状态
    char timestamp[9];  // 时间戳 "HH:MM:SS"
    char datestamp[11]; // 日期 "YYYY-MM-DD"
} gps_data_t;

// 全局GPS数据
static gps_data_t gps_data = {0};

/**
 * @brief 绘制GPS UI界面的基本框架
 */
static void draw_gps_ui_frame(void) {
    // 清屏为背景色
    st7789_fill_screen(COLOR_BACKGROUND);
    
    // 绘制标题区域
    st7789_fill_rect(0, 0, SCREEN_WIDTH, 30, ST7789_BLUE);
    st7789_draw_string(50, 8, "GPS Position Monitor", COLOR_TITLE, ST7789_BLUE, 1);
    
    // 绘制外边框
    st7789_draw_rect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, COLOR_BORDER);
    
    // 绘制分隔线
    st7789_draw_hline(0, 30, SCREEN_WIDTH, COLOR_BORDER);
    st7789_draw_hline(0, 200, SCREEN_WIDTH, COLOR_BORDER);
    
    // 绘制卫星信号强度区域标题
    st7789_draw_string(10, 210, "Satellite Signal", COLOR_LABEL, COLOR_BACKGROUND, 1);
    
    // 绘制固定标签
    st7789_draw_string(10, 40, "Latitude:", COLOR_LABEL, COLOR_BACKGROUND, 1);
    st7789_draw_string(10, 60, "Longitude:", COLOR_LABEL, COLOR_BACKGROUND, 1);
    st7789_draw_string(10, 80, "Altitude:", COLOR_LABEL, COLOR_BACKGROUND, 1);
    st7789_draw_string(10, 100, "Speed:", COLOR_LABEL, COLOR_BACKGROUND, 1);
    st7789_draw_string(10, 120, "Course:", COLOR_LABEL, COLOR_BACKGROUND, 1);
    st7789_draw_string(10, 140, "Satellites:", COLOR_LABEL, COLOR_BACKGROUND, 1);
    st7789_draw_string(10, 160, "HDOP:", COLOR_LABEL, COLOR_BACKGROUND, 1);
    st7789_draw_string(10, 180, "Status:", COLOR_LABEL, COLOR_BACKGROUND, 1);
    
    // 绘制日期和时间区域
    st7789_draw_rect(120, 180, 110, 15, COLOR_BORDER);
}

/**
 * @brief 绘制卫星信号强度图
 * @param satellites 卫星数量
 */
static void draw_satellite_signal(int satellites) {
    // 清除信号区域
    st7789_fill_rect(10, 225, SCREEN_WIDTH - 20, 40, COLOR_BACKGROUND);
    
    // 绘制信号强度条
    int max_satellites = 12; // 最大显示的卫星数
    int bar_width = (SCREEN_WIDTH - 40) / max_satellites;
    
    for (int i = 0; i < max_satellites; i++) {
        int x = 20 + i * bar_width;
        int max_height = 35;
        int height;
        
        if (i < satellites) {
            // 随机模拟不同信号强度 (1-5)
            int strength = rand() % 5 + 1;
            height = strength * max_height / 5;
            
            // 根据强度选择颜色
            uint16_t color;
            if (strength < 2) color = ST7789_RED;
            else if (strength < 4) color = ST7789_YELLOW;
            else color = ST7789_GREEN;
            
            st7789_fill_rect(x, 265 - height, bar_width - 2, height, color);
        } else {
            // 未使用的卫星位置显示为灰色空条
            st7789_draw_rect(x, 230, bar_width - 2, max_height, COLOR_GRID);
        }
    }
}

/**
 * @brief 更新GPS数据显示
 */
static void update_gps_display(void) {
    char buffer[32]; // 字符缓冲区
    
    // 擦除显示的旧值区域
    st7789_fill_rect(100, 40, 130, 150, COLOR_BACKGROUND);
    
    // 显示纬度
    snprintf(buffer, sizeof(buffer), "%.6f %c", 
            fabs(gps_data.latitude), gps_data.latitude >= 0 ? 'N' : 'S');
    st7789_draw_string(100, 40, buffer, COLOR_VALUE, COLOR_BACKGROUND, 1);
    
    // 显示经度
    snprintf(buffer, sizeof(buffer), "%.6f %c", 
            fabs(gps_data.longitude), gps_data.longitude >= 0 ? 'E' : 'W');
    st7789_draw_string(100, 60, buffer, COLOR_VALUE, COLOR_BACKGROUND, 1);
    
    // 显示海拔
    snprintf(buffer, sizeof(buffer), "%.1f m", gps_data.altitude);
    st7789_draw_string(100, 80, buffer, COLOR_VALUE, COLOR_BACKGROUND, 1);
    
    // 显示速度
    snprintf(buffer, sizeof(buffer), "%.1f km/h", gps_data.speed);
    st7789_draw_string(100, 100, buffer, COLOR_VALUE, COLOR_BACKGROUND, 1);
    
    // 显示航向
    snprintf(buffer, sizeof(buffer), "%.1f\xF8", gps_data.course); // \xF8是度数符号
    st7789_draw_string(100, 120, buffer, COLOR_VALUE, COLOR_BACKGROUND, 1);
    
    // 显示卫星数量
    snprintf(buffer, sizeof(buffer), "%d", gps_data.satellites);
    st7789_draw_string(100, 140, buffer, COLOR_VALUE, COLOR_BACKGROUND, 1);
    
    // 显示HDOP
    snprintf(buffer, sizeof(buffer), "%.1f", gps_data.hdop);
    st7789_draw_string(100, 160, buffer, COLOR_VALUE, COLOR_BACKGROUND, 1);
    
    // 显示定位状态
    if (gps_data.fix) {
        st7789_draw_string(100, 180, "Fixed", COLOR_GOOD, COLOR_BACKGROUND, 1);
    } else {
        st7789_draw_string(100, 180, "No Fix", COLOR_WARNING, COLOR_BACKGROUND, 1);
    }
    
    // 显示日期和时间
    st7789_draw_string(125, 180, gps_data.timestamp, COLOR_VALUE, COLOR_BACKGROUND, 1);
    
    // 更新卫星信号图
    draw_satellite_signal(gps_data.satellites);
}

/**
 * @brief 生成模拟GPS数据
 */
static void generate_mock_gps_data(void) {
    // 为随机数生成器设置种子
    static bool seed_set = false;
    if (!seed_set) {
        srand(time_us_32());
        seed_set = true;
    }
    
    // 模拟深圳市附近的GPS数据
    gps_data.latitude = 22.5431 + ((rand() % 1000) - 500) * 0.0001;
    gps_data.longitude = 114.0579 + ((rand() % 1000) - 500) * 0.0001;
    gps_data.altitude = 20.0 + (rand() % 300) / 10.0;
    gps_data.speed = (rand() % 1000) / 10.0;
    gps_data.course = (rand() % 3600) / 10.0;
    
    // 卫星数和信号状态有10%的几率变化
    if (rand() % 10 == 0) {
        gps_data.satellites = 4 + (rand() % 9); // 4-12颗卫星
    }
    
    // 若卫星数小于4，定位很可能失败
    gps_data.fix = (gps_data.satellites >= 4);
    
    // HDOP值 (0.8-3.0, 越小越好)
    gps_data.hdop = 0.8 + (rand() % 22) / 10.0;
    
    // 更新时间 (以实际系统时间为基础模拟)
    absolute_time_t t = get_absolute_time();
    uint32_t seconds = to_ms_since_boot(t) / 1000;
    uint32_t minutes = seconds / 60;
    uint32_t hours = minutes / 60;
    
    snprintf(gps_data.timestamp, sizeof(gps_data.timestamp), 
            "%02lu:%02lu:%02lu", hours % 24, minutes % 60, seconds % 60);
    
    // 日期格式 (固定值示例)
    strcpy(gps_data.datestamp, "2023-06-15");
}

/**
 * @brief GPS显示主函数 - 可以从主程序调用
 */
void gps_display_demo(void) {
    printf("Starting GPS display demo...\n");
    
    // 初始化(随机)GPS数据
    generate_mock_gps_data();
    
    // 绘制UI框架
    draw_gps_ui_frame();
    
    // 初始更新一次显示
    update_gps_display();
    
    // 模拟数据更新循环
    uint32_t last_update = 0;
    uint32_t last_blink = 0;
    bool indicator_on = false;
    
    while (true) {
        uint32_t current_time = to_ms_since_boot(get_absolute_time());
        
        // 每1秒更新一次GPS数据和显示
        if (current_time - last_update >= 1000) {
            generate_mock_gps_data();
            update_gps_display();
            last_update = current_time;
        }
        
        // 绘制GPS活动指示器闪烁 (右上角)
        if (current_time - last_blink >= 500) {
            indicator_on = !indicator_on;
            
            if (indicator_on) {
                st7789_fill_circle(SCREEN_WIDTH - 15, 15, 5, COLOR_GOOD);
            } else {
                st7789_fill_circle(SCREEN_WIDTH - 15, 15, 5, COLOR_BACKGROUND);
            }
            
            last_blink = current_time;
        }
        
        // 这里可以添加实际的GPS数据读取代码
        // 例如从UART接收NMEA数据并解析
        
        sleep_ms(100); // 降低CPU占用
    }
}

/**
 * @brief 主函数入口
 */
int main() {
    // 初始化标准库
    stdio_init_all();
    
    // 初始化ST7789
    st7789_config_t config = {
        .spi_inst    = spi0,
        .spi_speed_hz = 40 * 1000 * 1000, // 40MHz
        .pin_din     = 19,
        .pin_sck     = 18,
        .pin_cs      = 17,
        .pin_dc      = 20,
        .pin_reset   = 15,
        .pin_bl      = 10,
        .width       = SCREEN_WIDTH,
        .height      = SCREEN_HEIGHT,
        .rotation    = 0
    };
    
    st7789_init(&config);
    st7789_set_rotation(2); // 竖屏模式，旋转90度
    
    printf("GPS Display Demo\n");
    printf("----------------\n");
    
    // 打开显示屏背光
    printf("Turning on backlight...\n");
    st7789_set_backlight(true);
    sleep_ms(500);
    
    // 开始GPS显示演示
    gps_display_demo();
    
    return 0;
}

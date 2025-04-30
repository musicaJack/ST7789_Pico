#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/gpio.h"
#include <stdio.h>

// 引脚定义
#define PIN_DIN   19  // SPI MOSI
#define PIN_SCK   18  // SPI SCK
#define PIN_CS    17  // SPI CS - 需要手动控制
#define PIN_DC    20  // Data/Command
#define PIN_RESET 15  // Reset
#define PIN_BL    10  // Backlight

// 屏幕参数
#define SCREEN_WIDTH 240
#define SCREEN_HEIGHT 320

// SPI实例选择
#define LCD_SPI spi0

// 标志位
static bool is_initialized = false;

// 初始化SPI
static void spi_init_pins(void) {
    printf("Initializing SPI and GPIO...\n");
    
    // 预先复位，确保干净的初始状态
    gpio_init(PIN_RESET);
    gpio_set_dir(PIN_RESET, GPIO_OUT);
    gpio_put(PIN_RESET, 0);
    sleep_ms(100);
    gpio_put(PIN_RESET, 1);
    sleep_ms(100);
    
    // 初始化GPIO
    gpio_init(PIN_DC);
    gpio_init(PIN_BL);
    gpio_init(PIN_CS);
    
    // 设置GPIO方向
    gpio_set_dir(PIN_DC, GPIO_OUT);
    gpio_set_dir(PIN_BL, GPIO_OUT);
    gpio_set_dir(PIN_CS, GPIO_OUT);
    
    // 初始状态 - 确保CS为高电平，表示未选中
    gpio_put(PIN_CS, 1);
    gpio_put(PIN_DC, 1);
    gpio_put(PIN_BL, 0);  // 背光关闭
    
    // 初始化SPI
    printf("Configuring SPI...\n");
    spi_init(LCD_SPI, 40 * 1000 * 1000); // 速度40MHz
    
    // 设置SPI格式 - 必须是Mode 0
    spi_set_format(LCD_SPI, 8, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);
    
    // 设置SPI引脚功能 - 注意CS由软件控制
    gpio_set_function(PIN_SCK, GPIO_FUNC_SPI);
    gpio_set_function(PIN_DIN, GPIO_FUNC_SPI);
    
    printf("SPI and GPIO initialization complete\n");
}

// 发送命令
static void lcd_write_cmd(uint8_t cmd) {
    gpio_put(PIN_CS, 0);  // 选中芯片
    gpio_put(PIN_DC, 0);  // 命令模式
    spi_write_blocking(LCD_SPI, &cmd, 1);
    gpio_put(PIN_CS, 1);  // 取消选中
    sleep_us(10);         // 短暂延时
}

// 发送数据
static void lcd_write_data(uint8_t data) {
    gpio_put(PIN_CS, 0);  // 选中芯片
    gpio_put(PIN_DC, 1);  // 数据模式
    spi_write_blocking(LCD_SPI, &data, 1);
    gpio_put(PIN_CS, 1);  // 取消选中
}

// 发送多个数据
static void lcd_write_data_bulk(const uint8_t *data, size_t len) {
    gpio_put(PIN_CS, 0);  // 选中芯片
    gpio_put(PIN_DC, 1);  // 数据模式
    spi_write_blocking(LCD_SPI, data, len);
    gpio_put(PIN_CS, 1);  // 取消选中
}

// 通用初始化序列 - 适用于多种ST7789芯片
static void lcd_init_sequence(void) {
    printf("Running ST7789 initialization sequence...\n");
    
    // 复位后等待
    sleep_ms(120);
    
    // 基本初始化
    lcd_write_cmd(0x01);    // 软件复位
    sleep_ms(120);          // 复位后必须等待
    
    lcd_write_cmd(0x11);    // 退出睡眠模式
    sleep_ms(120);          // 等待退出睡眠
    
    // Memory Data Access Control
    lcd_write_cmd(0x36);    // MADCTL
    lcd_write_data(0x00);   // 设置为正常方向 (可能需要调整)
    
    // Interface Pixel Format
    lcd_write_cmd(0x3A);
    lcd_write_data(0x55);   // 65K of RGB interface (16bit/pixel)
    
    // 显示反转控制
    lcd_write_cmd(0x21);    // 开启显示反转
    
    // 设置行列地址范围
    lcd_write_cmd(0x2A);    // 列地址设置
    lcd_write_data(0x00);
    lcd_write_data(0x00);
    lcd_write_data((SCREEN_WIDTH >> 8) & 0xFF);
    lcd_write_data(SCREEN_WIDTH & 0xFF);
    
    lcd_write_cmd(0x2B);    // 行地址设置
    lcd_write_data(0x00);
    lcd_write_data(0x00);
    lcd_write_data((SCREEN_HEIGHT >> 8) & 0xFF);
    lcd_write_data(SCREEN_HEIGHT & 0xFF);
    
    // 附加设置 - 根据不同型号可能需要调整
    lcd_write_cmd(0xB2);    // Porch Setting
    lcd_write_data(0x0C);
    lcd_write_data(0x0C);
    lcd_write_data(0x00);
    lcd_write_data(0x33);
    lcd_write_data(0x33);
    
    lcd_write_cmd(0xB7);    // Gate Control
    lcd_write_data(0x35);
    
    lcd_write_cmd(0xBB);    // VCOM Setting
    lcd_write_data(0x20);   // 0.9V
    
    lcd_write_cmd(0xC0);    // LCM Control
    lcd_write_data(0x2C);
    
    lcd_write_cmd(0xC2);    // VDV and VRH Command Enable
    lcd_write_data(0x01);
    
    lcd_write_cmd(0xC3);    // VRH Set
    lcd_write_data(0x12);
    
    lcd_write_cmd(0xC4);    // VDV Set
    lcd_write_data(0x20);
    
    lcd_write_cmd(0xC6);    // Frame Rate Control in Normal Mode
    lcd_write_data(0x0F);   // 60Hz
    
    lcd_write_cmd(0xD0);    // Power Control 1
    lcd_write_data(0xA4);
    lcd_write_data(0xA1);
    
    lcd_write_cmd(0xE0);    // Positive Voltage Gamma Control
    lcd_write_data(0xD0);
    lcd_write_data(0x08);
    lcd_write_data(0x11);
    lcd_write_data(0x08);
    lcd_write_data(0x0C);
    lcd_write_data(0x15);
    lcd_write_data(0x39);
    lcd_write_data(0x33);
    lcd_write_data(0x50);
    lcd_write_data(0x36);
    lcd_write_data(0x13);
    lcd_write_data(0x14);
    lcd_write_data(0x29);
    lcd_write_data(0x2D);
    
    lcd_write_cmd(0xE1);    // Negative Voltage Gamma Control
    lcd_write_data(0xD0);
    lcd_write_data(0x08);
    lcd_write_data(0x10);
    lcd_write_data(0x08);
    lcd_write_data(0x06);
    lcd_write_data(0x06);
    lcd_write_data(0x39);
    lcd_write_data(0x44);
    lcd_write_data(0x51);
    lcd_write_data(0x0B);
    lcd_write_data(0x16);
    lcd_write_data(0x14);
    lcd_write_data(0x2F);
    lcd_write_data(0x31);
    
    sleep_ms(10);
    
    // 打开显示
    lcd_write_cmd(0x29);    // 开启显示
    sleep_ms(10);
    
    printf("ST7789 initialization sequence completed\n");
}

// 初始化LCD
static void lcd_init(void) {
    printf("Starting LCD initialization...\n");
    
    // 硬件复位
    printf("Performing hardware reset...\n");
    gpio_put(PIN_RESET, 0);
    sleep_ms(200);  // 保持低电平200ms
    gpio_put(PIN_RESET, 1);
    sleep_ms(200);  // 复位后等待
    
    // 运行初始化序列
    lcd_init_sequence();
    
    is_initialized = true;
    printf("LCD initialization complete\n");
}

// 测试填充屏幕为单色
static void lcd_fill_color(uint16_t color) {
    uint8_t hi = color >> 8;
    uint8_t lo = color & 0xFF;
    
    // 设置绘图区域
    lcd_write_cmd(0x2A);    // 列地址设置
    lcd_write_data(0x00);
    lcd_write_data(0x00);
    lcd_write_data((SCREEN_WIDTH >> 8) & 0xFF);
    lcd_write_data(SCREEN_WIDTH & 0xFF);
    
    lcd_write_cmd(0x2B);    // 行地址设置
    lcd_write_data(0x00);
    lcd_write_data(0x00);
    lcd_write_data((SCREEN_HEIGHT >> 8) & 0xFF);
    lcd_write_data(SCREEN_HEIGHT & 0xFF);
    
    // 开始写入RAM
    lcd_write_cmd(0x2C);
    
    // 填充全屏
    gpio_put(PIN_CS, 0);  // 选中芯片
    gpio_put(PIN_DC, 1);  // 数据模式
    
    for (int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; i++) {
        spi_write_blocking(LCD_SPI, &hi, 1);
        spi_write_blocking(LCD_SPI, &lo, 1);
    }
    
    gpio_put(PIN_CS, 1);  // 取消选中
}

int main() {
    stdio_init_all();
    sleep_ms(3000);  // 等待串口初始化完成
    printf("\n\n\nStarting LCD test program...\n");
    
    // 初始化SPI和GPIO
    spi_init_pins();
    
    // 初始化LCD
    lcd_init();
    
    // 开启背光
    printf("Turning on backlight...\n");
    gpio_put(PIN_BL, 1);  // 背光打开
    sleep_ms(500);
    
    // 循环绘制不同颜色
    printf("Starting display test...\n");
    
    // 尝试先填充红色屏幕
    printf("Filling screen with RED color\n");
    lcd_fill_color(0xF800);  // 红色
    sleep_ms(2000);
    
    // 填充绿色
    printf("Filling screen with GREEN color\n");
    lcd_fill_color(0x07E0);  // 绿色
    sleep_ms(2000);
    
    // 填充蓝色
    printf("Filling screen with BLUE color\n");
    lcd_fill_color(0x001F);  // 蓝色
    sleep_ms(2000);
    
    // 然后再进入彩色动画
    uint32_t t = 0;
    while (true) {
        // 设置绘图区域
        lcd_write_cmd(0x2A);    // 列地址设置
        lcd_write_data(0x00);
        lcd_write_data(0x00);
        lcd_write_data((SCREEN_WIDTH >> 8) & 0xFF);
        lcd_write_data(SCREEN_WIDTH & 0xFF);
        
        lcd_write_cmd(0x2B);    // 行地址设置
        lcd_write_data(0x00);
        lcd_write_data(0x00);
        lcd_write_data((SCREEN_HEIGHT >> 8) & 0xFF);
        lcd_write_data(SCREEN_HEIGHT & 0xFF);
        
        // 开始写入RAM
        lcd_write_cmd(0x2C);
        
        // 绘制动态彩色图案
        gpio_put(PIN_CS, 0);  // 选中芯片
        gpio_put(PIN_DC, 1);  // 数据模式
        
        for (uint16_t y = 0; y < SCREEN_HEIGHT; y++) {
            for (uint16_t x = 0; x < SCREEN_WIDTH; x++) {
                uint8_t r = (x + t) & 0x1F;
                uint8_t g = (y - t) & 0x3F;
                uint8_t b = (x + y + (t >> 1)) & 0x1F;
                
                uint16_t color = ((r & 0x1F) << 11) | ((g & 0x3F) << 5) | (b & 0x1F);
                uint8_t data[2] = {color >> 8, color & 0xFF};
                spi_write_blocking(LCD_SPI, data, 2);
            }
        }
        
        gpio_put(PIN_CS, 1);  // 取消选中
        
        t++;
        printf("Frame %d completed\n", t);
        sleep_ms(100);  // 添加帧间延时
    }
} 
# 树莓派Pico ST7789 LCD驱动库

[![English](https://img.shields.io/badge/language-English-blue.svg)](README.md)
[![简体中文](https://img.shields.io/badge/语言-简体中文-red.svg)](README_zh.md)

一个为Raspberry Pi Pico设计的模块化、功能丰富的ST7789 LCD显示屏驱动库。该库采用清晰的分层架构，便于理解、修改和扩展。

## 功能特点

- **硬件抽象层 (HAL)**：隔离硬件相关代码，便于移植
- **核心驱动**：处理ST7789特定的初始化和命令
- **图形库**：提供基本形状和文本的绘制功能
- **模块化设计**：组件之间关注点分离明确
- **可配置**：易于为不同的引脚配置和显示尺寸自定义

## 支持的显示屏

- 基于ST7789芯片的显示屏（240x320，也支持其他分辨率）
- 16位色彩 (RGB565)
- SPI接口

## 目录结构

```
ST7789_Pico/
├── include/             # 头文件
│   ├── st7789.h         # 主驱动头文件
│   ├── st7789_hal.h     # 硬件抽象层
│   └── st7789_gfx.h     # 图形库
├── src/                 # 源文件
│   ├── st7789.c         # 核心驱动实现
│   ├── st7789_hal.c     # 硬件实现
│   ├── st7789_gfx.c     # 基本图形实现
│   └── st7789_font.c    # 字体渲染
└── examples/            # 示例程序
    └── lcd_demo.c       # 演示程序
```

## 硬件连接

| ST7789 LCD | Raspberry Pi Pico |
|------------|------------------|
| VCC        | 3.3V             |
| GND        | GND              |
| SCL        | GPIO18 (SPI0 SCK)|
| SDA        | GPIO19 (SPI0 TX) |
| RES        | GPIO15           |
| DC         | GPIO20           |
| CS         | GPIO17           |
| BLK        | GPIO10           |

## 构建项目

### 前提条件

1. 安装Raspberry Pi Pico C/C++ SDK
2. 安装CMake（3.13或更新版本）
3. 安装构建工具（Make或Ninja）

### 构建指南

1. 克隆仓库：
```bash
git clone https://github.com/yourusername/ST7789_Pico.git
cd ST7789_Pico
```

2. 创建构建目录并进入：
```bash
mkdir build
cd build
```

3. 配置并构建项目：
```bash
cmake ..
make
```

这将生成几个文件，包括：
- `lcd_demo.uf2` - 演示程序
- `pico_spi_lcd.uf2` - 原始主程序

## 上传到Raspberry Pi Pico

1. **使用UF2方法**:
   - 按住Pico上的BOOTSEL按钮，同时将其连接到计算机
   - 连接后释放按钮
   - Pico将作为USB大容量存储设备出现
   - 将`.uf2`文件复制到Pico驱动器
   - Pico将自动重启并运行程序

2. **使用SWD（Picoprobe或调试探针）**:
```bash
openocd -f interface/picoprobe.cfg -f target/rp2040.cfg -c "program lcd_demo.elf verify reset exit"
```

## API参考

### 初始化

```c
// 配置显示屏
st7789_config_t config = {
    .spi_inst = spi0,
    .spi_speed_hz = 40 * 1000 * 1000,  // 40MHz
    
    .pin_din = 19,   // MOSI
    .pin_sck = 18,   // SCK
    .pin_cs = 17,    // CS
    .pin_dc = 20,    // Data/Command
    .pin_reset = 15, // Reset
    .pin_bl = 10,    // Backlight
    
    .width = 240,
    .height = 320,
    .rotation = 0,
};

// 初始化显示屏
st7789_init(&config);

// 打开背光
st7789_set_backlight(true);
```

### 绘图函数

```c
// 用颜色填充屏幕
st7789_fill_screen(ST7789_BLACK);

// 绘制图形
st7789_draw_pixel(x, y, color);
st7789_draw_line(x0, y0, x1, y1, color);
st7789_draw_rect(x, y, w, h, color);
st7789_fill_rect(x, y, w, h, color);
st7789_draw_circle(x0, y0, r, color);
st7789_fill_circle(x0, y0, r, color);

// 绘制文本
st7789_draw_char(x, y, 'A', ST7789_WHITE, ST7789_BLACK, 2);
st7789_draw_string(x, y, "Hello World", ST7789_WHITE, ST7789_BLACK, 2);
```

## 颜色定义

库提供了RGB565格式的常用颜色定义：

```c
ST7789_BLACK    0x0000
ST7789_WHITE    0xFFFF
ST7789_RED      0xF800
ST7789_GREEN    0x07E0
ST7789_BLUE     0x001F
ST7789_YELLOW   0xFFE0
ST7789_CYAN     0x07FF
ST7789_MAGENTA  0xF81F
```

## 贡献

欢迎贡献！请随时提交Pull Request。

## 许可证

本项目采用MIT许可证 - 详见LICENSE文件。 
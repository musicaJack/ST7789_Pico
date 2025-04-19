# Raspberry Pi Pico ST7789 LCD驱动

这个项目提供了使用Raspberry Pi Pico (RP2040) 驱动ST7789 TFT-LCD显示屏的示例代码。通过SPI接口，本项目实现了对LCD显示屏的基本驱动和图像绘制功能。

![项目示例图片](https://raw.githubusercontent.com/raspberrypi/pico-examples/master/gpio/images/Pico-GPIO-Pins.jpg)

## 功能特点

- 通过SPI接口驱动ST7789 LCD显示屏
- 支持240x320分辨率显示
- 实现16位色彩模式(RGB565)
- 提供了完整的初始化序列
- 支持渐变色彩图案显示
- 包含易于使用的开发工具和部署脚本

## 硬件要求

- Raspberry Pi Pico 或其他基于RP2040的开发板
- ST7789驱动的TFT-LCD显示屏
- 必要的连接线

## 引脚连接

| Pico引脚 | 功能 | LCD引脚 |
|---------|------|---------|
| GPIO 19 | SPI MOSI | DIN/SDA |
| GPIO 18 | SPI SCK | CLK/SCK |
| GPIO 17 | SPI CS | CS |
| GPIO 20 | Data/Command | DC |
| GPIO 15 | Reset | RST |
| GPIO 10 | Backlight | BL |
| 3.3V | 电源 | VCC |
| GND | 接地 | GND |

## 开发环境设置

### 准备工作

1. 安装[Raspberry Pi Pico SDK](https://github.com/raspberrypi/pico-sdk)
2. 安装[ARM GCC工具链](https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm/downloads)
3. 安装[CMake](https://cmake.org/download/)
4. 安装[Visual Studio Code](https://code.visualstudio.com/)和相关扩展
   - C/C++扩展
   - CMake Tools扩展

### 克隆并构建项目

```bash
# 克隆项目
git clone https://github.com/yourusername/pico-spi-lcd.git
cd pico-spi-lcd

# 创建构建目录
mkdir build
cd build

# 配置并构建项目
cmake .. -G "MinGW Makefiles"
mingw32-make
```

或者使用提供的批处理脚本：

```bash
build_pico.bat
```

## 部署到Pico

1. 按住Pico上的BOOTSEL按钮并连接到电脑
2. 松开按钮，Pico会作为USB存储设备挂载
3. 将生成的.uf2文件复制到Pico驱动器
   ```bash
   deploy_to_pico.bat
   ```

## 使用VS Code进行开发

本项目提供了完整的VS Code配置，包括任务和工具脚本：

1. 打开VS Code，按下`Ctrl+Shift+P`打开命令面板
2. 输入"Tasks"并选择"Tasks: Run Task"
3. 选择以下任务之一：
   - **构建Pico项目** - 编译项目
   - **清理并重新构建** - 清理构建目录并重新编译
   - **构建并部署到Pico** - 编译并部署到Pico
   - **使用批处理脚本构建** - 使用批处理脚本构建
   - **使用批处理脚本部署** - 自动查找Pico设备并部署

## 自定义开发

要修改项目以适应不同的显示屏或更改功能，主要编辑以下文件：

- `pico_spi_lcd.c` - 包含主要的LCD驱动代码和示例应用
- `CMakeLists.txt` - 项目构建配置

## 故障排除

- **屏幕未显示**: 检查硬件连接，特别是VCC/GND和信号线
- **显示内容混乱**: 检查LCD初始化序列，可能需要调整MADCTL寄存器设置
- **编译错误**: 确保SDK路径正确，ARM GCC工具链已正确安装

## 贡献

欢迎提交问题报告和改进建议！请随时提交Pull Request或创建Issue。

## 许可证

本项目采用MIT许可证 - 详见[LICENSE](LICENSE)文件。

## 致谢

- [Raspberry Pi Pico官方文档](https://www.raspberrypi.org/documentation/pico/getting-started/)
- [ST7789显示屏数据手册](https://www.newhavendisplay.com/appnotes/datasheets/LCDs/ST7789V.pdf) 
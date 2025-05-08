# ST7789 LCD Driver for Raspberry Pi Pico

A C++ library for controlling ST7789 LCD displays with Raspberry Pi Pico. This library provides a comprehensive set of features for graphics and text rendering, with support for DMA transfers and screen rotation.

## Features

- Full support for ST7789 LCD displays
- Hardware abstraction layer for easy porting
- Graphics library with basic shapes and text rendering
- DMA support for efficient data transfer
- Screen rotation support (0°, 90°, 180°, 270°)
- Built-in font support
- Configurable pin assignments
- Backlight control

## Hardware Requirements

- Raspberry Pi Pico
- ST7789 LCD display (240x320 resolution)
- SPI interface connections

## Pin Connections

Default pin configuration:
- MOSI: GPIO19
- SCK: GPIO18
- CS: GPIO17
- DC: GPIO20
- RESET: GPIO15
- BL (Backlight): GPIO10

## Installation

1. Clone this repository:
```bash
git clone https://github.com/yourusername/ST7789_Pico.git
cd ST7789_Pico
```

2. Build the project:
```bash
mkdir build
cd build
cmake ..
make
```

## Usage

### Basic Example

```cpp
#include "st7789.hpp"

int main() {
    // Create LCD object
    st7789::ST7789 lcd;
    
    // Configure LCD
    st7789::Config config;
    config.spi_inst = spi0;
    config.pin_din = 19;    // MOSI
    config.pin_sck = 18;    // SCK
    config.pin_cs = 17;     // CS
    config.pin_dc = 20;     // DC
    config.pin_reset = 15;  // RESET
    config.pin_bl = 10;     // Backlight
    config.width = 240;
    config.height = 320;
    
    // Initialize LCD
    if (!lcd.begin(config)) {
        return -1;
    }
    
    // Draw something
    lcd.clearScreen();
    lcd.drawString(10, 10, "Hello World!", st7789::WHITE, st7789::BLACK, 2);
}
```

### Available Demos

1. `lcd_demo.cpp`: Basic demonstration with screen rotation and test pattern
2. `lcd_dma_demo.cpp`: Demonstration of DMA transfer capabilities

## API Reference

### Basic Functions

- `begin()`: Initialize the display
- `clearScreen()`: Clear the screen
- `setRotation()`: Set screen rotation
- `setBacklight()`: Control backlight

### Graphics Functions

- `drawPixel()`: Draw a single pixel
- `drawLine()`: Draw a line
- `drawRect()`: Draw a rectangle
- `fillRect()`: Fill a rectangle
- `drawCircle()`: Draw a circle
- `fillCircle()`: Fill a circle
- `drawTriangle()`: Draw a triangle
- `drawString()`: Draw text

### DMA Functions

- `drawImageDMA()`: Draw image using DMA
- `fillRectDMA()`: Fill rectangle using DMA
- `isDmaEnabled()`: Check if DMA is enabled
- `isDmaBusy()`: Check if DMA transfer is in progress

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request.

## License

This project is licensed under the MIT License - see the LICENSE file for details.

## Acknowledgments

- Raspberry Pi Pico SDK
- ST7789 LCD datasheet
- All contributors to this project 
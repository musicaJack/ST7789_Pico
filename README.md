# ST7789 Pico Display Driver

![License](https://img.shields.io/badge/license-MIT-blue.svg)
![Platform](https://img.shields.io/badge/platform-Raspberry%20Pi%20Pico-brightgreen.svg)
![Version](https://img.shields.io/badge/version-1.0.0-orange.svg)

English | [中文](README.zh.md)

A high-performance display driver library for ST7789 TFT displays on Raspberry Pi Pico. This library provides comprehensive display control features including graphics rendering, text display, and basic display operations.

## Features

- Full ST7789 display driver support
- Hardware abstraction layer for easy porting
- Built-in graphics drawing functions
- Optimized SPI communication
- User-friendly API interface
- DMA support (Beta stage, may be unstable)

## Project Architecture

The project uses a modular design with the following components:

### Core Modules

- `st7789.hpp/cpp`: Core driver implementation, including display initialization and basic control functions
- `st7789_hal.hpp/cpp`: Hardware abstraction layer, handling low-level hardware communication
- `st7789_gfx.hpp/cpp`: Graphics functionality implementation, providing drawing and display features
- `st7789_font.cpp`: Font support
- `st7789_config.hpp`: Configuration file, containing pin definitions and display parameters

### Directory Structure

```
├── src/                    # Source code directory
│   ├── st7789.cpp         # Core driver implementation
│   ├── st7789_hal.cpp     # Hardware abstraction layer
│   ├── st7789_gfx.cpp     # Graphics functionality
│   └── st7789_font.cpp    # Font support
├── include/               # Header files directory
│   ├── st7789.hpp        # Core driver header
│   ├── st7789_hal.hpp    # Hardware abstraction layer header
│   ├── st7789_gfx.hpp    # Graphics functionality header
│   └── st7789_config.hpp # Configuration file
├── demo/                  # Example code
├── build/                 # Build output directory
└── CMakeLists.txt        # CMake build configuration
```

## Quick Start

### Hardware Connection

1. Connect the ST7789 display to Raspberry Pi Pico:
   - SCK: GPIO18
   - MOSI: GPIO19
   - CS: GPIO17
   - DC: GPIO20
   - RST: GPIO15
   - BLK: GPIO10
   - VCC: 3.3V
   - GND: GND

Connection diagram:
```
Raspberry Pi Pico         ST7789 Display
+---------------+         +---------------+
|               |         |               |
|  GPIO18 (SCK) |-------->| SCK          |
|  GPIO19 (MOSI)|-------->| MOSI         |
|  GPIO17 (CS)  |-------->| CS           |
|  GPIO20 (DC)  |-------->| DC           |
|  GPIO15 (RST) |-------->| RST          |
|  GPIO10 (BLK) |-------->| BLK          |
|  3.3V         |-------->| VCC          |
|  GND          |-------->| GND          |
|               |         |               |
+---------------+         +---------------+
```

Notes:
1. Ensure all connections are secure to avoid loose connections
2. Check power connections carefully before powering on
3. If display doesn't work, check:
   - Power voltage is stable at 3.3V
   - All signal connections are correct
   - Backlight control line is properly connected

### Software Setup

1. Clone the repository:
```bash
git clone https://github.com/musicaJack/ST7789_Pico.git
cd ST7789_Pico
```

2. Build the project:
```bash
./build_pico.bat
```

3. Upload to Pico:
```bash
./deploy_to_pico.bat
```

## API Usage Guide

### Basic Configuration

```cpp
#include "st7789.hpp"

// Create display object
st7789::ST7789 display;

// Configure display parameters
st7789::Config config;
config.spi_inst = spi0;           // Use SPI0
config.spi_speed_hz = 40000000;   // 40MHz SPI speed
config.pin_din = 19;              // MOSI
config.pin_sck = 18;              // SCK
config.pin_cs = 17;               // CS
config.pin_dc = 20;               // DC
config.pin_reset = 15;            // RST
config.pin_bl = 10;               // Backlight
config.width = 240;               // Display width
config.height = 320;              // Display height
config.rotation = st7789::ROTATION_0;  // Display rotation

// Initialize display
if (!display.begin(config)) {
    // Handle initialization failure
    return -1;
}
```

### Basic Display Operations

```cpp
// Clear screen
display.clear();

// Set display rotation
display.setRotation(st7789::ROTATION_90);  // Options: ROTATION_0, ROTATION_90, ROTATION_180, ROTATION_270

// Set backlight brightness (0-255)
display.setBrightness(128);
```

### Graphics Drawing

```cpp
// Draw pixel
display.drawPixel(10, 10, st7789::RED);

// Draw line
display.drawLine(0, 0, 100, 100, st7789::GREEN);

// Draw rectangle
display.drawRect(50, 50, 100, 100, st7789::BLUE);

// Fill rectangle
display.fillRect(50, 50, 100, 100, st7789::YELLOW);

// Draw circle
display.drawCircle(120, 160, 50, st7789::CYAN);

// Fill circle
display.fillCircle(120, 160, 50, st7789::MAGENTA);
```

### Text Display

```cpp
// Display text (default white, black background)
display.drawString(10, 10, "Hello World!");

// Display colored text
display.drawString(10, 30, "Colored Text", st7789::RED);

// Display text with background color
display.drawString(10, 50, "Text with Background", st7789::WHITE, st7789::BLUE);
```

### DMA Features (Beta)

```cpp
// Enable DMA (enabled by default)
config.dma.enabled = true;
config.dma.buffer_size = 4096;  // 4KB DMA buffer

// Fill rectangle using DMA
display.fillRectDMA(0, 0, 240, 320, st7789::BLACK);

// Check DMA status
if (display.isDmaEnabled()) {
    // DMA is enabled
}

if (display.isDmaBusy()) {
    // DMA transfer in progress
}
```

Notes:
1. DMA features are currently in Beta stage and may be unstable
2. Recommended for non-critical applications only
3. If display issues occur, try disabling DMA:
   ```cpp
   config.dma.enabled = false;
   ```

## Color Definitions

The library predefines the following colors (RGB565 format):
- `st7789::BLACK` (0x0000)
- `st7789::WHITE` (0xFFFF)
- `st7789::RED` (0xF800)
- `st7789::GREEN` (0x07E0)
- `st7789::BLUE` (0x001F)
- `st7789::YELLOW` (0xFFE0)
- `st7789::CYAN` (0x07FF)
- `st7789::MAGENTA` (0xF81F)

Custom colors can also be used:
```cpp
uint16_t customColor = 0x1234;  // Custom RGB565 color
```

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request. Before submitting code, please ensure:

1. Code follows project coding standards
2. Necessary comments and documentation are added
3. All tests pass

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.

## Contact

For questions or suggestions, please contact:

- Submit an Issue
- Email: your.email@example.com

## Acknowledgments

Thanks to all contributors to this project. 
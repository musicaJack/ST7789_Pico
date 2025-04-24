# ST7789 LCD Driver for Raspberry Pi Pico

[![English](https://img.shields.io/badge/language-English-blue.svg)](README.md)
[![简体中文](https://img.shields.io/badge/语言-简体中文-red.svg)](README_zh.md)

A modular, feature-rich library for controlling ST7789 LCD displays with Raspberry Pi Pico. The library is designed with a clean layered architecture for easy understanding, modification, and extension.

## Features

- **Hardware Abstraction Layer (HAL)**: Isolates hardware-specific code for easy porting
- **Core Driver**: Handles ST7789-specific initialization and commands
- **Graphics Library**: Provides drawing functions for basic shapes and text
- **Modular Design**: Clear separation of concerns between components
- **Configurable**: Easy to customize for different pin configurations and display sizes

## Supported Displays

- ST7789 based displays (240x320, other resolutions also supported)
- 16-bit color (RGB565)
- SPI interface

## Directory Structure

```
ST7789_Pico/
├── include/             # Header files
│   ├── st7789.h         # Main driver header
│   ├── st7789_hal.h     # Hardware abstraction layer
│   └── st7789_gfx.h     # Graphics library
├── src/                 # Source files
│   ├── st7789.c         # Core driver implementation
│   ├── st7789_hal.c     # Hardware implementation
│   ├── st7789_gfx.c     # Basic graphics implementation
│   └── st7789_font.c    # Font rendering
└── examples/            # Example programs
    └── lcd_demo.c       # Demo program
```

## Hardware Connection

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

> **Note**: This project uses a custom-designed development board as the base. The connections listed above are specifically optimized for this custom hardware platform to ensure optimal performance and integration.

## Building the Project

### Prerequisites

1. Install the Raspberry Pi Pico C/C++ SDK
2. Install CMake (3.13 or newer)
3. Install Build Tools (Make or Ninja)

### Build Instructions

1. Clone the repository:
```bash
git clone https://github.com/yourusername/ST7789_Pico.git
cd ST7789_Pico
```

2. Create a build directory and navigate to it:
```bash
mkdir build
cd build
```

3. Configure and build the project:
```bash
cmake ..
make
```

This will generate several files including:
- `lcd_demo.uf2` - The demo program 
- `pico_spi_lcd.uf2` - The original main program

## Uploading to Raspberry Pi Pico

1. **Using UF2 Method**:
   - Hold the BOOTSEL button on the Pico while connecting it to your computer
   - Release the button once connected
   - Pico will appear as a USB mass storage device
   - Copy the `.uf2` file to the Pico drive
   - The Pico will automatically reboot and run the program

2. **Using SWD (Picoprobe or Debug Probe)**:
```bash
openocd -f interface/picoprobe.cfg -f target/rp2040.cfg -c "program lcd_demo.elf verify reset exit"
```

## API Reference

### Initialization

```c
// Configure the display
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

// Initialize the display
st7789_init(&config);

// Turn on backlight
st7789_set_backlight(true);
```

### Drawing Functions

```c
// Fill screen with color
st7789_fill_screen(ST7789_BLACK);

// Draw shapes
st7789_draw_pixel(x, y, color);
st7789_draw_line(x0, y0, x1, y1, color);
st7789_draw_rect(x, y, w, h, color);
st7789_fill_rect(x, y, w, h, color);
st7789_draw_circle(x0, y0, r, color);
st7789_fill_circle(x0, y0, r, color);

// Draw text
st7789_draw_char(x, y, 'A', ST7789_WHITE, ST7789_BLACK, 2);
st7789_draw_string(x, y, "Hello World", ST7789_WHITE, ST7789_BLACK, 2);
```

## Color Definitions

The library provides common color definitions in RGB565 format:

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

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request.

## License

This project is licensed under the MIT License - see the LICENSE file for details. 

## Contact

For technical support or questions:
- Email: yinyue@beingdigital.cn
- Technical Discussion: Please use the project Wiki or forums
- For other inquiries: Feel free to reach out using the contact information above 
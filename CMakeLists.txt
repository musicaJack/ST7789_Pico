# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

# Specify Pico SDK path (if needed)
# set(PICO_SDK_PATH "your Pico SDK path")

# Set Pico board type
set(PICO_BOARD "pico" CACHE STRING "Board type")

# Set C/C++ standards
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Completely disable picotool and other tools download to avoid network issues
set(PICO_SKIP_TOOLS_PACKAGE_SEARCH 1)
set(SKIP_TOOL_RISCV_OPENOCD 1)
set(SKIP_TOOL_PICOTOOL 1)
set(PICO_SDK_FETCH_FROM_GIT 0)
set(PICOTOOL_FETCH_FROM_GIT 0)
set(PICO_SKIP_TOOL_CHECKS 1)
set(PICO_NO_HARDWARE 0)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.1.1)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.1.1)
set(picoVscode ${PICO_SDK_PATH}/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================

# Include Pico SDK import script
include(pico_sdk_import.cmake)

# Initialize project (must be before pico_sdk_init)
project(st7789_pico C CXX ASM)

# Initialize Pico SDK
pico_sdk_init()

# Add ST7789 library
add_library(st7789_lib
    src/st7789.cpp
    src/st7789_hal.cpp
    src/st7789_gfx.cpp
    src/st7789_font.cpp
)

# Set ST7789 library include directories
target_include_directories(st7789_lib PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/include
)

# Set ST7789 library link libraries
target_link_libraries(st7789_lib
    pico_stdlib
    hardware_spi
    hardware_gpio
    hardware_dma
)

# Add LCD demo program
add_executable(lcd_demo
    examples/lcd_demo.cpp
)

# LCD demo program include directories
target_include_directories(lcd_demo PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/include
)

# Link libraries required by LCD demo program
target_link_libraries(lcd_demo
    st7789_lib
    pico_stdlib
    hardware_spi
    hardware_gpio
)

# Configure standard input/output for LCD demo program
pico_enable_stdio_usb(lcd_demo 1)
pico_enable_stdio_uart(lcd_demo 0)

# Create additional output format files for LCD demo program
pico_add_extra_outputs(lcd_demo)

# Add DMA demo program
add_executable(lcd_dma_demo
    examples/lcd_dma_demo.cpp
)

# DMA demo program include directories
target_include_directories(lcd_dma_demo PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/include
)

# Link libraries required by DMA demo program
target_link_libraries(lcd_dma_demo
    st7789_lib
    pico_stdlib
    hardware_spi
    hardware_gpio
    hardware_dma
)

# Configure standard input/output for DMA demo program
pico_enable_stdio_usb(lcd_dma_demo 1)
pico_enable_stdio_uart(lcd_dma_demo 0)

# Create additional output format files for DMA demo program
pico_add_extra_outputs(lcd_dma_demo)
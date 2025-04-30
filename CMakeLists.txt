# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

# 指定Pico SDK路径 (如果需要)
# set(PICO_SDK_PATH "你的Pico SDK路径")

# 设置Pico板型 - 使用字符串形式
set(PICO_BOARD "pico" CACHE STRING "Board type")

# 设置C/C++标准
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# 完全禁用picotool和其他工具下载，避免网络问题
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

# 包含Pico SDK导入脚本
include(pico_sdk_import.cmake)

# 初始化项目（必须在pico_sdk_init前）
project(st7789_pico C CXX ASM)

# 初始化Pico SDK
pico_sdk_init()

# 添加ST7789库
add_library(st7789_lib
    src/st7789.c
    src/st7789_hal.c
    src/st7789_gfx.c
    src/st7789_font.c
)

# 设置ST7789库包含路径
target_include_directories(st7789_lib PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/include
)

# 设置ST7789库链接库
target_link_libraries(st7789_lib
    pico_stdlib
    hardware_spi
    hardware_gpio
)

# 添加示例程序
add_executable(lcd_demo
    examples/lcd_demo.c
)

# 示例程序包含目录
target_include_directories(lcd_demo PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/include
)

# 链接示例程序需要的库
target_link_libraries(lcd_demo
    st7789_lib
    pico_stdlib
    hardware_spi
    hardware_gpio
)

# 配置示例程序的标准输入/输出
pico_enable_stdio_usb(lcd_demo 1)
pico_enable_stdio_uart(lcd_demo 0)

# 创建示例程序的额外输出格式文件
pico_add_extra_outputs(lcd_demo)

# 添加GPS显示程序
add_executable(gps_display_demo
    examples/gps_display_demo.c
)

# GPS显示程序包含目录
target_include_directories(gps_display_demo PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/include
)

# 链接GPS显示程序需要的库
target_link_libraries(gps_display_demo
    st7789_lib
    pico_stdlib
    hardware_spi
    hardware_gpio
)

# 配置GPS显示程序的标准输入/输出
pico_enable_stdio_usb(gps_display_demo 1)
pico_enable_stdio_uart(gps_display_demo 0)

# 创建GPS显示程序的额外输出格式文件
pico_add_extra_outputs(gps_display_demo)
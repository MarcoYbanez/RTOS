cmake_minimum_required(VERSION 3.13)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_C_COMPILER "/opt/homebrew/bin/arm-none-eabi-gcc")
set(CMAKE_CXX_COMPILER "/opt/homebrew/bin/arm-none-eabi-g++")

# Pull in SDK (must be before project)
include(pico_sdk_import.cmake)

# Pull in FreeRTOS
include(FreeRTOS_Kernel_import.cmake)

project(example C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

file(GLOB Challenges "Challenges/*.c")
# Main Blinky
add_executable(digikey main.c ${Challenges})

target_include_directories(digikey PRIVATE ${CMAKE_CURRENT_LIST_DIR})

target_link_libraries(digikey pico_stdlib hardware_i2c FreeRTOS-Kernel
                      FreeRTOS-Kernel-Heap4)
pico_enable_stdio_usb(digikey 1)
pico_enable_stdio_uart(digikey 0)
pico_add_extra_outputs(digikey)

cmake_minimum_required(VERSION 3.13)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_C_COMPILER "/opt/homebrew/bin/arm-none-eabi-gcc")
set(CMAKE_CXX_COMPILER "/opt/homebrew/bin/arm-none-eabi-g++")

# Pull in SDK (must be before project)
include(pico_sdk_import.cmake)

# Pull in FreeRTOS
include(FreeRTOS_Kernel_import.cmake)

if(NOT FREERTOS_DEMO_PATH)
  if(DEFINED ENV{FREERTOS_DEMO_PATH})
    set(FREERTOS_DEMO_PATH $ENV{FREERTOS_DEMO_PATH})
    message(
      "Using FREERTOS_DEMO_PATH from environment ('${FREERTOS_DEMO_PATH}')")
  elseif(EXISTS ${FREERTOS_KERNEL_PATH}/../Demo)
    set(FREERTOS_DEMO_PATH ${FREERTOS_KERNEL_PATH}/../Demo)
  else()
    message(
      FATAL_ERROR
        "FreeRTOS demo path was not specified. Please set FREERTOS_DEMO_PATH.")
  endif()
endif()

project(example C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

file(GLOB i2c_devices "${CMAKE_CURRENT_LIST_DIR}/I2C/*.c")
file(GLOB spi_devices "${CMAKE_CURRENT_LIST_DIR}/SPI/*.c")

# Main Blinky
add_executable(main main.c ${i2c_devices} ${spi_devices})

target_include_directories(
  main PRIVATE ${CMAKE_CURRENT_LIST_DIR} ${CMAKE_CURRENT_LIST_DIR}/SPI
               ${CMAKE_CURRENT_LIST_DIR}/I2C)

target_link_libraries(main pico_stdlib hardware_i2c FreeRTOS-Kernel
                      FreeRTOS-Kernel-Heap4 hardware_spi)

pico_enable_stdio_usb(main 1)
pico_enable_stdio_uart(main 0)
pico_add_extra_outputs(main)

set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/exe)

# Add post-build command to generate UF2 (replace with your actual command)
add_custom_command(
  TARGET main
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E echo "Generating UF2 file..."
  COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:main>
          ${CMAKE_BINARY_DIR}/main.uf2)

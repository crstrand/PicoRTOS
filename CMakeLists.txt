# Set minimum required version of CMake
cmake_minimum_required(VERSION 3.12)

# Include build functions from Pico SDK
include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

# Pull in FreeRTOS
include(FreeRTOS_Kernel_import.cmake)

# Set name of project (as PROJECT_NAME) and C/C   standards
project(picoRTOS C CXX ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(PICO_DEOPTIMIZED_DEBUG 1)

# Creates a pico-sdk subdirectory in our project for the libraries
pico_sdk_init()

# Tell CMake where to find the executable source file
add_executable(${PROJECT_NAME} 
    src/main.c
    src/usb_descriptors.c
    src/filter.c
    src/get_serial.c
    src/freertos_hook.c
    )

# no_flash is for debugging with picoprobe
#  it will load the app into RAM and bypass writing to flash (less wear on flash chip)
#pico_set_binary_type(${PROJECT_NAME} no_flash)

# copy_to_ram will store app in flash, but copy it to RAM on boot to run it from RAM
#pico_set_binary_type(${PROJECT_NAME} copy_to_ram)

target_compile_definitions(${PROJECT_NAME} PRIVATE
        PICO_STDIO_STACK_BUFFER_SIZE=64 # use a small printf on stack buffer
)

#target_include_directories(${PROJECT_NAME} PRIVATE src)
target_include_directories(${PROJECT_NAME} PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/src
        )


# Link to pico_stdlib (gpio, time, etc. functions)
target_link_libraries(${PROJECT_NAME} 
    pico_stdlib 
    tinyusb_board
    pico_unique_id
    tinyusb_device
    FreeRTOS-Kernel
    FreeRTOS-Kernel-Heap4
)

# Create map/bin/hex/uf2 files
pico_add_extra_outputs(${PROJECT_NAME})

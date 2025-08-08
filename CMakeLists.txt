# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.2.0)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.2.0)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico_w CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(ProjetoIntegrado_PCEIoT_Board C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1

add_executable(ProjetoIntegrado_PCEIoT_Board 
    src/main.c
    src/ms5637_02ba03/ms5637.c
    src/sht4xl/SHT4xl-PCEIoT-Board.c 
    src/ssd1306/ssd1306.c
    src/io_sx1509b/io_expander.c  
    )

pico_set_program_name(ProjetoIntegrado_PCEIoT_Board "ProjetoIntegrado_PCEIoT_Board")
pico_set_program_version(ProjetoIntegrado_PCEIoT_Board "0.1")

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(ProjetoIntegrado_PCEIoT_Board 0)
pico_enable_stdio_usb(ProjetoIntegrado_PCEIoT_Board 1)

# Add the standard library to the build
target_link_libraries(ProjetoIntegrado_PCEIoT_Board
        pico_stdlib)

# Add the standard include files to the build
target_include_directories(ProjetoIntegrado_PCEIoT_Board PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
        ${CMAKE_CURRENT_LIST_DIR}/src/ms5637_02ba03
        ${CMAKE_CURRENT_LIST_DIR}/src/sht4xl
        ${CMAKE_CURRENT_LIST_DIR}/src/ssd1306
        ${CMAKE_CURRENT_LIST_DIR}/src/io_sx1509b
)

# Add any user requested libraries
target_link_libraries(ProjetoIntegrado_PCEIoT_Board 
        hardware_spi
        hardware_i2c
        
        )

pico_add_extra_outputs(ProjetoIntegrado_PCEIoT_Board)


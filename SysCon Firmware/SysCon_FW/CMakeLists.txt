# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# == DO NEVER EDIT THE NEXT LINES for Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.0.0)
set(toolchainVersion 13_2_Rel1)
set(picotoolVersion 2.0.0)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================


set(PICO_BOARD pico CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(main C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

#directory for generated files required for building
file(MAKE_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/generated)
#this is the directory where the input bitstream is located
set(BITSTREAM_IN_DIR ${CMAKE_CURRENT_LIST_DIR}/ps2_video_processor/outflow)
#this is the directory where the python script and output file are located
set(BITSTREAM_OUT_DIR ${CMAKE_CURRENT_LIST_DIR}/generated)

#generate the sdram.h file from the bitstream .bit
add_custom_command(
COMMENT "generating bitstream header"    
OUTPUT "video_processor_bitstream" ${BITSTREAM_OUT_DIR}/video_processor_bitstream.h
WORKING_DIRECTORY ${BITSTREAM_OUT_DIR}
USES_TERMINAL
PRE_BUILD
DEPENDS 
    ${BITSTREAM_OUT_DIR}/bit2source.py
    ${BITSTREAM_IN_DIR}/ps2_video_processor.bit
COMMAND 
    python bit2source.py 
    ${BITSTREAM_IN_DIR}/ps2_video_processor.bit 
    -O ${BITSTREAM_OUT_DIR}/video_processor_bitstream.h
)
#I have no idea why this is needed, but the header doesn't generate without it....
add_custom_target(gen_files
    DEPENDS ${BITSTREAM_OUT_DIR}/video_processor_bitstream.h
)

include_directories(inc generated)
file(GLOB MY_SOURCES "src/*.c" "inc/*.h")
# Add executable. Default name is the project name, version 0.1
add_executable(main ${MY_SOURCES})
pico_set_program_name(main "SysCon")
pico_set_program_version(main "0.1")

# generate the header file into the source tree as it is included in the RP2040 datasheet
pico_generate_pio_header(main ${CMAKE_CURRENT_LIST_DIR}/src/gamepad_spi.pio OUTPUT_DIR ${CMAKE_CURRENT_LIST_DIR}/generated)

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(main 1)
pico_enable_stdio_usb(main 0)

# Add the standard library to the build
target_link_libraries(main
        pico_stdlib)

# Add the standard include files to the build
target_include_directories(main PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}
  ${CMAKE_CURRENT_LIST_DIR}/.. # for our common lwipopts or any other standard includes, if required
)

# Add any user requested libraries
target_link_libraries(main pico_stdlib hardware_i2c)
target_link_libraries(main pico_stdlib hardware_spi)
target_link_libraries(main pico_stdlib pico_printf)
target_link_libraries(main pico_stdlib pico_multicore)
target_link_libraries(main pico_stdlib hardware_pio)
target_link_libraries(main pico_stdlib hardware_pwm)
target_link_libraries(main pico_stdlib hardware_adc)
target_link_libraries(main pico_stdlib hardware_flash)

pico_add_extra_outputs(main)


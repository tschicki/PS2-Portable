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

project(gamepad C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1
include_directories(inc generated)
file(GLOB MY_SOURCES "src/*.c" "inc/*.h")
add_executable(gamepad ${MY_SOURCES})

#add_executable(gamepad gamepad.c )

pico_set_program_name(gamepad "gamepad")
pico_set_program_version(gamepad "0.1")

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(gamepad 0)
pico_enable_stdio_usb(gamepad 0)

# Add the standard library to the build
target_link_libraries(gamepad
        pico_stdlib)

# Add the standard include files to the build
target_include_directories(gamepad PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}
  ${CMAKE_CURRENT_LIST_DIR}/.. # for our common lwipopts or any other standard includes, if required
)

# Add any user requested libraries
target_link_libraries(gamepad pico_stdlib)
target_link_libraries(gamepad pico_stdlib hardware_i2c)
target_link_libraries(gamepad pico_stdlib hardware_pwm)
target_link_libraries(gamepad pico_stdlib hardware_adc)
target_link_libraries(gamepad pico_stdlib pico_i2c_slave)

pico_add_extra_outputs(gamepad)




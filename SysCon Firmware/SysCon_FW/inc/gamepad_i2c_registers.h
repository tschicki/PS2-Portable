#ifndef GAMEPAD_I2C_Registers_h
#define GAMEPAD_I2C_Registers_h

#include <stdint.h>
#include <stdio.h>
#include <string.h>

/*Register Map*/
#define REG00_BUTTONS_L     UINT8_C(0x00)
#define REG01_BUTTONS_H     UINT8_C(0x01)
#define REG02_ANALOG_X_L    UINT8_C(0x02)
#define REG03_ANALOG_X_H    UINT8_C(0x03)
#define REG04_ANALOG_Y_L    UINT8_C(0x04)
#define REG05_ANALOG_Y_H    UINT8_C(0x05)
#define REG06_MOTOR         UINT8_C(0x06)
#define REG07_LED_R         UINT8_C(0x07)
#define REG08_LED_G         UINT8_C(0x08)
#define REG09_LED_B         UINT8_C(0x09)

#endif
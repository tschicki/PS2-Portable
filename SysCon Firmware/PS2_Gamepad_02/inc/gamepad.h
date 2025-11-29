
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef gamepad_h
#define gamepad_h

/* Includes ------------------------------------------------------------------*/
#include "MCU_interface.h"

/*I2C register map of the gamepad*/
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

#define ALARM_RUMBLE_BOOST            2
#define ALARM_IRQ_RUMBLE_BOOST        TIMER_IRQ_2
#define RUMBLE_BOOST_INTERVAL_US      6000

void i2c_slave_handler(i2c_inst_t *i2c, i2c_slave_event_t event);
void io_handler(void);
void led_handler(void);
void motor_handler(void);

void start_rumble_boost_timer(uint32_t delay_us);
void rumble_boost_alarm_irq(void);

uint16_t map_to_pwm(uint8_t input, uint8_t in_min, uint8_t in_max, uint16_t out_min, uint16_t out_max);

#endif


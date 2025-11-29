#ifndef POWER_LED_H
#define POWER_LED_H

#include "MCU_interface.h"
#include "gamepad_i2c.h"

extern struct extio *extio_dev;

#define PATTERN_RESERVED   0
#define PATTERN_SOC        1
#define PATTERN_CONFIG     2
#define PATTERN_SUCCESS    3
#define PATTERN_ERROR      4

#define LED_RED            0
#define LED_GREEN          1
#define LED_BLUE           2

struct paramsLED
 {
   uint8_t color1[3];
   uint8_t color2[3];
   uint8_t color1_duration;
   uint8_t color2_duration;
   uint8_t cycles;
   uint8_t enable;
 };

 struct powerLED
 {
   struct paramsLED led_parameters[16];
   uint16_t pattern_start_tick;
   uint8_t current_pattern;
   uint8_t cycle_counter;
   
 };



void handle_led_patterns(struct powerLED *powerLED, struct extio *extio_dev, uint16_t systick);
void led_patterns_init(struct powerLED *powerLED);
void set_power_led(float soc, struct powerLED *powerLED);

#endif
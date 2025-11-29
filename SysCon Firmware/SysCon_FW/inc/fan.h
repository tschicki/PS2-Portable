#ifndef FAN_H
#define FAN_H

#include "MCU_interface.h"
#include "helper_functions.h"

/*fan control*/
#define TEMPERATURE_PIN     28
#define TEMPERATURE_PIN_ADC 2

#define FAN_PWM_PIN             22
#define PWM_SLICE_NUM_28        3
#define PWM_WRAP_COUNT          5000
#define PWM_FAN_DEFAULT_COUNTS  5000

#define CONVERSION_FACTOR (float)(3.3 / (1 << 12))


struct fan_dev
{
    uint8_t fan_high_temp;
    uint8_t fan_low_temp;
    float fan_k;
    float fan_d;
    float ee_temperature;
};

void fan_endi(uint8_t enable);
void fan_init(void);
float calculate_ee_temp(uint16_t voltage);
void run_fan_control(struct fan_dev *fan_dev);
void fan_calculate_constants(struct fan_dev *fan_dev);
void update_fan_curve(uint8_t *temp, uint8_t direction);

#endif
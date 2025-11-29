#include "fan.h"

void fan_init(void)
{
  adc_init();
  /*initialize the io for reading the temperature*/
  adc_gpio_init(TEMPERATURE_PIN);

  /*initialize the io for the fan PWM output, set the default level and enable the output*/
  gpio_set_function(FAN_PWM_PIN, GPIO_FUNC_PWM);
  gpio_set_outover (FAN_PWM_PIN, GPIO_OVERRIDE_INVERT);
  pwm_set_wrap(PWM_SLICE_NUM_28, PWM_WRAP_COUNT);
  
}

void fan_endi(uint8_t enable)
{
  /*enable or disable the PWM slice and set the default level for the fan to be off*/
  pwm_set_chan_level(PWM_SLICE_NUM_28, PWM_CHAN_A, PWM_FAN_DEFAULT_COUNTS);
  pwm_set_enabled(PWM_SLICE_NUM_28, enable);
}

void run_fan_control(struct fan_dev *fan_dev)
{

  volatile uint16_t voltage = 0;
  uint16_t low_count = 0;
  float temperature = 0.0;

  // Select ADC input 2 (GPIO28)
  adc_select_input(TEMPERATURE_PIN_ADC);
  voltage = adc_read();
  temperature = calculate_ee_temp(voltage);
  low_count = (uint16_t)(fan_dev->fan_k * temperature + fan_dev->fan_d);

  if(low_count > PWM_WRAP_COUNT) low_count = PWM_WRAP_COUNT;

  /*high_count is the amount of counts the fan PWM should be HIGH, already taking into account the mosfet*/
  //high_count = PWM_WRAP_COUNT - low_count;
  
  // Set channel A output high for one cycle before dropping
  pwm_set_chan_level(PWM_SLICE_NUM_28, PWM_CHAN_A, low_count);
  fan_dev->ee_temperature = temperature;

  return;
}

float calculate_ee_temp(uint16_t voltage)
{
  /*
  input = analog voltage
  temperature : VO = (−3.88×10−6×T2) + (−1.15×10−2×T) + 1.8639
  pwm wrap: 125000000/25kHz = 5000
  pwm output is inverted -> calculate everything for off time

  take voltage and convert to temperature
  apply linear fan curve -> kx + d
    -low time at 40°C -> 5000
    -low time at 25°C -> 500

  k = dy/dx = (5000-500)/(40-25) = 300
  5000 = 300*40 + d 
  d = -7000
  y = 300x - 7000
  */

  float raw_converted = 0.0;
  uint16_t counts = 0;

  raw_converted = (float)voltage * CONVERSION_FACTOR; 

  /*best fit function for -40 to 85°C, see datasheet*/
  return (1858.3 - (raw_converted*1000))/11.67;
}

void fan_calculate_constants(struct fan_dev *fan_dev)
{
  fan_dev->fan_k = (float)((PWM_WRAP_COUNT-(PWM_WRAP_COUNT/10))/(fan_dev->fan_high_temp - fan_dev->fan_low_temp));
  fan_dev->fan_d = (float)(PWM_WRAP_COUNT - (fan_dev->fan_k * fan_dev->fan_high_temp));
  return;
}

void update_fan_curve(uint8_t *temp, uint8_t direction)
{
    uint32_t temperature = (uint32_t)*temp;
    /*handle incrementing/decrementing based on the direction*/
    inc_dec_setting(&temperature, direction, 1, 99, 0);
    *temp = (uint8_t)temperature;
    return;
}
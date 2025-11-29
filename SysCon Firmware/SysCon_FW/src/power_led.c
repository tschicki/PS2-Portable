#include "power_led.h"

  /*
  parameters for led:
  red, green, blue
  blink on time
  blink off time
  fading speed
  duration
  priority = identifier
  */

 /*red, green, blue*/
/*these are the RGB values, the controller will map them to 0-100% duty cycle*/
static uint8_t led_lookup[11][3] = {
{247, 2, 2},
{247, 109, 2},
{247, 186, 2},
{207, 247, 2},
{129, 247, 2},
{23, 247, 2},
{2, 247, 117},
{2, 247, 235},
{2, 174, 247},
{2, 19, 247},
{2, 19, 247},
}; 

 
 void handle_led_patterns(struct powerLED *powerLED, struct extio *extio_dev, uint16_t systick)
 {
    uint8_t new_pattern = 0;
    uint16_t elapsed_ticks = 0;

    for(uint8_t i = 0; i < 16; i++)
    {
        /*select the highest enabled pattern to continue*/
        if(powerLED->led_parameters[i].enable == 1) new_pattern = i;
    }

    if(new_pattern != powerLED->current_pattern)
    {
        /*a new pattern was issued, this if executes once*/
        powerLED->current_pattern = new_pattern;
        powerLED->pattern_start_tick = systick;
        powerLED->cycle_counter = 0;
    }

    /*handle overflow in tick counter ad set the elapsed ticks accordingly*/
    if((systick < powerLED->pattern_start_tick))
        elapsed_ticks = (65536 - powerLED->pattern_start_tick) + systick;
    else
        elapsed_ticks = systick - powerLED->pattern_start_tick;

    /*set the LED off and on time based on the set pattern*/
    if(elapsed_ticks < powerLED->led_parameters[powerLED->current_pattern].color1_duration)
        extio_set_LED(extio_dev, powerLED->led_parameters[powerLED->current_pattern].color1[LED_RED], powerLED->led_parameters[powerLED->current_pattern].color1[LED_GREEN], powerLED->led_parameters[powerLED->current_pattern].color1[LED_BLUE]);
    else if(elapsed_ticks <= powerLED->led_parameters[powerLED->current_pattern].color2_duration)
        extio_set_LED(extio_dev, powerLED->led_parameters[powerLED->current_pattern].color2[LED_RED], powerLED->led_parameters[powerLED->current_pattern].color2[LED_GREEN], powerLED->led_parameters[powerLED->current_pattern].color2[LED_BLUE]);
    else
    {
        powerLED->current_pattern = PATTERN_RESERVED;
        powerLED->cycle_counter++;
    }
        
    if(powerLED->led_parameters[powerLED->current_pattern].cycles != 0)
    {
        /*pattern is finite, disable it after the specified number of cycles*/
        if(powerLED->cycle_counter > powerLED->led_parameters[powerLED->current_pattern].cycles)
            powerLED->led_parameters[powerLED->current_pattern].enable = 0;
    }

 }


 void led_patterns_init(struct powerLED *powerLED)
 {
        for(uint8_t i = 0; i < 16; i++)
    {
        /*disable all patterns, just to be sure*/
        powerLED->led_parameters[i].enable = 0;
    }

    powerLED->current_pattern = PATTERN_RESERVED;

    powerLED->led_parameters[PATTERN_SOC].enable = 0;
    powerLED->led_parameters[PATTERN_SOC].color1[LED_RED] = 0;
    powerLED->led_parameters[PATTERN_SOC].color1[LED_GREEN] = 0;
    powerLED->led_parameters[PATTERN_SOC].color1[LED_BLUE] = 0;
    powerLED->led_parameters[PATTERN_SOC].color1_duration = 0;
    powerLED->led_parameters[PATTERN_SOC].color2[LED_RED] = 0;
    powerLED->led_parameters[PATTERN_SOC].color2[LED_GREEN] = 0;
    powerLED->led_parameters[PATTERN_SOC].color2[LED_BLUE] = 0;
    powerLED->led_parameters[PATTERN_SOC].color2_duration = 1;
    powerLED->led_parameters[PATTERN_SOC].cycles = 0;

    powerLED->led_parameters[PATTERN_CONFIG].enable = 0;
    powerLED->led_parameters[PATTERN_CONFIG].color1[LED_RED] = 0;
    powerLED->led_parameters[PATTERN_CONFIG].color1[LED_GREEN] = 0;
    powerLED->led_parameters[PATTERN_CONFIG].color1[LED_BLUE] = 0;
    powerLED->led_parameters[PATTERN_CONFIG].color1_duration = 5;
    powerLED->led_parameters[PATTERN_CONFIG].color2[LED_RED] = 255;
    powerLED->led_parameters[PATTERN_CONFIG].color2[LED_GREEN] = 0;
    powerLED->led_parameters[PATTERN_CONFIG].color2[LED_BLUE] = 255;
    powerLED->led_parameters[PATTERN_CONFIG].color2_duration = 10;
    powerLED->led_parameters[PATTERN_CONFIG].cycles = 0;

    powerLED->led_parameters[PATTERN_ERROR].enable = 0;
    powerLED->led_parameters[PATTERN_ERROR].color1[LED_RED] = 0;
    powerLED->led_parameters[PATTERN_ERROR].color1[LED_GREEN] = 0;   
    powerLED->led_parameters[PATTERN_ERROR].color1[LED_BLUE] = 255; 
    powerLED->led_parameters[PATTERN_ERROR].color1_duration = 5;
    powerLED->led_parameters[PATTERN_ERROR].color2[LED_RED] = 255;
    powerLED->led_parameters[PATTERN_ERROR].color2[LED_GREEN] = 0;
    powerLED->led_parameters[PATTERN_ERROR].color2[LED_BLUE] = 255;
    powerLED->led_parameters[PATTERN_ERROR].color2_duration = 10;
    powerLED->led_parameters[PATTERN_ERROR].cycles = 0;

    powerLED->led_parameters[PATTERN_SUCCESS].enable = 0;
    powerLED->led_parameters[PATTERN_SUCCESS].color1[LED_RED] = 0;
    powerLED->led_parameters[PATTERN_SUCCESS].color1[LED_GREEN] = 255;    
    powerLED->led_parameters[PATTERN_SUCCESS].color1[LED_BLUE] = 0;      
    powerLED->led_parameters[PATTERN_SUCCESS].color1_duration = 5;
    powerLED->led_parameters[PATTERN_SUCCESS].color2[LED_RED] = 0;
    powerLED->led_parameters[PATTERN_SUCCESS].color2[LED_GREEN] = 0;
    powerLED->led_parameters[PATTERN_SUCCESS].color2[LED_BLUE] = 0;
    powerLED->led_parameters[PATTERN_SUCCESS].color2_duration = 10;
    powerLED->led_parameters[PATTERN_SUCCESS].cycles = 0;    

 }

 void set_power_led(float soc, struct powerLED *powerLED)
{
  uint8_t soc_divide = 0;

      /*limit the input, just in case*/
    if (soc > 100.0) soc = 100.0;
    if (soc < 0.0) soc = 0.0;

    /*divide the soc down to the LUT range (10)*/
    soc_divide = (uint8_t)(soc / 10);

    if(soc_divide == 0)
    {
        powerLED->led_parameters[PATTERN_SOC].color1_duration = 5;
        powerLED->led_parameters[PATTERN_SOC].color2_duration = 10;
    }
    else
    {
        powerLED->led_parameters[PATTERN_SOC].color1_duration = 0;
        powerLED->led_parameters[PATTERN_SOC].color2_duration = 1;
    }

    //led pinout is wrong! red = green; green = blue; blue = red
    powerLED->led_parameters[PATTERN_SOC].color2[LED_RED] = led_lookup[soc_divide][LED_RED];
    powerLED->led_parameters[PATTERN_SOC].color2[LED_GREEN] = led_lookup[soc_divide][LED_GREEN];
    powerLED->led_parameters[PATTERN_SOC].color2[LED_BLUE] = led_lookup[soc_divide][LED_BLUE];
}
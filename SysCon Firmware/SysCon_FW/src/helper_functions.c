#include "helper_functions.h"


void inc_dec_setting(uint32_t *setting, uint8_t direction, uint8_t amount, uint32_t upper_limit, uint32_t lower_limit)
{
    uint32_t setting_temp = *setting;

    if(direction == ACTION_RIGHT)
        setting_temp = setting_temp + amount; 
    else
    {
        if(setting_temp >= amount) setting_temp = setting_temp - amount;
        else setting_temp = 0;
    }
        
    if(setting_temp > upper_limit) setting_temp = upper_limit;
    if(setting_temp < lower_limit) setting_temp = lower_limit;

    *setting = setting_temp;

    return;

}
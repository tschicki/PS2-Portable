#ifndef user_functions_h
#define user_functions_h

#include <stdint.h>


/*actions for performing various actions, like moving the image*/
#define ACTION_UP                        0
#define ACTION_DOWN                      1
#define ACTION_LEFT                      2
#define ACTION_RIGHT                     3
#define ACTION_SEL                       4
#define ACTION_BACK                      5


void inc_dec_setting(uint32_t *setting, uint8_t direction, uint8_t amount, uint32_t upper_limit, uint32_t lower_limit);


#endif
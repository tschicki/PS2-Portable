#ifndef MCU_interface_h
#define MCU_interface_h

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "pico/stdlib.h"
#include "pico/stdio/driver.h"
#include "hardware/i2c.h"
#include <pico/i2c_slave.h>
#include "hardware/pwm.h"
#include "hardware/adc.h"

/*Gamepad side definition*/
#define SIDE_LEFT           1
#define SIDE_RIGHT          0
/*I2C definitions*/
#define SLAVE_I2C             i2c1
#define SLAVE_I2C_BAUD_RATE   400000
#define SLAVE_I2C_SDA_PIN     18
#define SLAVE_I2C_SCL_PIN     19
#define SLAVE_ADDRESS_RIGHT   0x18
#define SLAVE_ADDRESS_LEFT    0x19
/*pin states to set*/
#define SYSPIN_HIGH         1  
#define SYSPIN_LOW          0
#define SYSPIN_HIZ          2
/*pin configuration defines*/
#define SYSPIN_NOPULL       0
#define SYSPIN_PULLUP       1
#define SYSPIN_PULLDOWN     3
#define SYSPIN_INPUT        4
#define SYSPIN_OUTPUT       5
#define SYSPIN_OUT_OD       6
/*common pins for both sides*/
#define SIDE_SEL          20
#define STICK_X           26
#define STICK_Y           27
#define STICK_VOLTAGE     28
#define X_ADC             0
#define Y_ADC             1
#define STICK_VOLTAGE_ADC 2
/*LED pins for right gamepad*/
#define PWM_R             0   
#define PWM_G             2
#define PWM_B             1
#define WRAP_MULTIPLIER   20
#define PWM_LED_WRAP      (255*WRAP_MULTIPLIER)
/*rumble motor*/
#define MOT_LEFT          22
#define MOT_RIGHT         21
#define PWM_MOT_WRAP      (255*WRAP_MULTIPLIER)
#define PWM_MOT_MIN       3090

/*right gamepad button definitions*/
typedef enum buttons_right_e
{
    R3 = 0,
    START,   
    CIRCLE,  
    TRIANGLE,
    CROSS,   
    SQUARE,  
    R2,      
    R1,      
    SELECT,  

    RIGHT_PINCOUNT,
}buttons_right_t;

/*left gamepad button definitions*/
typedef enum buttons_left_e
{
    L2 = 0,   
    L1,   
    DOWN, 
    RIGHT,
    LEFT, 
    UP,   
    L3,   

    LEFT_PINCOUNT,
}buttons_left_t;


struct io_location
{
    uint8_t io_pin;
    uint8_t reset_state;
    uint8_t pin_mode;
    uint8_t pull_mode;
    uint8_t pin_speed;
    uint8_t drive_strength;
    uint8_t shift;
};

struct Gamepad
{
    struct io_location button_right[RIGHT_PINCOUNT]; 
    struct io_location button_left[LEFT_PINCOUNT];
    /*output for LED enable; only on right gamepad*/
    struct io_location LED_enable;
    uint8_t side_sel;
    /*PWM slices*/
    uint slice_r;
    uint slice_g;
    uint slice_b;
    uint slice_mot;
    /*PWM channels*/
    uint chan_r;
    uint chan_g;
    uint chan_b;
    uint chan_mot;
};

uint8_t determine_side(uint8_t side_sel_pin);
void GPIO_init(struct Gamepad *io);
void i2c_interface_init(i2c_inst_t *hi2c, uint8_t SDA_Pin, uint8_t SCL_Pin, uint32_t baudrate);
void sys_set_pin(struct io_location io, uint8_t pin_state);
uint8_t sys_get_pin(struct io_location io);

static void init_buttons_struct(struct Gamepad *io);
static void LED_init(struct Gamepad *io, uint8_t IO_R, uint8_t IO_G, uint8_t IO_B);
static void Motor_init(struct Gamepad *io, uint8_t IO);
static void PWM_init(uint *slice, uint *channel, uint8_t IO, uint16_t wrap);
static void init_button(struct io_location *io);


#endif

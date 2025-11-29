#ifndef gamepad_user_h
#define gamepad_user_h

#include "MCU_interface.h"
#include "flash.h"
#include "gamepad_i2c.h"
#include "helper_functions.h"

/*virtual digital buttons*/
#define BUTTON_SELECT               0
#define BUTTON_L3                   1
#define BUTTON_R3                   2
#define BUTTON_START                3
#define BUTTON_UP                   4
#define BUTTON_RIGHT                5
#define BUTTON_DOWN                 6
#define BUTTON_LEFT                 7
#define BUTTON_L2                   8
#define BUTTON_R2                   9
#define BUTTON_L1                   10
#define BUTTON_R1                   11
#define BUTTON_TRIANGLE             12
#define BUTTON_CIRCLE               13
#define BUTTON_CROSS                14
#define BUTTON_SQUARE               15
/*virtual button pressures for analog mode and virtual ds2_dev analog sticks*/
/*pressures starting from 4, because the analog sticks come before the pressure values in the analog buffer*/
#define RIGHT_STICK                 0
#define LEFT_STICK                  2
#define STICK_Y                     1
#define STICK_X                     0

#define PRESSURE_RIGHT              4
#define PRESSURE_LEFT               5
#define PRESSURE_UP                 6
#define PRESSURE_DOWN               7
#define PRESSURE_TRIANGLE           8
#define PRESSURE_CIRCLE             9
#define PRESSURE_CROSS              10
#define PRESSURE_SQUARE             11
#define PRESSURE_L1                 12
#define PRESSURE_R1                 13
#define PRESSURE_L2                 14
#define PRESSURE_R2                 15
/*physical remote IO number*/
#define GP_BTN_0                    0
#define GP_BTN_1                    1
#define GP_BTN_2                    2
#define GP_BTN_3                    3
#define GP_BTN_4                    4
#define GP_BTN_5                    5
#define GP_BTN_6                    6
#define GP_BTN_7                    7
#define GP_BTN_8                    8
#define GP_BTN_9                    9
#define GP_BTN_10                   10
#define GP_BTN_11                   11
#define GP_BTN_12                   12
#define GP_BTN_13                   13
#define GP_BTN_14                   14
#define GP_BTN_15                   15

#define NUMBER_OF_BUTTONS           16
#define NUMBER_OF_DIGITAL_IO        32

#define RED_LED_LEFT                16
#define GREEN_LED_LEFT              17
#define BLUE_LED_LEFT               18
#define RED_LED_RIGHT               19
#define GREEN_LED_RIGHT             20
#define BLUE_LED_RIGHT              21

#define RES1_LEFT                   22
#define RES2_LEFT                   23
#define RES3_LEFT                   24
#define RES4_LEFT                   25
#define RES5_LEFT                   26
#define RES1_RIGHT                  27
#define RES2_RIGHT                  28
#define RES3_RIGHT                  29
#define RES4_RIGHT                  30
#define RES5_RIGHT                  31
/*to define physical sides of analog sticks and buttons*/
#define GP_RIGHT                    0
#define GP_LEFT                     1

#define THRESHOLD_INVERT_H          200
#define THRESHOLD_INVERT_L          80

#define PI_4            0.785398
#define MAX_POINTER     2048.0

#define STICK_CALIBRATION_MASK            (0x01 << BUTTON_CROSS)
#define VOLUME_UP_MASK                    (0x01 << BUTTON_UP)
#define VOLUME_DOWN_MASK                  (0x01 << BUTTON_DOWN)


#define DS2_SET_OUTPUT_BIT(data, physical_position, virtual_position) \
    (((data & (0x00000001 << virtual_position)) >> virtual_position) << physical_position)

#define DS2_GET_INPUT_BIT(data_out, data_in, virtual_position) \
    ((data_out & ~(0x00000001 << virtual_position)) | (data_in << virtual_position))

struct axis{
    uint16_t deadzone;
    uint16_t center;
    uint16_t min;
    uint16_t max;
    uint8_t invert_direction;
    uint8_t virtual_axis;
    uint16_t raw_analog_stick_data;
    uint16_t deadzone_stick_data;
    uint16_t scaled_stick_data;
    uint8_t processed_stick_data;
    
};

struct joystick{
    struct axis axes[2];
    uint8_t virtual_stick_side;
    uint8_t pointer_value;
};

struct gamepad_user{
    /*left and right I2C IO expanders for buttons*/
    struct extio extio_dev[2];
    /*configurations for both digital and analog pins*/
    struct joystick analog_stick[2];

    /*polling data of buttons and analog sticks*/
    volatile uint32_t current_digital_states;
    volatile uint32_t previous_digital_states;
    volatile uint8_t current_analog_states[16];

    uint8_t ps1_analog_en;
    uint32_t rumble_intensity;
    uint8_t rumble_en;

};


/*internal functions*/
void gamepad_interface_init(struct gamepad_user *gp_dev);
void gamepad_user_init_remote_io_structs(struct gamepad_user *gp_dev, uint32_t *flash_setting);
void gamepad_user_set_rumble(struct gamepad_user *gp_dev, uint8_t strength_right, uint8_t strength_left);

void gamepad_user_initialize_analog_sticks(struct gamepad_user *gp_dev);
void gamepad_user_read_buttons(struct gamepad_user *gp_dev);
static void gamepad_user_read_analog_stick(struct gamepad_user *gp_dev, uint8_t ds2_dev_side);
static void gamepad_user_process_stick_data(struct axis *axis_x, struct axis *axis_y);
static uint8_t gamepad_user_map_axis(uint16_t axis, uint16_t in_min, uint16_t in_max, uint8_t out_min, uint8_t out_max, uint8_t invert);
static void gamepad_user_fill_pressure_buffer(uint16_t button_states, volatile uint8_t *analog_states);
void center_analog_sticks(struct gamepad_user *gp_dev);
void calibrate_analog_sticks(struct gamepad_user *gp_dev);
void store_stick_calibration(struct gamepad_user *gp_dev, uint32_t *flash_setting);
void invert_stick(struct gamepad_user *gp_dev);
void invert_single_axis(struct axis *axis);

void inc_dec_deadzone(struct gamepad_user *gp_dev, uint8_t inc_dec);
void analog_emulation_toggle(struct gamepad_user *gp_dev);
void run_analog_emulation(struct gamepad_user *gp_dev);

#endif
#include "gamepad_user.h"


void gamepad_interface_init(struct gamepad_user *gp_dev)
{

    /*initialize the I2C for reading the buttons and analog sticks*/
    i2c_interface_init(GAMEPAD_I2C, GAMEPAD_I2C_SDA_PIN, GAMEPAD_I2C_SCL_PIN, GAMEPAD_I2C_BAUD_RATE);

    gp_dev->extio_dev[GP_LEFT].device_address = 0x19;
    gp_dev->extio_dev[GP_RIGHT].device_address = 0x18;

    gp_dev->extio_dev[GP_LEFT].interface = GAMEPAD_I2C;
    gp_dev->extio_dev[GP_RIGHT].interface = GAMEPAD_I2C;

}

void gamepad_user_set_rumble(struct gamepad_user *gp_dev, uint8_t strength_right, uint8_t strength_left)
{

    uint8_t right, left = 0;

    if(gp_dev->rumble_en != 0){
        switch(gp_dev->rumble_intensity){
        case 1:
            right = strength_right / 10;
            left = strength_left / 10;
            break;
        case 2: 
            right = strength_right / 5;
            left = strength_left / 5;
            break;
        case 3:
            right = strength_right;
            left = strength_left;
            break;
        default:
            right = 0;
            left = 0;
        }
    }
    else{
        right = 0;
        left = 0;
    }

    /*lastly we send the new value to the gamepads*/
    extio_set_rumble(&gp_dev->extio_dev[GP_RIGHT], (uint8_t)right);
    extio_set_rumble(&gp_dev->extio_dev[GP_LEFT], (uint8_t)left);
}

void gamepad_user_read_buttons(struct gamepad_user *gp_dev)
{
    uint16_t button_states_left = 0;
    uint16_t button_states_right = 0;
    uint16_t analog_x_left, analog_y_left = 0;
    uint16_t analog_x_right, analog_y_right = 0;

    extio_poll_inputs(&gp_dev->extio_dev[GP_LEFT], &button_states_left, &analog_x_left, &analog_y_left);
    extio_poll_inputs(&gp_dev->extio_dev[GP_RIGHT], &button_states_right, &analog_x_right, &analog_y_right);

    /*analog sticks*/
    gp_dev->analog_stick[GP_LEFT].axes[0].raw_analog_stick_data = analog_x_left;
    gp_dev->analog_stick[GP_LEFT].axes[1].raw_analog_stick_data = analog_y_left;
    gp_dev->analog_stick[GP_RIGHT].axes[0].raw_analog_stick_data = analog_x_right;
    gp_dev->analog_stick[GP_RIGHT].axes[1].raw_analog_stick_data = analog_y_right;

    /*writing both axes of the stick into the polling data buffer for the ds2_dev*/
    /*the position inside the buffer is determined by the virtual side and axis numbers; for remapping*/
    
    gamepad_user_process_stick_data(&gp_dev->analog_stick[GP_LEFT].axes[STICK_X], &gp_dev->analog_stick[GP_LEFT].axes[STICK_Y]);
    gp_dev->current_analog_states[gp_dev->analog_stick[GP_LEFT].virtual_stick_side + gp_dev->analog_stick[GP_LEFT].axes[0].virtual_axis] = gp_dev->analog_stick[GP_LEFT].axes[STICK_X].processed_stick_data;
    gp_dev->current_analog_states[gp_dev->analog_stick[GP_LEFT].virtual_stick_side + gp_dev->analog_stick[GP_LEFT].axes[1].virtual_axis] = gp_dev->analog_stick[GP_LEFT].axes[STICK_Y].processed_stick_data;

    gamepad_user_process_stick_data(&gp_dev->analog_stick[GP_RIGHT].axes[STICK_X], &gp_dev->analog_stick[GP_RIGHT].axes[STICK_Y]);
    gp_dev->current_analog_states[gp_dev->analog_stick[GP_RIGHT].virtual_stick_side + gp_dev->analog_stick[GP_RIGHT].axes[0].virtual_axis] = gp_dev->analog_stick[GP_RIGHT].axes[STICK_X].processed_stick_data;
    gp_dev->current_analog_states[gp_dev->analog_stick[GP_RIGHT].virtual_stick_side + gp_dev->analog_stick[GP_RIGHT].axes[1].virtual_axis] = gp_dev->analog_stick[GP_RIGHT].axes[STICK_Y].processed_stick_data;
    
    /*buttons*/
    gp_dev->previous_digital_states = gp_dev->current_digital_states;
    gp_dev->current_digital_states = button_states_left | button_states_right;
    /*analog stick emulation mostly for PS1 games; usefol for games that only support DPAD*/
    if(gp_dev->ps1_analog_en == 1) run_analog_emulation(gp_dev);
    
    gamepad_user_fill_pressure_buffer(gp_dev->current_digital_states, gp_dev->current_analog_states);

}

static void gamepad_user_process_stick_data(struct axis *axis_x, struct axis *axis_y)
{

/*map the inputs to the range -2048, 2048*/
    int16_t x_data_int = axis_x->raw_analog_stick_data - 2048;
    int16_t y_data_int = axis_y->raw_analog_stick_data - 2048;

    /*applying the deadzone setting*/
    if (abs(x_data_int) < axis_x->deadzone)
        x_data_int = 0;
    
    if (abs(y_data_int) < axis_y->deadzone)
    {
        // multiple of 90 degs so don't scale
        y_data_int = 0;
        axis_x->scaled_stick_data = x_data_int + 2048;
        axis_y->scaled_stick_data = y_data_int + 2048;
    }
    else
    {

        float x_data_float = (float)x_data_int;
        float y_data_float = (float)y_data_int;

        /*interpolate the multiplication factor between sqrt(2) (at 45°) and 1 (at 90°)*/
        // t = 2 when 90°, 1 when 45°
        float t = fabs(atan(x_data_float / y_data_float) * (PI_4)); 
        // s = 1 when 90°, sqrt(2) when 45°
        float s = sqrt(3.0-t);

        /*calculating the vector length*/
        float vector = sqrt((x_data_float*x_data_float) + (y_data_float*y_data_float));

        /*calculating the roundness factor r*/
        float r = powf(vector/MAX_POINTER, 4); //was 3

        /*applying both the scaling and roundness factor by weighted average and adding 2048 to be in uint16_t again*/
        axis_x->scaled_stick_data = (uint16_t)(s * r * x_data_float + (1-r)*x_data_float + 2048);
        axis_y->scaled_stick_data = (uint16_t)(s * r * y_data_float + (1-r)*y_data_float + 2048);

    }

    /*scaling the input from 12 bit to 8 bit*/
    /*applying the deadzone setting and inversion*/
    axis_x->processed_stick_data = gamepad_user_map_axis(axis_x->scaled_stick_data, axis_x->min, axis_x->max, 0, 255, axis_x->invert_direction);
    axis_y->processed_stick_data = gamepad_user_map_axis(axis_y->scaled_stick_data, axis_y->min, axis_y->max, 0, 255, axis_y->invert_direction);

    return;
}

static uint8_t gamepad_user_map_axis(uint16_t axis, uint16_t in_min, uint16_t in_max, uint8_t out_min, uint8_t out_max, uint8_t invert)
{
    uint8_t mapped_axis = 0;

    if (axis > in_max)
        axis = in_max;
    if (axis < in_min)
        axis = in_min;
    /*converted old mapping to float for improved precision*/
    float delta_out = (float)out_max - (float)out_min;
    float delta_in = (float)in_max - (float)in_min;
    float factor = delta_out / delta_in;
    float result_f = (((float)(axis - in_min)) * factor) + (float)out_min;
    mapped_axis = (uint8_t)result_f;
    
    //mapped_axis = (uint8_t)((axis - in_min) * (out_max - out_min) / (in_max - in_min) + out_min);

    /*handling axis inversion*/
    if (invert == TRUE)
        mapped_axis = 255 - mapped_axis;

    return mapped_axis;
}

static void gamepad_user_fill_pressure_buffer(uint16_t button_states, volatile uint8_t *analog_states)
{
    uint16_t button_state = 0;
    uint8_t analog_state = 0;

    for (uint8_t i=0; i<16; i++)
    {
        button_state = (button_states >> i) & 0x0001;
        analog_state = button_state ? 0x00 : 0xFF;

        switch (i)
        {
            case BUTTON_UP:
                analog_states[PRESSURE_UP] = analog_state;
                break;
            case BUTTON_RIGHT:
                analog_states[PRESSURE_RIGHT] = analog_state;
                break;
            case BUTTON_DOWN:
                analog_states[PRESSURE_DOWN] = analog_state;
                break;
            case BUTTON_LEFT:
                analog_states[PRESSURE_LEFT] = analog_state;
                break;
            case BUTTON_L2:
                analog_states[PRESSURE_L2] = analog_state;
                break;
            case BUTTON_R2:
                analog_states[PRESSURE_R2] = analog_state;
                break;
            case BUTTON_L1:
                analog_states[PRESSURE_L1] = analog_state;
                break;
            case BUTTON_R1:
                analog_states[PRESSURE_R1] = analog_state;
                break;
            case BUTTON_TRIANGLE:
                analog_states[PRESSURE_TRIANGLE] = analog_state;
                break;
            case BUTTON_CIRCLE:
                analog_states[PRESSURE_CIRCLE] = analog_state;
                break;
            case BUTTON_CROSS:
                analog_states[PRESSURE_CROSS] = analog_state;
                break;
            case BUTTON_SQUARE:
                analog_states[PRESSURE_SQUARE] = analog_state;
                break;
            default:
                break;
        }
    }
}

void gamepad_user_init_remote_io_structs(struct gamepad_user *gp_dev, uint32_t *flash_setting)
{

    gp_dev->analog_stick[GP_LEFT].virtual_stick_side = LEFT_STICK;
    gp_dev->analog_stick[GP_LEFT].axes[0].deadzone = flash_setting[NO_AX0_DEADZONE];
    gp_dev->analog_stick[GP_LEFT].axes[0].center = flash_setting[NO_AX0_CENTER];
    gp_dev->analog_stick[GP_LEFT].axes[0].min = flash_setting[NO_AX0_MIN];
    gp_dev->analog_stick[GP_LEFT].axes[0].max = flash_setting[NO_AX0_MAX];
    gp_dev->analog_stick[GP_LEFT].axes[0].invert_direction = flash_setting[NO_AX0_INVERT];
    gp_dev->analog_stick[GP_LEFT].axes[0].virtual_axis = flash_setting[NO_AX0_VIRTUAL_AXIS];
    gp_dev->analog_stick[GP_LEFT].axes[1].deadzone = flash_setting[NO_AX1_DEADZONE];
    gp_dev->analog_stick[GP_LEFT].axes[1].center = flash_setting[NO_AX1_CENTER];
    gp_dev->analog_stick[GP_LEFT].axes[1].min = flash_setting[NO_AX1_MIN];
    gp_dev->analog_stick[GP_LEFT].axes[1].max = flash_setting[NO_AX1_MAX];
    gp_dev->analog_stick[GP_LEFT].axes[1].invert_direction = flash_setting[NO_AX1_INVERT];
    gp_dev->analog_stick[GP_LEFT].axes[1].virtual_axis = flash_setting[NO_AX1_VIRTUAL_AXIS];

    gp_dev->analog_stick[GP_RIGHT].virtual_stick_side = RIGHT_STICK;
    gp_dev->analog_stick[GP_RIGHT].axes[0].deadzone = flash_setting[NO_AX2_DEADZONE];
    gp_dev->analog_stick[GP_RIGHT].axes[0].center = flash_setting[NO_AX2_CENTER];
    gp_dev->analog_stick[GP_RIGHT].axes[0].min = flash_setting[NO_AX2_MIN];
    gp_dev->analog_stick[GP_RIGHT].axes[0].max = flash_setting[NO_AX2_MAX];
    gp_dev->analog_stick[GP_RIGHT].axes[0].invert_direction = flash_setting[NO_AX2_INVERT];
    gp_dev->analog_stick[GP_RIGHT].axes[0].virtual_axis = flash_setting[NO_AX2_VIRTUAL_AXIS];
    gp_dev->analog_stick[GP_RIGHT].axes[1].deadzone = flash_setting[NO_AX3_DEADZONE];
    gp_dev->analog_stick[GP_RIGHT].axes[1].center = flash_setting[NO_AX3_CENTER];
    gp_dev->analog_stick[GP_RIGHT].axes[1].min = flash_setting[NO_AX3_MIN];
    gp_dev->analog_stick[GP_RIGHT].axes[1].max = flash_setting[NO_AX3_MAX];
    gp_dev->analog_stick[GP_RIGHT].axes[1].invert_direction = flash_setting[NO_AX3_INVERT];
    gp_dev->analog_stick[GP_RIGHT].axes[1].virtual_axis = flash_setting[NO_AX3_VIRTUAL_AXIS];

    gp_dev->ps1_analog_en = 0;
    
}

void center_analog_sticks(struct gamepad_user *gp_dev)
{
    /*update the center value of both sticks*/
    gp_dev->analog_stick[GP_LEFT].axes[0].center = gp_dev->analog_stick[GP_LEFT].axes[0].raw_analog_stick_data;
    gp_dev->analog_stick[GP_LEFT].axes[1].center = gp_dev->analog_stick[GP_LEFT].axes[1].raw_analog_stick_data;
    gp_dev->analog_stick[GP_RIGHT].axes[0].center = gp_dev->analog_stick[GP_RIGHT].axes[0].raw_analog_stick_data;
    gp_dev->analog_stick[GP_RIGHT].axes[1].center = gp_dev->analog_stick[GP_RIGHT].axes[1].raw_analog_stick_data;

    /*here we reset the min and max for both sticks to the center value, so the calibration should find new values in all cases*/
    gp_dev->analog_stick[GP_LEFT].axes[0].min = gp_dev->analog_stick[GP_LEFT].axes[0].center;
    gp_dev->analog_stick[GP_LEFT].axes[0].max = gp_dev->analog_stick[GP_LEFT].axes[0].center;
    gp_dev->analog_stick[GP_LEFT].axes[1].min = gp_dev->analog_stick[GP_LEFT].axes[1].center;
    gp_dev->analog_stick[GP_LEFT].axes[1].max = gp_dev->analog_stick[GP_LEFT].axes[1].center;
    gp_dev->analog_stick[GP_RIGHT].axes[0].min = gp_dev->analog_stick[GP_RIGHT].axes[0].center;
    gp_dev->analog_stick[GP_RIGHT].axes[0].max = gp_dev->analog_stick[GP_RIGHT].axes[0].center;
    gp_dev->analog_stick[GP_RIGHT].axes[1].min = gp_dev->analog_stick[GP_RIGHT].axes[1].center;
    gp_dev->analog_stick[GP_RIGHT].axes[1].max = gp_dev->analog_stick[GP_RIGHT].axes[1].center;
}

void calibrate_analog_sticks(struct gamepad_user *gp_dev)
{
    if(gp_dev->analog_stick[GP_LEFT].axes[0].raw_analog_stick_data < gp_dev->analog_stick[GP_LEFT].axes[0].min)
        gp_dev->analog_stick[GP_LEFT].axes[0].min = gp_dev->analog_stick[GP_LEFT].axes[0].raw_analog_stick_data;
    if(gp_dev->analog_stick[GP_LEFT].axes[0].raw_analog_stick_data > gp_dev->analog_stick[GP_LEFT].axes[0].max)
        gp_dev->analog_stick[GP_LEFT].axes[0].max = gp_dev->analog_stick[GP_LEFT].axes[0].raw_analog_stick_data;
    if(gp_dev->analog_stick[GP_LEFT].axes[1].raw_analog_stick_data < gp_dev->analog_stick[GP_LEFT].axes[1].min)
        gp_dev->analog_stick[GP_LEFT].axes[1].min = gp_dev->analog_stick[GP_LEFT].axes[1].raw_analog_stick_data;
    if(gp_dev->analog_stick[GP_LEFT].axes[1].raw_analog_stick_data > gp_dev->analog_stick[GP_LEFT].axes[1].max)
        gp_dev->analog_stick[GP_LEFT].axes[1].max = gp_dev->analog_stick[GP_LEFT].axes[1].raw_analog_stick_data;
    
    if(gp_dev->analog_stick[GP_RIGHT].axes[0].raw_analog_stick_data < gp_dev->analog_stick[GP_RIGHT].axes[0].min)
        gp_dev->analog_stick[GP_RIGHT].axes[0].min = gp_dev->analog_stick[GP_RIGHT].axes[0].raw_analog_stick_data;
    if(gp_dev->analog_stick[GP_RIGHT].axes[0].raw_analog_stick_data > gp_dev->analog_stick[GP_RIGHT].axes[0].max)
        gp_dev->analog_stick[GP_RIGHT].axes[0].max = gp_dev->analog_stick[GP_RIGHT].axes[0].raw_analog_stick_data;            
    if(gp_dev->analog_stick[GP_RIGHT].axes[1].raw_analog_stick_data < gp_dev->analog_stick[GP_RIGHT].axes[1].min)
        gp_dev->analog_stick[GP_RIGHT].axes[1].min = gp_dev->analog_stick[GP_RIGHT].axes[1].raw_analog_stick_data;
    if(gp_dev->analog_stick[GP_RIGHT].axes[1].raw_analog_stick_data > gp_dev->analog_stick[GP_RIGHT].axes[1].max)
        gp_dev->analog_stick[GP_RIGHT].axes[1].max = gp_dev->analog_stick[GP_RIGHT].axes[1].raw_analog_stick_data;

}

void store_stick_calibration(struct gamepad_user *gp_dev, uint32_t *flash_setting)
{
    flash_setting[NO_AX0_DEADZONE] = gp_dev->analog_stick[GP_LEFT].axes[0].deadzone;
    flash_setting[NO_AX0_CENTER] = gp_dev->analog_stick[GP_LEFT].axes[0].center;
    flash_setting[NO_AX0_MIN] = gp_dev->analog_stick[GP_LEFT].axes[0].min;
    flash_setting[NO_AX0_MAX] = gp_dev->analog_stick[GP_LEFT].axes[0].max;
    flash_setting[NO_AX0_INVERT] = gp_dev->analog_stick[GP_LEFT].axes[0].invert_direction;
    flash_setting[NO_AX0_VIRTUAL_AXIS] = gp_dev->analog_stick[GP_LEFT].axes[0].virtual_axis;
    flash_setting[NO_AX1_DEADZONE] = gp_dev->analog_stick[GP_LEFT].axes[1].deadzone;
    flash_setting[NO_AX1_CENTER] = gp_dev->analog_stick[GP_LEFT].axes[1].center;
    flash_setting[NO_AX1_MIN] = gp_dev->analog_stick[GP_LEFT].axes[1].min;
    flash_setting[NO_AX1_MAX] = gp_dev->analog_stick[GP_LEFT].axes[1].max;
    flash_setting[NO_AX1_INVERT] = gp_dev->analog_stick[GP_LEFT].axes[1].invert_direction;
    flash_setting[NO_AX1_VIRTUAL_AXIS] = gp_dev->analog_stick[GP_LEFT].axes[1].virtual_axis;
    
    flash_setting[NO_AX2_DEADZONE] = gp_dev->analog_stick[GP_RIGHT].axes[0].deadzone;
    flash_setting[NO_AX2_CENTER] = gp_dev->analog_stick[GP_RIGHT].axes[0].center;
    flash_setting[NO_AX2_MIN] = gp_dev->analog_stick[GP_RIGHT].axes[0].min;
    flash_setting[NO_AX2_MAX] = gp_dev->analog_stick[GP_RIGHT].axes[0].max;
    flash_setting[NO_AX2_INVERT] = gp_dev->analog_stick[GP_RIGHT].axes[0].invert_direction;
    flash_setting[NO_AX2_VIRTUAL_AXIS] = gp_dev->analog_stick[GP_RIGHT].axes[0].virtual_axis;
    flash_setting[NO_AX3_DEADZONE] = gp_dev->analog_stick[GP_RIGHT].axes[1].deadzone;
    flash_setting[NO_AX3_CENTER] = gp_dev->analog_stick[GP_RIGHT].axes[1].center;
    flash_setting[NO_AX3_MIN] = gp_dev->analog_stick[GP_RIGHT].axes[1].min;
    flash_setting[NO_AX3_MAX] = gp_dev->analog_stick[GP_RIGHT].axes[1].max;
    flash_setting[NO_AX3_INVERT] = gp_dev->analog_stick[GP_RIGHT].axes[1].invert_direction;
    flash_setting[NO_AX3_VIRTUAL_AXIS] = gp_dev->analog_stick[GP_RIGHT].axes[1].virtual_axis;
}

void invert_stick(struct gamepad_user *gp_dev)
{
    invert_single_axis(&gp_dev->analog_stick[GP_LEFT].axes[0]);
    invert_single_axis(&gp_dev->analog_stick[GP_LEFT].axes[1]);
    invert_single_axis(&gp_dev->analog_stick[GP_RIGHT].axes[0]);
    invert_single_axis(&gp_dev->analog_stick[GP_RIGHT].axes[1]);
}

void invert_single_axis(struct axis *axis)
{
    if((axis->processed_stick_data > THRESHOLD_INVERT_H) || (axis->processed_stick_data < THRESHOLD_INVERT_L))
        if(axis->invert_direction == TRUE)
            axis->invert_direction = FALSE;
        else
            axis->invert_direction = TRUE;
}

void inc_dec_deadzone(struct gamepad_user *gp_dev, uint8_t inc_dec)
{
    uint32_t deadzone = (uint32_t)gp_dev->analog_stick[GP_RIGHT].axes[0].deadzone;
    /*handle incrementing/decrementing based on the direction*/
    inc_dec_setting(&deadzone, inc_dec, 1, 100, 0);

    gp_dev->analog_stick[GP_RIGHT].axes[0].deadzone = (uint16_t)deadzone;
    gp_dev->analog_stick[GP_RIGHT].axes[1].deadzone = (uint16_t)deadzone;
    gp_dev->analog_stick[GP_LEFT].axes[0].deadzone = (uint16_t)deadzone;
    gp_dev->analog_stick[GP_LEFT].axes[1].deadzone = (uint16_t)deadzone;

    return;
}

void analog_emulation_toggle(struct gamepad_user *gp_dev)
{
    if(gp_dev->ps1_analog_en == 0) gp_dev->ps1_analog_en = 1;
    else gp_dev->ps1_analog_en = 0;

    return;
}

void run_analog_emulation(struct gamepad_user *gp_dev)
{
    volatile uint32_t digital_states = gp_dev->current_digital_states;

    /*first button -> right*/
    if(gp_dev->current_analog_states[gp_dev->analog_stick[GP_LEFT].virtual_stick_side + gp_dev->analog_stick[GP_LEFT].axes[0].virtual_axis] > 200)
        digital_states = digital_states & ~((uint32_t)1<<BUTTON_RIGHT); 

    /*second button -> left*/
    if(gp_dev->current_analog_states[gp_dev->analog_stick[GP_LEFT].virtual_stick_side + gp_dev->analog_stick[GP_LEFT].axes[0].virtual_axis] < 54)
        digital_states = digital_states & ~((uint32_t)1<<BUTTON_LEFT);
 
    /*first button -> down*/
    if(gp_dev->current_analog_states[gp_dev->analog_stick[GP_LEFT].virtual_stick_side + gp_dev->analog_stick[GP_LEFT].axes[1].virtual_axis] > 200)
        digital_states = digital_states & ~((uint32_t)1<<BUTTON_DOWN); 

    /*second button -> up*/
    if(gp_dev->current_analog_states[gp_dev->analog_stick[GP_LEFT].virtual_stick_side + gp_dev->analog_stick[GP_LEFT].axes[1].virtual_axis] < 54)
        digital_states = digital_states & ~((uint32_t)1<<BUTTON_UP);

    gp_dev->current_digital_states = digital_states;

    return;

}



        




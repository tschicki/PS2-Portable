#include "gamepad_i2c.h"

static syscon_error_t extio_get_regs(struct extio *extio_dev, uint8_t start_address, uint8_t *rw_buffer, uint8_t no_of_regs)
{
    int8_t result = ERROR_OK;

    extio_dev->extio_busy = 1;
    if (i2c_read(extio_dev->interface, extio_dev->device_address, start_address, rw_buffer, no_of_regs) != I2C_SUCCESS)
        result = GENERIC_ERROR;
    extio_dev->extio_busy = 0;
    return result;
}

static syscon_error_t extio_set_regs(struct extio *extio_dev, uint8_t start_address, uint8_t *rw_buffer, uint8_t no_of_regs)
{
    int8_t result = ERROR_OK;

    extio_dev->extio_busy = 1;
    if (i2c_write(extio_dev->interface, extio_dev->device_address, start_address, rw_buffer, no_of_regs) != I2C_SUCCESS)
        result = GENERIC_ERROR;
    extio_dev->extio_busy = 0;
    return result;
}


syscon_error_t extio_poll_inputs(struct extio *extio_dev, uint16_t *button_states, uint16_t *analog_x, uint16_t *analog_y)
{
    uint8_t rw_buffer[6] = {0};

    if (extio_get_regs(extio_dev, REG00_BUTTONS_L, rw_buffer, 6) != ERROR_OK)
        return EXTIO_FAILED_TO_GET_INPUTS;

    *button_states = (uint16_t)rw_buffer[0] | ((uint16_t)rw_buffer[1] << 8);
    *analog_x = (uint16_t)rw_buffer[2] | ((uint16_t)rw_buffer[3] << 8);
    *analog_y = (uint16_t)rw_buffer[4] | ((uint16_t)rw_buffer[5] << 8);

    return ERROR_OK;
}

syscon_error_t extio_set_LED(struct extio *extio_dev, uint8_t red, uint8_t green, uint8_t blue)
{
    uint8_t rw_buffer[3] = {red, green, blue};

    if (extio_set_regs(extio_dev, REG07_LED_R, rw_buffer, 3) != ERROR_OK)
        return EXTIO_FAILED_TO_SET_LED;

    return ERROR_OK;
}

syscon_error_t extio_set_rumble(struct extio *extio_dev, uint8_t pwm)
{
    uint8_t rw_buffer = pwm;

    if (extio_set_regs(extio_dev, REG06_MOTOR, &rw_buffer, 1) != ERROR_OK)
        return EXTIO_FAILED_TO_SET_RUMBLE;

    return ERROR_OK;
}


#ifndef GAMEPAD_I2C_h
#define GAMEPAD_I2C_h

#include "gamepad_i2c_registers.h"
#include "MCU_interface.h"
#include "Error_Codes.h"

/*pincount per io expander, 16 for this chip*/
#define IO_EXPANDER_GPIO_AMOUNT 16
/*IO defines specific to the external GPIO*/
#define OUTPUT_DRIVE_025    0x00
#define OUTPUT_DRIVE_05     0x01
#define OUTPUT_DRIVE_075    0x02
#define OUTPUT_DRIVE_1      0x03
#define PORT_0              0
#define PORT_1              1

struct extio{
    i2c_inst_t *interface;
    uint8_t device_address;
    uint8_t extio_busy;
};

static syscon_error_t extio_get_regs(struct extio *extio_dev, uint8_t start_address, uint8_t *rw_buffer, uint8_t no_of_regs);
static syscon_error_t extio_set_regs(struct extio *extio_dev, uint8_t start_address, uint8_t *rw_buffer, uint8_t no_of_regs);
syscon_error_t extio_poll_inputs(struct extio *extio_dev, uint16_t *button_states, uint16_t *analog_x, uint16_t *analog_y);
syscon_error_t extio_set_LED(struct extio *extio_dev, uint8_t red, uint8_t green, uint8_t blue);
syscon_error_t extio_set_rumble(struct extio *extio_dev, uint8_t pwm);


/*macro to set and reset bits in the 16 bit control word*/
#define EXTIO_SET_BITS(data, position, bit) \
    ((data & ~(0x0001 << position)) | ((bit & 0x0001) << position))

#define EXTIO_GET_BITS(data, position) \
    (uint8_t)((data & (0x0001 << position)) >> position)

#endif
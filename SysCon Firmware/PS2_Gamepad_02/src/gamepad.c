#include "gamepad.h"

uint8_t side_flag = 0;
struct Gamepad io;

uint16_t rumble_last = 0;
volatile uint8_t rumble_boost_flag = 0;

static struct
{
    uint8_t mem[10];
    uint8_t mem_size;
    uint8_t mem_address;
    bool mem_address_written;
} registers;

int main(void)
{
    /*first thing to do is to determine the gamepad side for initialization*/
    io.side_sel = determine_side(SIDE_SEL);
    
    /*initialize the gamepad IO, this contains left and right gamepad*/
    GPIO_init(&io);
    
    /*set the register size for the I2C slave and clear all registers*/
    registers.mem_size = sizeof(registers.mem);
    for(uint8_t i = 0; i < registers.mem_size; i++)
        registers.mem[i] = 0;

    /*initialize the slave based on the jumper positions*/
    if (io.side_sel == SIDE_RIGHT)
        i2c_slave_init(SLAVE_I2C, SLAVE_ADDRESS_RIGHT, &i2c_slave_handler);
    else
        i2c_slave_init(SLAVE_I2C, SLAVE_ADDRESS_LEFT, &i2c_slave_handler);

    while (1)
    {
        io_handler();
        if(io.side_sel == SIDE_RIGHT) led_handler();
        motor_handler();
    }
}

void i2c_slave_handler(i2c_inst_t *i2c, i2c_slave_event_t event)
{
    switch (event)
    {
    case I2C_SLAVE_RECEIVE: // master has written some data
        if (!registers.mem_address_written)
        {
            // writes always start with the memory address
            registers.mem_address = i2c_read_byte_raw(i2c);
            registers.mem_address_written = true;
        }
        else
        {
            // save into memory
            if (registers.mem_address <= registers.mem_size)
            {
                registers.mem[registers.mem_address] = i2c_read_byte_raw(i2c);
                registers.mem_address++;
            }
        }
        break;
    case I2C_SLAVE_REQUEST: // master is requesting data
        // load from memory
        if (registers.mem_address <= registers.mem_size)
        {
            i2c_write_byte_raw(i2c, registers.mem[registers.mem_address]);
            registers.mem_address++;
        }
        else
        {
            i2c_write_byte_raw(i2c, 0x00);
        }
        break;
    case I2C_SLAVE_FINISH: // master has signalled Stop / Restart
        registers.mem_address_written = false;
        break;
    default:
        break;
    }
}

void io_handler(void)
{
    /*read all button inputs and place them in registers 0 and 1*/
    /*read the analog sticks and stick voltage -> convert and store in reg 2-5 */
    /*run this funtion in main, as the slave is based on IRQ*/
    uint16_t digitalstates = 0;
    uint8_t pinstate = 0;

    uint16_t analog_x, analog_y, analog_vcc = 0;

    if (io.side_sel == SIDE_RIGHT)
    {
        for (uint8_t i = 0; i < RIGHT_PINCOUNT; i++)
        {
            pinstate = sys_get_pin(io.button_right[i]);
            digitalstates = digitalstates | (((uint16_t)pinstate) << io.button_right[i].shift);
        }
    }
    else
    {
        for (uint8_t i = 0; i < LEFT_PINCOUNT; i++)
        {
            pinstate = sys_get_pin(io.button_left[i]);
            digitalstates = digitalstates | (((uint16_t)pinstate) << io.button_left[i].shift);
        }
    }
    /*write digital states into the register map*/
    registers.mem[REG00_BUTTONS_L] = (uint8_t)digitalstates;
    registers.mem[REG01_BUTTONS_H] = (uint8_t)(digitalstates >> 8);

    /*adc read analog stick; consider splitting it into left&right in the future*/
    adc_select_input(X_ADC);
    analog_x = adc_read();
    adc_select_input(Y_ADC);
    analog_y = adc_read();
    adc_select_input(STICK_VOLTAGE_ADC);
    analog_vcc = adc_read();

    /*if VCC is 1.8V, multiply the values and cap them at 12bit; to "fake" the full range*/
    if (analog_vcc < 2400)
    {
        analog_x = analog_x * 2;
        analog_y = analog_y * 2;

        if (analog_x > 4096)
            analog_x = 4096;
        if (analog_y > 4096)
            analog_y = 4096;
    }
    /*write analog states into the register map*/
    registers.mem[REG02_ANALOG_X_L] = (uint8_t)analog_x;
    registers.mem[REG03_ANALOG_X_H] = (uint8_t)(analog_x >> 8);
    registers.mem[REG04_ANALOG_Y_L] = (uint8_t)analog_y;
    registers.mem[REG05_ANALOG_Y_H] = (uint8_t)(analog_y >> 8);
}

/*read the LED regs and set the respective PWM outputs*/
void led_handler(void)
{
    uint16_t red = registers.mem[REG07_LED_R] * WRAP_MULTIPLIER;
    uint16_t green = registers.mem[REG08_LED_G] * WRAP_MULTIPLIER;
    uint16_t blue = registers.mem[REG09_LED_B] * WRAP_MULTIPLIER;

    if(red > PWM_LED_WRAP) red = PWM_LED_WRAP;
    if(green > PWM_LED_WRAP) green = PWM_LED_WRAP;
    if(blue > PWM_LED_WRAP) blue = PWM_LED_WRAP;

    /*invert set value due to LED connection*/
    red = PWM_LED_WRAP - red;
    green = PWM_LED_WRAP - green;
    blue = PWM_LED_WRAP - blue;

    pwm_set_chan_level(io.slice_r, io.chan_r, red);
    pwm_set_chan_level(io.slice_g, io.chan_g, green);
    pwm_set_chan_level(io.slice_b, io.chan_b, blue);
}

/*read the Rumble PWM reg and set the respective PWM output*/
void motor_handler(void)
{
    /*map the register to PWM*/
    /*0 should turn off rumble; 1 should generate a voltage just high enough to start the motor (2V?)*/
    /*translate the motor startup voltage to PWM_MOT_MIN: 
    5100(PWM_MOT_WRAP) is equivalent to 3.3V -> PWM_MOT_MIN = measured startup voltage [V] * (5100/3.3)*/
    uint8_t rumble_strength = registers.mem[REG06_MOTOR];
    uint16_t pwm = map_to_pwm(rumble_strength, 0, 255, PWM_MOT_MIN, PWM_MOT_WRAP);

    /*if the register is set to zero we force off the motor; any vlue above that will start rumble*/
    if(rumble_strength == 0) pwm = 0;

    pwm_set_chan_level(io.slice_mot, io.chan_mot, pwm);

}

uint16_t map_to_pwm(uint8_t input, uint8_t in_min, uint8_t in_max, uint16_t out_min, uint16_t out_max)
{
    /*clip the input*/
    if (input > in_max)
        input = in_max;
    if (input < in_min)
        input = in_min;

    float delta_out = (float)out_max - (float)out_min;
    float delta_in = (float)in_max - (float)in_min;
    float factor = delta_out / delta_in;
    float result_f = (((float)(input - in_min)) * factor) + (float)out_min;
    uint16_t mapped_pwm = (uint16_t)result_f;

    if(mapped_pwm > out_max) mapped_pwm = out_max;
    if(mapped_pwm < out_min) mapped_pwm = out_min;
    //mapped_pwm = (uint8_t)((input - in_min) * (out_max - out_min) / (in_max - in_min) + out_min);

    return mapped_pwm;
}

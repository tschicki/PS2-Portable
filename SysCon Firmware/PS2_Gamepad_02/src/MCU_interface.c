#include "MCU_interface.h"

uint8_t determine_side(uint8_t side_sel_pin)
{
  gpio_init(side_sel_pin);
  gpio_set_dir(side_sel_pin, GPIO_IN);

  return gpio_get(side_sel_pin);
}

void GPIO_init(struct Gamepad *io)
{
  /*first initialize the button structs*/
  init_buttons_struct(io);
  /*then initialize the IOs for all buttons + all special functions for each gamepad side*/
  if(io->side_sel == SIDE_RIGHT)
  {
    for (uint8_t i = 0; i < RIGHT_PINCOUNT; i++) init_button(&io->button_right[i]);
    LED_init(io, PWM_R, PWM_G, PWM_B);
    Motor_init(io, MOT_RIGHT);
  }
  else
  {
    for (uint8_t i = 0; i < LEFT_PINCOUNT; i++) init_button(&io->button_left[i]);
    Motor_init(io, MOT_LEFT);
  }

  /*the initialize the IO for reading the analog sticks, pinout common to both gamepads*/
  adc_init();
  adc_gpio_init(STICK_X);
  adc_gpio_init(STICK_Y);
  adc_gpio_init(STICK_VOLTAGE);

  /*lastly initialize the IO for I2C, pinout common to both gamepads*/
  i2c_interface_init(SLAVE_I2C, SLAVE_I2C_SDA_PIN, SLAVE_I2C_SCL_PIN, SLAVE_I2C_BAUD_RATE);
}

void i2c_interface_init(i2c_inst_t *hi2c, uint8_t SDA_Pin, uint8_t SCL_Pin, uint32_t baudrate)
{
  
  gpio_init(SDA_Pin);
  gpio_set_function(SDA_Pin, GPIO_FUNC_I2C);
  gpio_pull_up(SDA_Pin);

  gpio_init(SCL_Pin);
  gpio_set_function(SCL_Pin, GPIO_FUNC_I2C);
  gpio_pull_up(SCL_Pin);
  
  i2c_init(hi2c, baudrate);
  
}

static void LED_init(struct Gamepad *io, uint8_t IO_R, uint8_t IO_G, uint8_t IO_B)
{
  /*init the LED_EN output*/
  init_button(&io->LED_enable);

  /*initialize the io for the LED output, set the default level and enable the output*/
  PWM_init(&io->slice_r, &io->chan_r, IO_R, PWM_LED_WRAP);
  PWM_init(&io->slice_g, &io->chan_g, IO_G, PWM_LED_WRAP);
  PWM_init(&io->slice_b, &io->chan_b, IO_B, PWM_LED_WRAP);  

  /*enable the LED*/
  sys_set_pin(io->LED_enable, SYSPIN_LOW);
  
}

static void Motor_init(struct Gamepad *io, uint8_t IO)
{
  PWM_init(&io->slice_mot, &io->chan_mot, IO, PWM_MOT_WRAP);
}

static void PWM_init(uint *slice, uint *channel, uint8_t IO, uint16_t wrap)
{
  uint t_slice, t_channel = 0;

  t_slice = pwm_gpio_to_slice_num (IO);
  t_channel = pwm_gpio_to_channel(IO);
  gpio_set_function(IO, GPIO_FUNC_PWM);
  pwm_set_wrap(t_slice, wrap);
  pwm_set_chan_level(t_slice, t_channel, wrap);
  pwm_set_enabled(t_slice, true);

  *slice = t_slice;
  *channel = t_channel;
}

static void init_buttons_struct(struct Gamepad *io)
{
  /*LED enable for right gamepad*/
  io->LED_enable.io_pin = 10;
  io->LED_enable.shift = 0;
  io->LED_enable.pin_mode = SYSPIN_OUT_OD;
  io->LED_enable.drive_strength = GPIO_DRIVE_STRENGTH_8MA;
  io->LED_enable.pin_speed = GPIO_SLEW_RATE_SLOW;
  io->LED_enable.pull_mode = SYSPIN_NOPULL;
  io->LED_enable.reset_state = SYSPIN_HIZ;

  /*Inputs*/
  io->button_right[R3].io_pin = 25;
  io->button_right[R3].shift = 2;
  io->button_right[R3].pin_mode = SYSPIN_INPUT;
  io->button_right[R3].pull_mode = SYSPIN_PULLUP;

  io->button_right[START].io_pin = 24;
  io->button_right[START].shift = 3;
  io->button_right[START].pin_mode = SYSPIN_INPUT;
  io->button_right[START].pull_mode = SYSPIN_PULLUP;

  io->button_right[CIRCLE].io_pin = 5;
  io->button_right[CIRCLE].shift = 13;
  io->button_right[CIRCLE].pin_mode = SYSPIN_INPUT;
  io->button_right[CIRCLE].pull_mode = SYSPIN_PULLUP;

  io->button_right[TRIANGLE].io_pin = 4;
  io->button_right[TRIANGLE].shift = 12;
  io->button_right[TRIANGLE].pin_mode = SYSPIN_INPUT;
  io->button_right[TRIANGLE].pull_mode = SYSPIN_PULLUP;

  io->button_right[CROSS].io_pin = 6;
  io->button_right[CROSS].shift = 14;
  io->button_right[CROSS].pin_mode = SYSPIN_INPUT;
  io->button_right[CROSS].pull_mode = SYSPIN_PULLUP;

  io->button_right[SQUARE].io_pin = 3;
  io->button_right[SQUARE].shift = 15;
  io->button_right[SQUARE].pin_mode = SYSPIN_INPUT;
  io->button_right[SQUARE].pull_mode = SYSPIN_PULLUP;

  io->button_right[R2].io_pin = 8;
  io->button_right[R2].shift = 9;
  io->button_right[R2].pin_mode = SYSPIN_INPUT;
  io->button_right[R2].pull_mode = SYSPIN_PULLUP;

  io->button_right[R1].io_pin = 7;
  io->button_right[R1].shift = 11;
  io->button_right[R1].pin_mode = SYSPIN_INPUT;
  io->button_right[R1].pull_mode = SYSPIN_PULLUP;

  io->button_right[SELECT].io_pin = 23;
  io->button_right[SELECT].shift = 0;
  io->button_right[SELECT].pin_mode = SYSPIN_INPUT;
  io->button_right[SELECT].pull_mode = SYSPIN_PULLUP;

/*left side-------------------------------------*/
  io->button_left[L2].io_pin = 3;
  io->button_left[L2].shift = 8;
  io->button_left[L2].pin_mode = SYSPIN_INPUT;
  io->button_left[L2].pull_mode = SYSPIN_PULLUP;

  io->button_left[L1].io_pin = 4;
  io->button_left[L1].shift = 10;
  io->button_left[L1].pin_mode = SYSPIN_INPUT;
  io->button_left[L1].pull_mode = SYSPIN_PULLUP;

  io->button_left[DOWN].io_pin = 0;
  io->button_left[DOWN].shift = 6;
  io->button_left[DOWN].pin_mode = SYSPIN_INPUT;
  io->button_left[DOWN].pull_mode = SYSPIN_PULLUP;

  io->button_left[RIGHT].io_pin = 7;
  io->button_left[RIGHT].shift = 5;
  io->button_left[RIGHT].pin_mode = SYSPIN_INPUT;
  io->button_left[RIGHT].pull_mode = SYSPIN_PULLUP;

  io->button_left[LEFT].io_pin = 1;
  io->button_left[LEFT].shift = 7;
  io->button_left[LEFT].pin_mode = SYSPIN_INPUT;
  io->button_left[LEFT].pull_mode = SYSPIN_PULLUP;

  io->button_left[UP].io_pin = 6;
  io->button_left[UP].shift = 4;
  io->button_left[UP].pin_mode = SYSPIN_INPUT;
  io->button_left[UP].pull_mode = SYSPIN_PULLUP;

  io->button_left[L3].io_pin = 2;
  io->button_left[L3].shift = 1;
  io->button_left[L3].pin_mode = SYSPIN_INPUT;
  io->button_left[L3].pull_mode = SYSPIN_PULLUP;
}

static void init_button(struct io_location *io)
{
    /*first initialize the IO*/
    gpio_init(io->io_pin);
    /*then set the specific parameters, based on the pin mode*/
    switch (io->pin_mode)
    {
      case SYSPIN_INPUT:
        gpio_set_dir(io->io_pin, GPIO_IN);
        break;

      case SYSPIN_OUTPUT:
        gpio_set_dir(io->io_pin, GPIO_OUT);
        gpio_set_slew_rate(io->io_pin, io->pin_speed);
        gpio_set_drive_strength(io->io_pin, io->drive_strength);
        sys_set_pin(*io, io->reset_state);
        break;

      case SYSPIN_OUT_OD:
        gpio_set_dir(io->io_pin, GPIO_IN);
        gpio_set_slew_rate(io->io_pin, io->pin_speed);
        gpio_set_drive_strength(io->io_pin, io->drive_strength);
        sys_set_pin(*io, io->reset_state);
        break;
    }

    /*then set pull resistors*/
    if (io->pull_mode == SYSPIN_PULLUP)
      gpio_pull_up(io->io_pin);
    else if (io->pull_mode == SYSPIN_PULLDOWN)
      gpio_pull_down(io->io_pin);
    else gpio_disable_pulls(io->io_pin);
}


void sys_set_pin(struct io_location io, uint8_t pin_state)
{
  /*use this if the io is PP*/
  if (io.pin_mode == SYSPIN_OUTPUT)
    gpio_put(io.io_pin, pin_state);
  /*use this if the io is OD*/
  else if (io.pin_mode == SYSPIN_OUT_OD)
  {
    if (pin_state == SYSPIN_HIZ)
      gpio_set_dir(io.io_pin, GPIO_IN);
    else if (pin_state == SYSPIN_LOW)
    {
      gpio_put(io.io_pin, pin_state);
      gpio_set_dir(io.io_pin, GPIO_OUT);
    }
  }
}

uint8_t sys_get_pin(struct io_location io)
{
  uint8_t result;
  result = gpio_get(io.io_pin);
  return result;
}

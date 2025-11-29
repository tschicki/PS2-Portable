#include "MCU_interface.h"

struct SysCon_Pins io;

syscon_error_t i2c_read(i2c_inst_t *i2c, uint8_t slave_addr, uint8_t reg_addr, uint8_t *reg_data, uint32_t len)
{

  int result = i2c_write_blocking(i2c, slave_addr, &reg_addr, 1, true); // true to keep master control of bus
  if (result != 1)
    return GENERIC_ERROR;
  result = i2c_read_blocking(i2c, slave_addr, reg_data, len, false); // false - finished with
  if (result != len)
    return GENERIC_ERROR;
/*-------------------------------------------------------------------------------------------*/
/*DEBUG*/
  #if defined (I2C_DEBUG) 
  uint8_t *p;
  p = reg_data;
  printf("I2C read from: 0x%02X\r\n", (slave_addr));
  for (uint8_t i = 0; i < len; i++)
  {
    printf("Reg %02X : 0x%02X\r\n", reg_addr, *p);
    p++;
    reg_addr++;
  }
  #endif
/*-------------------------------------------------------------------------------------------*/
  return ERROR_OK;
}

syscon_error_t i2c_read_no_regs(i2c_inst_t *i2c, uint8_t slave_addr, uint8_t *reg_data, uint32_t len)
{

  if(i2c_read_blocking(i2c, slave_addr, reg_data, len, false) != len)
    return GENERIC_ERROR;
/*-------------------------------------------------------------------------------------------*/
/*DEBUG*/
  #if defined (I2C_DEBUG) 
  uint8_t *p;
  p = reg_data;
  printf("I2C read from: 0x%02X\r\n", (slave_addr));
  for (uint8_t i = 0; i < len; i++)
  {
    printf("%u : 0x%02X\r\n",i, *p);
    p++;
  }
  #endif
/*-------------------------------------------------------------------------------------------*/
  return ERROR_OK;
}

syscon_error_t i2c_write(i2c_inst_t *i2c, uint8_t slave_addr, uint8_t reg_addr, uint8_t *reg_data, uint32_t len)
{
  /*required for now to insert the register address into the data array....*/
  uint8_t data[255] = {0};
  for (uint16_t i = 0; i < len; i++)
  {
    data[0] = reg_addr;
    data[i + 1] = reg_data[i];
  }

  int result = i2c_write_blocking(i2c, slave_addr, data, len + 1, false);
  if (result != (len + 1))
    return GENERIC_ERROR;
/*-------------------------------------------------------------------------------------------*/
/*DEBUG*/
  #if defined (I2C_DEBUG) 
  uint8_t *p;
  p = reg_data;
  printf("I2C write to: 0x%02X\r\n", (slave_addr));
  for (uint8_t i = 0; i < len; i++)
  {
    printf("Reg%02X : 0x%02X\r\n", reg_addr, *p);
    p++;
    reg_addr++;
  }
  #endif
/*-------------------------------------------------------------------------------------------*/
  return ERROR_OK;
}

syscon_error_t spi_write(spi_inst_t *spi, uint8_t len, uint8_t *frame)
{
  int result = spi_write_blocking(spi, frame, len);
  if (result != len)
    return GENERIC_ERROR;
  else return ERROR_OK;
}

syscon_error_t spi_read_write(spi_inst_t *spi, uint8_t *source, uint8_t *dest, uint8_t len)
{
  int result = 0;

  if(spi_write_read_blocking(spi, source, dest, len) != len)
    return GENERIC_ERROR;
  else
    return ERROR_OK;
}

void delay_ms(uint32_t period)
{
  sleep_ms(period);
}

void delay_us(uint64_t period)
{
  sleep_us(period);
}

void i2c_interface_init(i2c_inst_t *hi2c, uint8_t SDA_Pin, uint8_t SCL_Pin, uint32_t baudrate)
{
  i2c_init(hi2c, baudrate);
  gpio_set_function(SDA_Pin, GPIO_FUNC_I2C);
  gpio_set_function(SCL_Pin, GPIO_FUNC_I2C);
  gpio_pull_up(SDA_Pin);
  gpio_pull_up(SCL_Pin);
}

void spi_interface_init(spi_inst_t *spi, uint8_t tx_pin, uint8_t rx_pin, uint8_t clk_pin, uint32_t baudrate)
{
  spi_init(spi, baudrate);
  spi_set_format(spi, 8, SPI_CPOL_1, SPI_CPHA_1, SPI_MSB_FIRST);
  gpio_set_function(tx_pin, GPIO_FUNC_SPI);
  gpio_set_function(rx_pin, GPIO_FUNC_SPI);
  gpio_set_function(clk_pin, GPIO_FUNC_SPI);

}

void MX_UART_Init(uart_inst_t *huart0)
{
  uart_init(huart0, UART_BAUD_RATE);
  gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
  gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);
  stdio_init_all();
  printf("SysCon - PROTOTYPE FIRMWARE by Tschicki\r\n");
/*-------------------------------------------------------------------------------------------*/
/*DEBUG*/
  #if defined (BQ_DEBUG) || defined (I2C_DEBUG) || defined (MAX_DEBUG) || defined (STATE_DEBUG)
    printf("Compiled with debug flags enabled:\r\n");
  #endif
  #if defined (BQ_DEBUG) 
  printf("-BQ debug print\n");
  #endif
  #if defined (I2C_DEBUG) 
  printf("-I2C debug print\n");
  #endif
  #if defined (MAX_DEBUG) 
  printf("-MAX debug print\n");
  #endif
  #if defined (STATE_DEBUG) 
  printf("-STATE debug print\n");
  #endif
/*-------------------------------------------------------------------------------------------*/
}

void MX_GPIO_Init(struct SysCon_Pins *io)
{
  /*initialize the syscon io struct with all pin parameters*/
  init_gpio_struct(io);
  /*write the pin configs to the gpio registers*/
  init_gpio_configs(io);

  return;
}

static void init_gpio_struct(struct SysCon_Pins *io)
{
  /*Inputs*/
  io->pin[PIN_POWERBTN].io_pin = 8;
  io->pin[PIN_POWERBTN].pin_mode = SYSPIN_INPUT;
  io->pin[PIN_POWERBTN].pull_mode = SYSPIN_NOPULL;

  io->pin[PIN_PS2_LED_GREEN].io_pin = 24;
  io->pin[PIN_PS2_LED_GREEN].pin_mode = SYSPIN_INPUT;
  io->pin[PIN_PS2_LED_GREEN].pull_mode = SYSPIN_NOPULL;

  io->pin[PIN_PS2_LED_RED].io_pin = 17;
  io->pin[PIN_PS2_LED_RED].pin_mode = SYSPIN_INPUT;
  io->pin[PIN_PS2_LED_RED].pull_mode = SYSPIN_NOPULL;

  io->pin[PIN_DCDC_PG].io_pin = POWER_GOOD_PIN;
  io->pin[PIN_DCDC_PG].pin_mode = SYSPIN_INPUT;
  io->pin[PIN_DCDC_PG].pull_mode = SYSPIN_NOPULL;

  io->pin[PIN_HP_DETECT].io_pin = 25;
  io->pin[PIN_HP_DETECT].pin_mode = SYSPIN_INPUT;
  io->pin[PIN_HP_DETECT].pull_mode = SYSPIN_NOPULL;

  io->pin[PIN_T20_CDONE].io_pin = 15;
  io->pin[PIN_T20_CDONE].pin_mode = SYSPIN_INPUT;
  io->pin[PIN_T20_CDONE].pull_mode = SYSPIN_NOPULL;

  /*Open Drain outputs*/
  io->pin[PIN_CHARGE_EN].io_pin = 9;
  io->pin[PIN_CHARGE_EN].pin_mode = SYSPIN_OUT_OD;
  io->pin[PIN_CHARGE_EN].pull_mode = SYSPIN_NOPULL;
  io->pin[PIN_CHARGE_EN].pin_speed = GPIO_SLEW_RATE_FAST;
  io->pin[PIN_CHARGE_EN].reset_state = SYSPIN_HIZ;
  io->pin[PIN_CHARGE_EN].drive_strength = GPIO_DRIVE_STRENGTH_12MA;

  io->pin[PIN_PS2_RESET].io_pin = 23;
  io->pin[PIN_PS2_RESET].pin_mode = SYSPIN_OUT_OD;
  io->pin[PIN_PS2_RESET].pull_mode = SYSPIN_NOPULL;
  io->pin[PIN_PS2_RESET].pin_speed = GPIO_SLEW_RATE_FAST;
  io->pin[PIN_PS2_RESET].reset_state = SYSPIN_HIZ;
  io->pin[PIN_PS2_RESET].drive_strength = GPIO_DRIVE_STRENGTH_12MA;

  io->pin[PIN_GAMEPAD_ACK].io_pin = GAMEPAD_ACK_PIN;
  io->pin[PIN_GAMEPAD_ACK].pin_mode = SYSPIN_OUT_OD;
  io->pin[PIN_GAMEPAD_ACK].pull_mode = SYSPIN_NOPULL;
  io->pin[PIN_GAMEPAD_ACK].pin_speed = GPIO_SLEW_RATE_FAST;
  io->pin[PIN_GAMEPAD_ACK].reset_state = SYSPIN_HIZ;
  io->pin[PIN_GAMEPAD_ACK].drive_strength = GPIO_DRIVE_STRENGTH_12MA;

  io->pin[PIN_T20_CRESET].io_pin = 16;
  io->pin[PIN_T20_CRESET].pin_mode = SYSPIN_OUT_OD;
  io->pin[PIN_T20_CRESET].pull_mode = SYSPIN_NOPULL;
  io->pin[PIN_T20_CRESET].pin_speed = GPIO_SLEW_RATE_FAST;
  io->pin[PIN_T20_CRESET].reset_state = SYSPIN_HIZ;
  io->pin[PIN_T20_CRESET].drive_strength = GPIO_DRIVE_STRENGTH_12MA;

  /*Push-Pull Outputs*/
  io->pin[PIN_DCDC_EN].io_pin = 27;
  io->pin[PIN_DCDC_EN].pin_mode = SYSPIN_OUTPUT;
  io->pin[PIN_DCDC_EN].pull_mode = SYSPIN_NOPULL;
  io->pin[PIN_DCDC_EN].pin_speed = GPIO_SLEW_RATE_FAST;
  io->pin[PIN_DCDC_EN].reset_state = SYSPIN_LOW;
  io->pin[PIN_DCDC_EN].drive_strength = GPIO_DRIVE_STRENGTH_12MA;

  io->pin[PIN_SYS_FORCE_ON].io_pin = 7;
  io->pin[PIN_SYS_FORCE_ON].pin_mode = SYSPIN_OUTPUT;
  io->pin[PIN_SYS_FORCE_ON].pull_mode = SYSPIN_NOPULL;
  io->pin[PIN_SYS_FORCE_ON].pin_speed = GPIO_SLEW_RATE_FAST;
  io->pin[PIN_SYS_FORCE_ON].reset_state = SYSPIN_LOW;
  io->pin[PIN_SYS_FORCE_ON].drive_strength = GPIO_DRIVE_STRENGTH_12MA;

  io->pin[PIN_T20_CE0].io_pin = 13;
  io->pin[PIN_T20_CE0].pin_mode = SYSPIN_OUTPUT;
  io->pin[PIN_T20_CE0].pull_mode = SYSPIN_NOPULL;
  io->pin[PIN_T20_CE0].pin_speed = GPIO_SLEW_RATE_FAST;
  io->pin[PIN_T20_CE0].reset_state = SYSPIN_LOW;
  io->pin[PIN_T20_CE0].drive_strength = GPIO_DRIVE_STRENGTH_12MA;

  io->pin[PIN_T20_CE1].io_pin = 14;
  io->pin[PIN_T20_CE1].pin_mode = SYSPIN_OUTPUT;
  io->pin[PIN_T20_CE1].pull_mode = SYSPIN_NOPULL;
  io->pin[PIN_T20_CE1].pin_speed = GPIO_SLEW_RATE_FAST;
  io->pin[PIN_T20_CE1].reset_state = SYSPIN_LOW;
  io->pin[PIN_T20_CE1].drive_strength = GPIO_DRIVE_STRENGTH_12MA;

}

static void init_gpio_configs(struct SysCon_Pins *io)
{
  for (uint8_t i = 0; i < SYSCON_PINCOUNT; i++)
  {
    /*first initialize the IO*/
    gpio_init(io->pin[i].io_pin);
    /*then set the specific parameters, based on the pin mode*/
    switch (io->pin[i].pin_mode)
    {
    case SYSPIN_INPUT:
      gpio_set_dir(io->pin[i].io_pin, GPIO_IN);
      break;

    case SYSPIN_OUTPUT:
      gpio_set_dir(io->pin[i].io_pin, GPIO_OUT);
      gpio_set_slew_rate(io->pin[i].io_pin, io->pin[i].pin_speed);
      gpio_set_drive_strength(io->pin[i].io_pin, io->pin[i].drive_strength);
      sys_set_pin(io->pin[i], io->pin[i].reset_state);
      break;

    case SYSPIN_OUT_OD:
      gpio_set_dir(io->pin[i].io_pin, GPIO_IN);
      gpio_set_slew_rate(io->pin[i].io_pin, io->pin[i].pin_speed);
      gpio_set_drive_strength(io->pin[i].io_pin, io->pin[i].drive_strength);
      sys_set_pin(io->pin[i], io->pin[i].reset_state);
      break;
    }

    /*then set pull resistors*/
    if (io->pin[i].pull_mode == SYSPIN_PULLUP)
      gpio_pull_up(io->pin[i].io_pin);
    else if (io->pin[i].pull_mode == SYSPIN_PULLDOWN)
      gpio_pull_down(io->pin[i].io_pin);
    else gpio_disable_pulls(io->pin[i].io_pin);
  }
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

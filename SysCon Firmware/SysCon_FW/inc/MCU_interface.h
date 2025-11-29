#ifndef MCU_interface_h
#define MCU_interface_h

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>


#include "pico/stdlib.h"
#include "pico/stdio/driver.h"
#include "hardware/i2c.h"
#include "hardware/spi.h"
#include "pico/multicore.h"
#include "hardware/pwm.h"
#include "hardware/adc.h"
#include "hardware/flash.h"
#include "hardware/resets.h"
#include "hardware/clocks.h"
#include "hardware/pio.h"
#include "hardware/watchdog.h"

#include "Error_Codes.h"
#include "system_states.h"

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------
/*Defines for mainboard bringup, BE VERY CAREFUL!*/
//#define MFG_STEP_1    /*will configure the firmware to flash the STUSB4500 config upon long-press of the power button, then halt*/
//#define MFG_STEP_2    /*will configure the firmware to enable operation without batteries*/
//#define MFG_STEP_3    /*will configure the firmware to flash the MAX17320 NVM config -> do this only once or twice, very limited write cycles!*/
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------


/*set the firmware version here*/
#define FW_VERSION_MAJOR    0
#define FW_VERSION_MINOR    1
/*Debug print for various functions*/
//#define BQ_DEBUG      /*will enable UART debug print for the BQ25792*/
//#define MAX_DEBUG     /*will enable UART debug print for the MAX17320*/
//#define I2C_DEBUG     /*will enable UART debug print for all I2C communication*/
//#define STUSB_DEBUG   /*will enable UART debug print for the STUSB4500*/
//#define STATE_DEBUG   /*will enable UART debug print for the main state machine*/
#define BLOCK_T20_JTAG  /*will block flashing the FPGA via JTAG during runtime*/


/*no touch zone*/
#if defined(MFG_STEP_1)
    #define DEBUG_WRITE_STUSB_NVM
    #define DEBUG_IGNORE_MAX
    #define BLOCK_CHARGE
    #if defined(MFG_STEP_2) || defined(MFG_STEP_3)
    _Static_assert(0, "Only one MFG step at a time!");
    #endif
#endif

#if defined(MFG_STEP_2)
    #define DEBUG_IGNORE_MAX
    #define BLOCK_CHARGE
    #if defined(MFG_STEP_1) || defined(MFG_STEP_3)
    _Static_assert(0, "Only one MFG step at a time!");
    #endif
#endif

#if defined(MFG_STEP_3)
    #define DEBUG_FLASH_MAX_NVM
    #define BLOCK_CHARGE
    #if defined(MFG_STEP_1) || defined(MFG_STEP_2)
    _Static_assert(0, "Only one MFG step at a time!");
    #endif
#else
    /*the watchdog could trigger while the MAX17320 NVM is updating, that's why we turn it off for this step*/
    #define WATCHDOG_EN
    #define WATCHDOG_TIMEOUT       10000 //ms
#endif

/*general defines*/
#define ENABLE              1
#define DISABLE             0
#define OK                  0
#define NOK                 1
#define TRUE                1
#define FALSE               0
#define ERROR               2
#define I2C_SUCCESS         0
#define I2C_FAIL            1
/*Power management I2C definitions*/
#define PRW_I2C             i2c0
#define PWR_I2C_BAUD_RATE       400000
#define PWR_I2C_SDA_PIN     20
#define PWR_I2C_SCL_PIN     21
/*Gamepad I2C definitions*/
#define GAMEPAD_I2C             i2c1
#define GAMEPAD_I2C_BAUD_RATE   400000
#define GAMEPAD_I2C_SDA_PIN     18
#define GAMEPAD_I2C_SCL_PIN     19
/*Debug UART definitions*/
#define UART                uart0
#define UART_BAUD_RATE      115200
#define UART_TX_PIN         0
#define UART_RX_PIN         1
/*Gamepad SPI definitions*/
#define GAMEPAD_SPI         spi0
#define GAMEPAD_ACK_PIN     6
#define GAMEPAD_SCK_PIN     4
#define GAMEPAD_DATA_PIN    3
#define GAMEPAD_CMD_PIN     2
#define GAMEPAD_ATT_PIN     5
/*FPGA SPI definitions*/
#define FPGA_SPI            spi1
#define FPGA_SPI_BAUDRATE   25000000
#define FPGA_SCK_PIN        10
#define FPGA_RX_PIN         12
#define FPGA_TX_PIN         11
#define T_CRESET_MS         1
#define T_DMIN_MS           1

/*SysCon pin states to set*/
#define SYSPIN_HIGH         1  
#define SYSPIN_LOW          0
#define SYSPIN_HIZ          2
/*SysCon pin configuration defines*/
#define SYSPIN_NOPULL       0
#define SYSPIN_PULLUP       1
#define SYSPIN_PULLDOWN     3
#define SYSPIN_INPUT        4
#define SYSPIN_OUTPUT       5
#define SYSPIN_OUT_OD       6

#define POWER_GOOD_PIN      26


/*SYSCON used pin definitions*/
typedef enum syscone_pins_e
{
    PIN_CHARGE_EN = 0,    
    PIN_DCDC_PG,      
    PIN_POWERBTN,     
    PIN_PS2_RESET,    
    PIN_PS2_LED_GREEN,
    PIN_PS2_LED_RED,  
    PIN_SYS_FORCE_ON, 
    PIN_DCDC_EN,         
    PIN_GAMEPAD_ACK,      
    PIN_HP_DETECT,
    PIN_T20_CRESET,
    PIN_T20_CDONE,
    PIN_T20_CE0,
    PIN_T20_CE1,

    SYSCON_PINCOUNT,
}syscon_pins_t;

struct io_location
{
    uint8_t io_pin;
    uint8_t reset_state;
    uint8_t pin_mode;
    uint8_t pull_mode;
    uint8_t pin_speed;
    uint8_t drive_strength;
};

struct SysCon_Pins
{
    struct io_location pin[SYSCON_PINCOUNT]; 
};


void i2c_interface_init(i2c_inst_t *hi2c, uint8_t SDA_Pin, uint8_t SCL_Pin, uint32_t baudrate);
void spi_interface_init(spi_inst_t *spi, uint8_t tx_pin, uint8_t rx_pin, uint8_t clk_pin, uint32_t baudrate);

void SystemClock_Config(void);

void MX_UART_Init(uart_inst_t *huart0);
static void init_gpio_struct(struct SysCon_Pins *io);
static void init_gpio_configs(struct SysCon_Pins *io);
void MX_GPIO_Init(struct SysCon_Pins *io);


syscon_error_t i2c_read(i2c_inst_t *interface, uint8_t slave_addr, uint8_t reg_addr, uint8_t *reg_data, uint32_t len);
syscon_error_t i2c_read_no_regs(i2c_inst_t *interface, uint8_t slave_addr, uint8_t *reg_data, uint32_t len);
syscon_error_t i2c_write(i2c_inst_t *interface, uint8_t slave_addr, uint8_t reg_addr, uint8_t *reg_data, uint32_t len);
syscon_error_t spi_write(spi_inst_t *spi, uint8_t len, uint8_t *frame);
syscon_error_t spi_read_write(spi_inst_t *spi, uint8_t *source, uint8_t *dest, uint8_t len);

void delay_ms(uint32_t period);
void delay_us(uint64_t period);
void error_handler(int8_t result);
void sys_set_pin(struct io_location io, uint8_t pin_state);
uint8_t sys_get_pin(struct io_location io);

#endif

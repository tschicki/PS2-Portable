#ifndef gamepad_core_h
#define gamepad_core_h

#include "MCU_interface.h"
#include "gamepad_user.h"
#include "gamepad_spi.pio.h"

/*setup for timer of ACK return*/
#define ALARM_ACK 0
#define ALARM_IRQ_ACK TIMER_IRQ_0

#define ALARM_TIMEOUT 3
#define ALARM_IRQ_TIMEOUT TIMER_IRQ_3

/*commands*/
/*initialization value of current operation*/
#define CMD_NO_OPERATION            0x00
/*supported commands*/
#define CMD_POLLING                 0x42
#define CMD_CONFIG_MODE             0x43
#define CMD_PRESSURE_CONFIG         0x40
#define CMD_POLLING_CONFIG_STATUS   0x41
#define CMD_EN_ANALOG               0x44
#define CMD_IDENTITY                0x45
#define CMD_GET_CONSTANT_1          0x46
#define CMD_GET_CONSTANT_2          0x47
#define CMD_GET_CONSTANT_3          0x4C
#define CMD_EN_RUMBLE               0x4D
#define CMD_POLLING_CONFIG          0x4F
/*device modes and type*/
#define DEVICE_MODE_DIGITAL         0x41
#define DEVICE_MODE_ANALOG          0x73
#define DEVICE_MODE_ANALOG_PRESSURE 0x79
#define DEVICE_MODE_ESCAPE          0xF3
#define TYPE_DUALSHOCK              0x03

#define DUALSHOCK_MODE_TIMEOUT      3000000
#define TIMEOUT_INTERVAL            30 // 30*100ms

#define ANALOG_PRESSURE_MASK        0x03FFC000

#define SMALL_MOTOR_STRENGTH        127


typedef enum {
    STATE_IDLE = 0,
    STATE_SEND_ID_AND_LENGTH_GET_CMD,
    STATE_SEND_RESPONSE,
} ControllerState;


struct ds2{

    /*variables for gamepad operation and communication---------------------------------------------------------------------------------*/
    //gamepad state machine
    ControllerState gControllerState;
   /*current cmd is the last byte received from the console*/
    uint8_t received_byte;
    /*current operation is set in the second header byte and is the command received from the console; used to define gamepad behavior*/
    uint8_t current_operation;
    /*response byte to the console*/
    uint8_t response_byte;
    /*buffer for storing analog, button states to reply*/
    volatile uint8_t polling_response[18];
    /*length of the reponse*/
    uint8_t response_length;
    uint8_t response_counter;
    /*gamepad modes; set by the console; default at startup is DIGITAL*/
    uint8_t set_escape_mode;
    uint8_t escape_mode;
    volatile uint8_t gamepad_mode;
    /*temporary value to tell the gamepad which part of the constant value to return (command splits it into 2 parts)*/
    uint8_t constant_value_number;
    /*actuator mapping; configured by command 0x4D*/
    uint8_t actuator_mapping[6];
    uint8_t polling_config[6];
    /*used for enabling/disabling specific response bytes when polling, currently not implemented*/
    uint32_t polling_mask;
    /*indicate whether ACK is low in the IRQ*/
    uint8_t ack_low_flag;
    /*flag to control whether ACK is sent or not*/
    uint8_t allow_ack;
    /*flag to indicate whether the dualshock timeout needs to be (re)started*/
    uint8_t restart_timeout;

    volatile uint16_t start_systick;
};



/*internal functions*/
static void ds2_protocol_handler(void);
static void ds2_execute_command(struct ds2 *ds2_dev);
static void ds2_init_protocol_struct(struct ds2 *ds2_dev);
static void ds2_enable_small_motor(uint8_t value);
static void ds2_enable_large_motor(uint8_t value);
static void ds2_att_handler(uint gpio, uint32_t events);
static void ds2_return_ack_timer(uint32_t delay_us);
static void ds2_ack_alarm_irq(void);

void ds2_poll_inputs(struct ds2 *ds2_dev);
void ds2_initialize(struct ds2 *ds2_dev);
void ds2_deinitialize(struct ds2 *ds2_dev);
void ds2_run_core1_gamepad(void);
void ds2_timeout_irq(void);
void ds2_start_timeout_timer(uint32_t delay_us);
void check_timeout(struct ds2 *ds2_dev);


#endif

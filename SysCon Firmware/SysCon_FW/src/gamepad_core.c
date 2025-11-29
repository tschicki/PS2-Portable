//#pragma GCC optimize (0)
#include "gamepad_core.h"

uint gSpiOffset;
uint gSpiSm;
uint volatile gamepad_reset_flag = 0;

struct ds2 ds2_dev;
extern volatile uint32_t digital_states;
extern volatile uint8_t current_analog_states[16];
extern volatile uint16_t systick;
extern volatile uint8_t motor_left_pwm;
extern volatile uint8_t motor_right_pwm;

static uint8_t response_pressure_config[] = {
    0x00, 0x00, 0x02, 0x00, 0x00, 0x5A
};
//static uint8_t response_pressure_config_digital[] = {
//    0x00, 0x00, 0x00, 0x00, 0x00, 0x5A
//};
//static uint8_t response_pressure_config_dualshock[] = {
//    0xFF, 0xFF, 0x03, 0x00, 0x00, 0x5A
//};
/*used to be 0x00, 0x00, 0x00, 0x00, 0x00, 0x5A*/
static uint8_t response_polling_config_status_digital[] = {
    0xFF, 0xFF, 0x03, 0x00, 0x00, 0x5A
};
static uint8_t response_polling_config_status_dualshock[] = {
    0xFF, 0xFF, 0x03, 0x00, 0x00, 0x5A
};
static uint8_t response_en_analog[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
static uint8_t response_identity[] = {
    TYPE_DUALSHOCK, 0x02, 0x00, 0x02, 0x01, 0x00
};
static uint8_t response_constant_1a[] = {
    0x00, 0x00, 0x01, 0x02, 0x00, 0x0A
};
static uint8_t response_constant_1b[] = {
    0x00, 0x00, 0x01, 0x01, 0x01, 0x14
};
static uint8_t response_constant_2[] = {
    0x00, 0x00, 0x02, 0x00, 0x01, 0x00
};
static uint8_t response_constant_3a[] = {
    0x00, 0x00, 0x00, 0x04, 0x00, 0x00
};
static uint8_t response_constant_3b[] = {
    0x00, 0x00, 0x00, 0x07, 0x00, 0x10
};
static uint8_t response_polling_config[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x5a
};

/*-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
/*internal functions for handling the protocol*/
/*main DS2 protocol handler; runs for every received byte*/
static void ds2_protocol_handler(void)//void __not_in_flash_func (ds2_protocol_handler)(void)
{
    pio_interrupt_clear(pio0, 0);
    //get received command
    ds2_dev.received_byte = pio_spi_read_rx_fifo(pio0, gSpiSm);

    switch (ds2_dev.gControllerState)
    {
    /*when reading the first byte, prepare the answer for the next byte*/
    case STATE_IDLE:
        if (ds2_dev.received_byte == 0x01)
        {
            ds2_dev.allow_ack = 1;
            ds2_dev.gControllerState = STATE_SEND_ID_AND_LENGTH_GET_CMD;
            ds2_dev.response_counter = 0;
            ds2_dev.current_operation = CMD_NO_OPERATION;
            if (ds2_dev.escape_mode == TRUE)
                pio_spi_write_tx_fifo(pio0, gSpiSm, DEVICE_MODE_ESCAPE);
            else
                pio_spi_write_tx_fifo(pio0, gSpiSm, ds2_dev.gamepad_mode);        
        }
        else ds2_dev.allow_ack = 0; /*the other case would be mostly 0x81 -> memory card*/
        break;
    /*store the current operation for the next bytes*/
    case STATE_SEND_ID_AND_LENGTH_GET_CMD:
        ds2_dev.current_operation = ds2_dev.received_byte;
        pio_spi_write_tx_fifo(pio0, gSpiSm, 0x5a);
        ds2_dev.gControllerState = STATE_SEND_RESPONSE; 
        break;
    /*handle every byte coming after the header*/
    case STATE_SEND_RESPONSE:
        
        /*this is not ideal yet, ideally the gamepad should know how many bytes it should return and go back to idle state*/
        /*that however breaks functionality, because sometimes the PS2 and gamepad expect different amounts - and I did not find where that is yet*/
        /*TODO: check whether below could do things to arrays outside their boundaries....*/
        ds2_execute_command(&ds2_dev);
        pio_spi_write_tx_fifo(pio0, gSpiSm, ds2_dev.response_byte);
        if(ds2_dev.response_counter >= ds2_dev.response_length)
            ds2_dev.allow_ack = 0;  //last byte does not get ACKed
        /*keep track of the response byte numbers; IMPORTANT: received byte is always 1 less, as the first byte received is still part of the header!*/
        ds2_dev.response_counter++;
        break;
    }
    /*send the ACK only when the gamepad was addressed; from experience this is necessary when running PS1 games via DKWDRV, they like to send memcard commands on the SPI*/
    if(ds2_dev.allow_ack == 1) ds2_return_ack_timer(1);
    
}
/*find out what the gamepad needs to RETURN for each byte based on the command; fill the return buffer for the rest of the frame*/
static void ds2_execute_command(struct ds2 *ds2_dev)//void __not_in_flash_func(ds2_execute_command)(struct ds2 *ds2_dev)
{

    /*response length depending on gamepad mode*/
    if(ds2_dev->escape_mode == TRUE)
        ds2_dev->response_length = 6;
    else
    {
        if (ds2_dev->gamepad_mode == DEVICE_MODE_DIGITAL) ds2_dev->response_length = 2;
        else if (ds2_dev->gamepad_mode == DEVICE_MODE_ANALOG) ds2_dev->response_length = 6;
        else if (ds2_dev->gamepad_mode == DEVICE_MODE_ANALOG_PRESSURE) ds2_dev->response_length = 18;
    }

    switch (ds2_dev->current_operation)
    {
    case CMD_CONFIG_MODE:
        if(ds2_dev->escape_mode == FALSE) ds2_dev->response_byte = ds2_dev->polling_response[ds2_dev->response_counter];
        else ds2_dev->response_byte = 0x00;

        /*set or reset escape mode*/
        if (ds2_dev->response_counter == 1){
            if (ds2_dev->received_byte == 0x00){
                ds2_dev->set_escape_mode = FALSE;
            }
            else if (ds2_dev->received_byte == 0x01){
                ds2_dev->set_escape_mode = TRUE;
            }
        }
        return;

    case CMD_POLLING:
        ds2_dev->set_escape_mode = FALSE;
        ds2_dev->response_byte = ds2_dev->polling_response[ds2_dev->response_counter];

        if ((ds2_dev->response_counter > 0) && (ds2_dev->response_counter < 7)){
            if (ds2_dev->actuator_mapping[ds2_dev->response_counter - 1] == 0x00){
                if (ds2_dev->received_byte == 0xFF) ds2_enable_small_motor(TRUE);
                else ds2_enable_small_motor(FALSE);
            }
            if (ds2_dev->actuator_mapping[ds2_dev->response_counter - 1] == 0x01){
                if (ds2_dev->received_byte > 0) ds2_enable_large_motor(ds2_dev->received_byte);
                else    ds2_enable_large_motor(FALSE);
            }
        }
        return;

    case CMD_PRESSURE_CONFIG:
        ds2_dev->response_byte = response_pressure_config[ds2_dev->response_counter];
        return;

    case CMD_POLLING_CONFIG_STATUS:
        if(ds2_dev->response_counter == 5){
            ds2_dev->response_byte = 0x5a;
        }
        else{
            if(ds2_dev->gamepad_mode == DEVICE_MODE_DIGITAL)
                ds2_dev->response_byte = 0x00;    /*in digital mode this returns all 0x00*/
            else
                ds2_dev->response_byte = ds2_dev->polling_config[ds2_dev->response_counter]; 
        }  
        return;

    case CMD_EN_ANALOG:
        ds2_dev->response_byte = response_en_analog[ds2_dev->response_counter];
        if (ds2_dev->response_counter == 1){
            if (ds2_dev->received_byte == 0x01){
                ds2_dev->gamepad_mode = DEVICE_MODE_ANALOG;
            }
            else if (ds2_dev->received_byte == 0x00){
                ds2_dev->gamepad_mode = DEVICE_MODE_DIGITAL;
            }
        }
        return;

    case CMD_IDENTITY:
        if(ds2_dev->response_counter == 2){  
            if (ds2_dev->gamepad_mode == DEVICE_MODE_DIGITAL){
                ds2_dev->response_byte = 0x00;   
            }      
            else ds2_dev->response_byte = 0x01;
        }
        else ds2_dev->response_byte = response_identity[ds2_dev->response_counter];
        return;

    case CMD_GET_CONSTANT_1:
        if(ds2_dev->response_counter == 1)      ds2_dev->constant_value_number = ds2_dev->received_byte; 
        //for response 0 it does not matter which state constant_value_number has, as all first bytes of the constant numbers are 0x00
        if(ds2_dev->constant_value_number == 0x00){
            ds2_dev->response_byte = response_constant_1a[ds2_dev->response_counter];
        }
        else if(ds2_dev->constant_value_number == 0x01){
            ds2_dev->response_byte = response_constant_1b[ds2_dev->response_counter];
        }
        return;

    case CMD_GET_CONSTANT_2:
        ds2_dev->response_byte = response_constant_2[ds2_dev->response_counter];
        return;

    case CMD_GET_CONSTANT_3:
        if(ds2_dev->response_counter == 1){
            ds2_dev->constant_value_number = ds2_dev->received_byte; 
        }
        //for response 0 it does not matter which state constant_value_number has, as all first bytes of the constant numbers are 0x00
        if(ds2_dev->constant_value_number == 0x00){
            ds2_dev->response_byte = response_constant_3a[ds2_dev->response_counter];
        }
        else if(ds2_dev->constant_value_number == 0x01){
            ds2_dev->response_byte = response_constant_3b[ds2_dev->response_counter];
        }
        return;

    case CMD_EN_RUMBLE:
        // return previous mapping
        ds2_dev->response_byte = ds2_dev->actuator_mapping[ds2_dev->response_counter];
        //store new mapping
        if((ds2_dev->response_counter > 0) && (ds2_dev->response_counter < 7)){
            ds2_dev->actuator_mapping[ds2_dev->response_counter - 1] = ds2_dev->received_byte;
        }
        return;

    case CMD_POLLING_CONFIG:
        ds2_dev->response_byte = response_polling_config[ds2_dev->response_counter];
        if((ds2_dev->response_counter > 0) && (ds2_dev->response_counter < 7)){
            ds2_dev->polling_config[ds2_dev->response_counter - 1] = ds2_dev->received_byte;
        }
        /*take the polling config and shift it into a mask for polling later*/
        /*curently not used for anything!*/
        if(ds2_dev->response_counter == 5){
            ds2_dev->polling_mask = 0x00000000 | (((uint32_t)(ds2_dev->polling_config[0])) << 24) | (((uint32_t)(ds2_dev->polling_config[1])) << 16) | (((uint32_t)(ds2_dev->polling_config[2] & 0x03)) << 6);
            if((ds2_dev->polling_mask & ANALOG_PRESSURE_MASK) != 0){
                ds2_dev->gamepad_mode = DEVICE_MODE_ANALOG_PRESSURE;
            }
        } 
        return;
    }
}

/*reset the gamepad states and parameters; useful for initializing and resetting the protocol in case of a bad frame (to keep it in sync)*/
static void ds2_init_protocol_struct(struct ds2 *ds2_dev)
{
    ds2_dev->escape_mode = FALSE;
    ds2_dev->set_escape_mode = FALSE;
    ds2_dev->gamepad_mode = DEVICE_MODE_DIGITAL;
    ds2_dev->gControllerState = STATE_IDLE;
    ds2_dev->constant_value_number = 0;
    ds2_dev->ack_low_flag = 0;
    ds2_dev->polling_mask = 0xFFFFC000;
    for (uint8_t i = 0; i < 6; i++){
        ds2_dev->actuator_mapping[i] = 0xFF;
        ds2_dev->polling_config[i] = response_polling_config_status_digital[i];
    }
    
    ds2_dev->start_systick = systick;
}

/*----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
/*high level functions to operate the gamepad*/
/*main function for getting all gamepad input data; shall be run by core1*/
void ds2_poll_inputs(struct ds2 *ds2_dev)
{
    /*TODO:this is still lacking the implementation to apply the polling config set by the PS2; is it really needed for compatibility?*/
    //write the inputs into the button polling response buffer
    ds2_dev->polling_response[0] = (uint8_t)digital_states;
    ds2_dev->polling_response[1] = (uint8_t)(digital_states >> 8);
    /*return values of analog sticks and pressure sensors*/
    for (uint8_t i = 0; i < 16; i++)
    {
        ds2_dev->polling_response[i+2] = current_analog_states[i];
    }

}

static void ds2_enable_small_motor(uint8_t value)
{
    if(value > 0)
        motor_left_pwm = SMALL_MOTOR_STRENGTH;
    else
        motor_left_pwm = 0;
}

static void ds2_enable_large_motor(uint8_t value)
{
    motor_right_pwm = value;
}

void ds2_deinitialize(struct ds2 *ds2_dev)
{
    /*disable IRQ first*/
    gpio_set_irq_enabled_with_callback(GAMEPAD_ATT_PIN, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, false, &ds2_att_handler);
    irq_set_enabled(PIO0_IRQ_0, false);

    /*disable the PIO state machine, place DAT pin as input, unclaim and remove the program from PIO*/
    pio_spi_deinit(pio0, gSpiSm, GAMEPAD_DATA_PIN);
    pio_remove_program_and_unclaim_sm(&gamepad_spi_program, pio0, gSpiSm, gSpiOffset);

    /*let core0 know that deinitialization is complete*/
    gamepad_reset_flag = 0;
    
}
/*initialize the PIO, interrupts and DS2 struct, done once at startup by core1*/
void ds2_initialize(struct ds2 *ds2_dev)
{

    /*setup interrupt for ATT rising and falling edge detection*/
    gpio_set_irq_enabled_with_callback(GAMEPAD_ATT_PIN, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &ds2_att_handler);

    /*all the PIO initialization*/
    gSpiOffset = pio_add_program(pio0, &gamepad_spi_program);
    gSpiSm = pio_claim_unused_sm(pio0, true);
    pio_spi_init(pio0, gSpiSm, gSpiOffset, GAMEPAD_CMD_PIN, GAMEPAD_SCK_PIN, GAMEPAD_DATA_PIN);
    pio_set_irq0_source_enabled(pio0, pis_interrupt0, true);
    irq_set_exclusive_handler(PIO0_IRQ_0, ds2_protocol_handler);
    irq_set_enabled(PIO0_IRQ_0, true);

    /*init all variables for running the dualshock 2 emulation*/
    ds2_init_protocol_struct(ds2_dev);  
}

/*main loop for core1 to handle the DS2 protocol*/
void ds2_run_core1_gamepad(void)
{   
    /*initialize all DS2 protocol parameters*/
    ds2_initialize(&ds2_dev);
    /*main loop of DS2 gamepad handler; runs on core1*/
    while (1)
    {
        ds2_poll_inputs(&ds2_dev);
        check_timeout(&ds2_dev);
        if(gamepad_reset_flag == 1)
        {
            /*stay here until core1 is reset*/
            ds2_deinitialize(&ds2_dev);
            while(1);
        }
            
    }
}

static void ds2_att_handler(uint gpio, uint32_t events)
{
    if ((gpio == GAMEPAD_ATT_PIN) && (events == GPIO_IRQ_EDGE_FALL))
    {
        /*start of communication*/
        pio_spi_read_rx_fifo(pio0, gSpiSm);
        pio_spi_write_tx_fifo(pio0, gSpiSm, 0xFF);
        ds2_dev.gControllerState = STATE_IDLE;
        ds2_dev.escape_mode = ds2_dev.set_escape_mode;
    }
    else if ((gpio == GAMEPAD_ATT_PIN) && (events == GPIO_IRQ_EDGE_RISE))
    {
        /*end of communication*/
        ds2_dev.gControllerState = STATE_IDLE;
        /*reset the PIO, just to be safe*/
        pio_sm_clear_fifos (pio0, gSpiSm);
        pio_sm_exec(pio0, gSpiSm, pio_encode_jmp(gSpiOffset));

        ds2_dev.start_systick = systick;
    }
}

static void ds2_return_ack_timer(uint32_t delay_us)
{
    /*setup the interrupt and irq handler for the ACK pin*/
    hw_set_bits(&timer_hw->inte, 1u << ALARM_ACK);
    irq_set_exclusive_handler(ALARM_IRQ_ACK, ds2_ack_alarm_irq);
    /*try to prioritize the timer IRQ over the ATT IRQ to get more consistent ACK lengths*/
    irq_set_priority(ALARM_IRQ_ACK, 0x7F);
    /*Enable the alarm irq*/
    irq_set_enabled(ALARM_IRQ_ACK, true);
    /*Alarm is only 32 bits!!!!*/
    uint64_t target = timer_hw->timerawl + delay_us;
    /*Write the lower 32 bits of the target time to the alarm which will arm it*/
    timer_hw->alarm[ALARM_ACK] = (uint32_t)target;
}

static void ds2_ack_alarm_irq(void)
{
    // Clear the alarm irq
    hw_clear_bits(&timer_hw->intr, 1u << ALARM_ACK);
    // Assume alarm 0 has fired
    
    if (ds2_dev.ack_low_flag == 0)
    { 
        gpio_put(GAMEPAD_ACK_PIN, SYSPIN_LOW);
        gpio_set_dir(GAMEPAD_ACK_PIN, GPIO_OUT);
        ds2_dev.ack_low_flag = 1;
        ds2_return_ack_timer(1);
    }
    else if (ds2_dev.ack_low_flag == 1)
    {
        gpio_set_dir(GAMEPAD_ACK_PIN, GPIO_IN);
        irq_set_enabled(ALARM_IRQ_ACK, false);
        ds2_dev.ack_low_flag = 0;
    }
    
}

void check_timeout(struct ds2 *ds2_dev)
{
    uint16_t elapsed_ticks = 0;

    /*handle overflow in tick counter and set the elapsed ticks accordingly*/
    if((systick < ds2_dev->start_systick)) 
        elapsed_ticks = (65536 - ds2_dev->start_systick) + systick;
    else 
        elapsed_ticks = systick - ds2_dev->start_systick;

    /*after the specified timeout we reset the gamepad mode; looks like this is needed for PS1 games, as some may not like analog or dualshock mode at startup*/
    /*the original controller does this too, although the exact timeout is unknown (probably between 1s-10s)*/
    if(elapsed_ticks > TIMEOUT_INTERVAL){
        /*we reset the gamepad mode to digital when there is no ongoing transfer*/
        if(ds2_dev->gControllerState == STATE_IDLE) ds2_dev->gamepad_mode = DEVICE_MODE_DIGITAL;
    }

}
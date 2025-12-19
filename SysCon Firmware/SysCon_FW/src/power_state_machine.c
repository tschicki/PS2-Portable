#include "power_state_machine.h"

/*flag to stop the gamepad emulator; set to 1 to stop and wait until 0*/
extern volatile uint gamepad_reset_flag;
/*flash variables offset in flash, must align with 4k sectors*/
const uint8_t *flash_target_contents = (const uint8_t *)(XIP_BASE + FLASH_TARGET_OFFSET);
/*used to temporarily store and disable interrupts for flash functions*/
uint32_t irq_status = 0;

void run_power_state_machine(struct power_dev *power_dev)
{
    /*in case an error happened in the power polling function, shut down*/
    if (power_dev->power_error_flag == 1)
        power_dev->cur_run_state = syscon_state_str(STATE_RUN_ERROR, power_dev->cur_run_state);

    switch (power_dev->cur_run_state)
    {
    case STATE_INIT:
        power_dev->cur_run_state = syscon_state_str(run_state_init(power_dev), power_dev->cur_run_state);
        /*only continue when communication with MAX is possible -> checked in run_state_init() function*/
        if (power_dev->cur_run_state != STATE_RUN_ERROR)
        {
            /*charging state machine should only start when everything was initialized*/
            power_dev->cur_chg_state = syscon_state_str(STATE_WAIT_FOR_STATUS, power_dev->cur_chg_state);
            /*starts the timer to launch the power management check continuosly*/
            start_power_poll_timer(POWER_POLL_INTERVAL_US);
        }
        break;

    case STATE_WAIT_USER:
        power_dev->next_run_state = run_state_wait_user(&power_dev->io);
        if (power_dev->next_run_state != STATE_WAIT_USER)
        {
            power_dev->cur_run_state = syscon_state_str(power_dev->next_run_state, power_dev->cur_run_state);
            power_dev->powerLED.led_parameters[PATTERN_SOC].enable = 1;
        }
        break;

    case STATE_BOOT:
        power_dev->next_run_state = run_state_boot(power_dev);
        if (power_dev->next_run_state != STATE_BOOT)
        {
            power_dev->cur_run_state = syscon_state_str(power_dev->next_run_state, power_dev->cur_run_state);
        }
        break;

    case STATE_RUN:
        power_dev->next_run_state = run_state_run(&power_dev->io);
        if (power_dev->next_run_state != STATE_RUN)
        {
            power_dev->last_run_state = power_dev->cur_run_state;
            power_dev->cur_run_state = syscon_state_str(power_dev->next_run_state, power_dev->cur_run_state);
        }
        break;

    case STATE_CONFIG:
        power_dev->next_run_state = run_state_config(power_dev);
        if (power_dev->next_run_state != STATE_CONFIG)
        {
            power_dev->cur_run_state = syscon_state_str(power_dev->next_run_state, power_dev->cur_run_state);
        }
        break;

    case STATE_SHUTDOWN:
        power_dev->cur_run_state = syscon_state_str(run_state_shutdown(&power_dev->io, &power_dev->amp_dev, &power_dev->gp_dev), power_dev->cur_run_state);
        break;

    case STATE_LOAD_SETTINGS:
        power_dev->cur_run_state = syscon_state_str(run_state_load_settings(power_dev), power_dev->cur_run_state);
        break;

    case STATE_STORE_SETTINGS:
        power_dev->cur_run_state = syscon_state_str(run_state_store_settings(power_dev), power_dev->cur_run_state);
        break;

    case STATE_OFF:
        power_dev->cur_run_state = syscon_state_str(run_state_off(power_dev), power_dev->cur_run_state);
        break;

    case STATE_WRITE_CONFIG:
        manufacturing_config(power_dev);
        handle_led_patterns(&power_dev->powerLED, &power_dev->gp_dev.extio_dev[GP_RIGHT], systick);
        power_dev->cur_run_state = syscon_state_str(STATE_RUN_HALT, power_dev->cur_run_state);
        break;
    
    case STATE_RUN_HALT:
        /*do nothing*/
        break;

    case STATE_RUN_ERROR:
        /*not defined yet*/
        power_dev->cur_run_state = syscon_state_str(STATE_OFF, power_dev->cur_run_state);
        break;

    default:
        syscon_error_str(ENTERED_UNDEFINED_STATE);
        power_dev->cur_run_state = syscon_state_str(STATE_RUN_ERROR, power_dev->cur_run_state);
        break;
    }
}

syscon_state_t run_state_init(struct power_dev *power_dev)
{
    uint8_t retry = 0;

    spi_interface_init(FPGA_SPI, FPGA_TX_PIN, FPGA_RX_PIN, FPGA_SCK_PIN, FPGA_SPI_BAUDRATE);
    i2c_interface_init(PRW_I2C, PWR_I2C_SDA_PIN, PWR_I2C_SCL_PIN, PWR_I2C_BAUD_RATE);
    power_dev->BQ_Dev.interface = PRW_I2C;
    power_dev->STUSB_Dev.interface = PRW_I2C;
    power_dev->MAX_Dev.interface = PRW_I2C;

    led_patterns_init(&power_dev->powerLED);
    gamepad_interface_init(&power_dev->gp_dev);
    fan_init();
    menu_fill_struct(&power_dev->menu_dev);

    /*initialize battery mnagement*/
    BQ_init_structs(&power_dev->BQ_Dev);

    #ifndef DEBUG_IGNORE_MAX
    /*try to cummunicate with MAX - first time powering on or when battery was dead this will trigger*/
    do
    {
        retry++;
        if (MAX_get_status(&power_dev->MAX_Dev) != ERROR_OK)
        {
            printf("ERROR communicating with protector! - TRY #%u\n", retry);
            sys_set_pin(power_dev->io.pin[PIN_CHARGE_EN], SYSPIN_LOW);
            delay_ms(500);
            sys_set_pin(power_dev->io.pin[PIN_CHARGE_EN], SYSPIN_HIZ);
        }
        else
            break;
    } while (retry < 3);

    if (retry == 3)
        return STATE_RUN_ERROR;
    
    
    /*set config only if current config was lost -> battery removed
    possible registers to indicate: IOTG*/
    if (BQ_check_config_present(&power_dev->BQ_Dev) == BQ_CONFIG_MISMATCH)
    {
        printf("UPDATING MAX CONFIG\n");
        /*only needed if NVM was not flashed*/
        // if (MAX_update_nvconfig(&power_dev->MAX_Dev) != ERROR_OK)
        //     return STATE_SHUTDOWN;

        printf("UPDATING BQ CONFIG\n");
        if (BQ_set_config(&power_dev->BQ_Dev) != ERROR_OK)
            return STATE_SHUTDOWN;
    }
    #endif
    if (BQ_write_ADC_config(&power_dev->BQ_Dev) != ERROR_OK)
        return STATE_SHUTDOWN;
    if (BQ_ADC_EnDi(&power_dev->BQ_Dev, ADC_ENABLE) != ERROR_OK)
        return STATE_SHUTDOWN;

    // debug - dump MAX settings

    return STATE_LOAD_SETTINGS;
}

syscon_state_t run_state_wait_user(struct SysCon_Pins *io)
{
    /*was the power button pressed?*/
    if (sys_get_pin(io->pin[PIN_POWERBTN]) == 0)
    {
        delay_ms(1000);

        if (sys_get_pin(io->pin[PIN_POWERBTN]) == 0)
        {
#if defined(DEBUG_WRITE_STUSB_NVM) || defined(DEBUG_FLASH_MAX_NVM)
            /*if this was defined, stop here and got to NVM flashing*/
            return STATE_WRITE_CONFIG;
#else
            /*else boot normally*/
            sys_set_pin(io->pin[PIN_SYS_FORCE_ON], SYSPIN_HIGH);
            return STATE_BOOT;
#endif
        }
    }
    return STATE_WAIT_USER;
}

syscon_state_t run_state_boot(struct power_dev *power_dev)
{
    syscon_state_t result = STATE_RUN_ERROR;
    /*enable the DCDC converters and the falling edge interrupt on PG*/
    sys_set_pin(power_dev->io.pin[PIN_DCDC_EN], SYSPIN_HIGH);
    endi_power_good_irq(&power_dev->io, ENABLE);
    /*a bit of delay to let the voltages stabilize*/
    delay_ms(1500);

    if (sys_get_pin(power_dev->io.pin[PIN_PS2_LED_RED]))
    {
        /*if the PS2 red led is on, press the power button*/
        sys_set_pin(power_dev->io.pin[PIN_PS2_RESET], SYSPIN_LOW);

        for (uint8_t retry = 0; retry < 100; retry++)
        {
            if (sys_get_pin(power_dev->io.pin[PIN_PS2_LED_GREEN]))
            {
                /*wait until the LED turns green, if it takes too long there is something wrong*/
                sys_set_pin(power_dev->io.pin[PIN_PS2_RESET], SYSPIN_HIZ);
                result = STATE_RUN;
            }
            else
                result = STATE_RUN_ERROR;

            delay_ms(10);
        }
    }

    if (result == STATE_RUN)
    {
        /*enable the fan at the default PWM value*/
        fan_calculate_constants(&power_dev->fan_dev);
        fan_endi(ENABLE);

        /*start gamepad emulator*/
        multicore_launch_core1(ds2_run_core1_gamepad);

        /*load bitstream to video processor and configure fpga, onla makes sense when the PS2 booted*/
        if (fpga_init(&power_dev->menu_dev.fpga_dev, &power_dev->io) != ERROR_OK)
            result = STATE_RUN_ERROR;

        /*initialize LM49450 and start button polling timer, must be in this sequence and after poweron*/
        amp_init(&power_dev->amp_dev, power_dev->settings);
        start_button_poll_timer(BUTTON_POLL_INTERVAL_US);
    }
    else
    {
        /*in case the PS2 failed to boot, unpress the reset*/
        sys_set_pin(power_dev->io.pin[PIN_PS2_RESET], SYSPIN_HIZ);
    }
    return result;
}

syscon_state_t run_state_run(struct SysCon_Pins *io)
{

    if (sys_get_pin(io->pin[PIN_PS2_LED_RED]))
        return STATE_SHUTDOWN;

    else if ((sys_get_pin(io->pin[PIN_POWERBTN]) == 0) && sys_get_pin(io->pin[PIN_PS2_LED_GREEN]))
    {
        delay_ms(1000);
        if (sys_get_pin(io->pin[PIN_POWERBTN]) == 0)
        {
            return STATE_SHUTDOWN;
        }
        else
            return STATE_CONFIG;
    }

    return STATE_RUN;
}

syscon_state_t run_state_config(struct power_dev *power_dev)
{
    if (power_dev->menu_dev.menu_active == 0)
    {
        /*do this only once when launching the menu*/
        menu_init(power_dev);
        // power_dev->powerLED.led_parameters[PATTERN_CONFIG].enable = 1;
        power_dev->menu_dev.menu_active = 1;
    }
    /*main function to execute menu*/
    menu_run(power_dev, systick);

    if ((sys_get_pin(power_dev->io.pin[PIN_POWERBTN]) == 0) || (sys_get_pin(power_dev->io.pin[PIN_PS2_LED_RED])))
    {
        /*cleanup menu and return to previous state*/
        power_dev->menu_dev.menu_active = 0;
        menu_cleanup(&power_dev->menu_dev, &power_dev->io);
        // power_dev->powerLED.led_parameters[PATTERN_CONFIG].enable = 0;
        while (sys_get_pin(power_dev->io.pin[PIN_POWERBTN]) == 0)
            ;
        return power_dev->last_run_state;
    }
    else
        return STATE_CONFIG;
}

syscon_state_t run_state_shutdown(struct SysCon_Pins *io, struct speaker_amp *amp_dev, struct gamepad_user *gp_dev)
{
    if (sys_get_pin(io->pin[PIN_PS2_LED_GREEN]))
    {
        amp_mute(amp_dev, 1);
        gp_dev->rumble_en = 0;
        sys_set_pin(io->pin[PIN_PS2_RESET], SYSPIN_LOW);
        delay_ms(2000);
        sys_set_pin(io->pin[PIN_PS2_RESET], SYSPIN_HIZ);

        for (uint8_t retry = 0; retry < 100; retry++)
        {
            /*in case the PS2 locks up, only wait a limited time for it to shutdown*/
            delay_ms(50);
            if (sys_get_pin(io->pin[PIN_PS2_LED_RED]))
                return STATE_STORE_SETTINGS;
        }
    }
    return STATE_STORE_SETTINGS;
}

syscon_state_t run_state_load_settings(struct power_dev *power_dev)
{
    irq_status = save_and_disable_interrupts();
    /*loading from flash should happen before the core1 is started*/
    flash_init(&power_dev->settings, SETTINGS_COUNT, flash_target_contents, MAGIC_NUMBER);
    restore_interrupts(irq_status);
    /*load all gamepad parameters from flash and init the respective structs*/
    gamepad_user_init_remote_io_structs(&power_dev->gp_dev, power_dev->settings);
    /*load the video configs from flash to the array in fpga_dev*/
    fpga_copy_video_cfg(&power_dev->menu_dev.fpga_dev, power_dev->settings, RES_LOAD);
    power_dev->amp_dev.headphone_volume = power_dev->settings[NO_HEADPHONE_VOLUME];
    power_dev->amp_dev.speaker_volume = power_dev->settings[NO_SPEAKER_VOLUME];
    power_dev->menu_dev.fpga_dev.brightness = (uint8_t)power_dev->settings[NO_DISP_BRIGHTNESS];
    power_dev->fan_dev.fan_low_temp = power_dev->settings[NO_FAN_TEMP_LOW];
    power_dev->fan_dev.fan_high_temp = power_dev->settings[NO_FAN_TEMP_HIGH];

    power_dev->gp_dev.rumble_intensity = power_dev->settings[NO_RUMBLE_INTENSITY];
    power_dev->menu_dev.fpga_dev.magh_autodetect_enable = power_dev->settings[NO_MAGH_AUTODETECT];
    power_dev->gp_dev.rumble_en = 1;
    power_dev->store_settings_flag = 0;

    return STATE_WAIT_USER;
}

syscon_state_t run_state_store_settings(struct power_dev *power_dev)
{
    /*request a stop of the gamepad emulator and wait until core1 resets the flag*/
    gamepad_reset_flag = 1;
    while (gamepad_reset_flag == 1)
        ;
    multicore_reset_core1();

    irq_status = save_and_disable_interrupts();
    /*writing to flash must happen after core1 was stopped*/
    if (power_dev->store_settings_flag == 1)
        flash_store(power_dev->settings, flash_target_contents, SETTINGS_COUNT);
    restore_interrupts(irq_status);

    power_dev->store_settings_flag = 0;
    return STATE_OFF;
}

syscon_state_t run_state_off(struct power_dev *power_dev)
{
    if (power_dev->cur_chg_state == STATE_GET_POWER_STATUS)
    {
        /*only shut down power management supervising, if no charger is plugged */
        stop_power_poll_timer();
        BQ_ADC_EnDi(&power_dev->BQ_Dev, ADC_DISABLE);
        MAX_enter_ship_mode(&power_dev->MAX_Dev);
    }

    /*disable the fan PWM output*/
    fan_endi(DISABLE);
    /*disable PG IRQ, turn on all voltage rails and turn off the syscon after a little delay*/
    endi_power_good_irq(&power_dev->io, DISABLE);
    stop_button_poll_timer();
    sys_set_pin(power_dev->io.pin[PIN_DCDC_EN], SYSPIN_LOW);
    delay_ms(500);
    sys_set_pin(power_dev->io.pin[PIN_SYS_FORCE_ON], SYSPIN_LOW);

    return STATE_LOAD_SETTINGS;
}

syscon_error_t manufacturing_config(struct power_dev *power_dev)
{
    power_dev->powerLED.led_parameters[PATTERN_ERROR].enable = 1;

    if (power_dev->BQ_Dev.BQ_CHGStatus.vbus_present_stat == 1)
    {
        #ifdef DEBUG_WRITE_STUSB_NVM
        STUSB_nvm_flash(&power_dev->STUSB_Dev); // TODO: implement returns!
        //if (STUSB_re_negotiate(&power_dev->STUSB_Dev) != ERROR_OK)
        //    return ERROR_FLASH_CONFIGS;
        sys_set_pin(power_dev->io.pin[PIN_DCDC_EN], SYSPIN_HIGH); /*enable the DC-DC converters for voltage measurement*/
        #endif
        
        #ifdef DEBUG_FLASH_MAX_NVM
        if (MAX_write_nvconfig(&power_dev->MAX_Dev) != ERROR_OK)
            return ERROR_FLASH_CONFIGS;
        #endif

        power_dev->powerLED.led_parameters[PATTERN_ERROR].enable = 0;
        power_dev->powerLED.led_parameters[PATTERN_SUCCESS].enable = 1;
        return ERROR_OK;
    }
    else
    {
        return ERROR_FLASH_CONFIGS;
    }
}

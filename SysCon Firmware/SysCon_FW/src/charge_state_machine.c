#include "charge_state_machine.h"

void run_charge_state_machine(struct power_dev *power_dev)
{

    switch (power_dev->cur_chg_state)
    {

    case STATE_CHARGE_DEFAULT:
        /*do nothing*/
        break;

    case STATE_WAIT_FOR_STATUS:
        /*the power_status_known flag is set by the power state machine, once everything was initialized*/
        if (power_dev->power_status_known == 1)
        {
            #ifndef BLOCK_CHARGE
            power_dev->cur_chg_state = syscon_state_str(STATE_GET_POWER_STATUS, power_dev->cur_chg_state);
            #endif
        }
        break;

    case STATE_GET_POWER_STATUS:
        power_dev->next_chg_state = run_state_get_power_status(&power_dev->BQ_Dev);
        if (power_dev->next_chg_state != STATE_GET_POWER_STATUS)
        {
            power_dev->cur_chg_state = syscon_state_str(power_dev->next_chg_state, power_dev->cur_chg_state);
        }
        break;

    case STATE_CHARGE_INIT:
        power_dev->cur_chg_state = syscon_state_str(run_state_charge_init(&power_dev->STUSB_Dev, &power_dev->BQ_Dev, &power_dev->io), power_dev->cur_chg_state);
        break;

    case STATE_CHARGE:
        power_dev->next_chg_state = get_charger_status(&power_dev->BQ_Dev);
        power_dev->powerLED.led_parameters[PATTERN_SOC].enable = 1;
        if (power_dev->next_chg_state != STATE_CHARGE)
        {
            power_dev->cur_chg_state = syscon_state_str(power_dev->next_chg_state, power_dev->cur_chg_state);
        }
        break;

    case STATE_CHARGE_ERROR:
        /*not yet defined*/
        sys_set_pin(power_dev->io.pin[PIN_CHARGE_EN], SYSPIN_HIZ);
        /*dump all the MAX registers via UART*/
        MAX_dump_settings_uart(&power_dev->MAX_Dev);
        /*dump all the BQ registers via UART*/
        if (power_dev->powerLED.led_parameters[PATTERN_ERROR].enable == 0)
        {
            printf("----------------------------------------------\r\n");
            printf("ADC READINGS:\r\n");
            if (power_dev->BQ_Dev.BQ_ADC_Config.ibus_adc_dis == ADC_FUNC_ENABLE)
                printf("IBUS: %dmA\r\n", power_dev->BQ_Dev.BQ_ADC_Readings.ibus_adc);
            if ((power_dev->BQ_Dev.BQ_ADC_Config.ibat_adc_dis == ADC_FUNC_ENABLE) && (power_dev->BQ_Dev.BQ_ADC_Config.en_ibat == ADC_FUNC_ENABLE))
                printf("IBAT: %dmA\r\n", power_dev->BQ_Dev.BQ_ADC_Readings.ibat_adc);
            if (power_dev->BQ_Dev.BQ_ADC_Config.vbus_adc_dis == ADC_FUNC_ENABLE)
                printf("VBUS: %umV\r\n", power_dev->BQ_Dev.BQ_ADC_Readings.vbus_adc);
            if (power_dev->BQ_Dev.BQ_ADC_Config.vbat_adc_dis == ADC_FUNC_ENABLE)
                printf("VBAT: %umV\r\n", power_dev->BQ_Dev.BQ_ADC_Readings.vbat_adc);
            if (power_dev->BQ_Dev.BQ_ADC_Config.vsys_adc_dis == ADC_FUNC_ENABLE)
                printf("VSYS: %umV\r\n", power_dev->BQ_Dev.BQ_ADC_Readings.vsys_adc);
            if (power_dev->BQ_Dev.BQ_ADC_Config.ts_adc_dis == ADC_FUNC_ENABLE)
                printf("TempSens: %f%%\r\n", (float)(power_dev->BQ_Dev.BQ_ADC_Readings.ts_adc) * 0.0976563f);
            if (power_dev->BQ_Dev.BQ_ADC_Config.tdie_adc_dis == ADC_FUNC_ENABLE)
                printf("DieTemp: %f°C\r\n", (float)(power_dev->BQ_Dev.BQ_ADC_Readings.tdie_adc * 0.5));
            if (power_dev->BQ_Dev.BQ_ADC_Config.dp_adc_dis == ADC_FUNC_ENABLE)
                printf("D+: %umV\r\n", power_dev->BQ_Dev.BQ_ADC_Readings.dp_adc);
            if (power_dev->BQ_Dev.BQ_ADC_Config.dm_adc_dis == ADC_FUNC_ENABLE)
                printf("D-: %umV\r\n", power_dev->BQ_Dev.BQ_ADC_Readings.dm_adc);
            if (power_dev->BQ_Dev.BQ_ADC_Config.vac2_adc_dis == ADC_FUNC_ENABLE)
                printf("VAC2: %umV\r\n", power_dev->BQ_Dev.BQ_ADC_Readings.vac2_adc);
            if (power_dev->BQ_Dev.BQ_ADC_Config.vac1_adc_dis == ADC_FUNC_ENABLE)
                printf("VAC1: %umV\r\n", power_dev->BQ_Dev.BQ_ADC_Readings.vac1_adc);
            printf("----------------------------------------------\r\n");
            printf("CHG Status:\r\n");
            printf("IINDPM Status: %x\r\n", power_dev->BQ_Dev.BQ_CHGStatus.iindpm_stat);
            printf("VINDPM Status: %x\r\n", power_dev->BQ_Dev.BQ_CHGStatus.vindpm_stat);
            printf("Watchdog Status: %x\r\n", power_dev->BQ_Dev.BQ_CHGStatus.wd_stat);
            printf("Poor Source Status: %x\r\n", power_dev->BQ_Dev.BQ_CHGStatus.poorsrc_stat);
            printf("Power Good Status: %x\r\n", power_dev->BQ_Dev.BQ_CHGStatus.pg_stat);
            printf("VAC2 Insert Status: %x\r\n", power_dev->BQ_Dev.BQ_CHGStatus.ac2_present_stat);
            printf("VAC1 Insert Status: %x\r\n", power_dev->BQ_Dev.BQ_CHGStatus.ac1_present_stat);
            printf("VBUS Present Status: %x\r\n", power_dev->BQ_Dev.BQ_CHGStatus.vbus_present_stat);
            printf("Charge status bits: %x\r\n", power_dev->BQ_Dev.BQ_CHGStatus.chg_stat);
            printf("VBUS status bits: %x\r\n", power_dev->BQ_Dev.BQ_CHGStatus.vbus_stat);
            printf("BC1.2 status bit: %x\r\n", power_dev->BQ_Dev.BQ_CHGStatus.bc12_done_stat);
            printf("ICO Status: %x\r\n", power_dev->BQ_Dev.BQ_CHGStatus.ico_stat);
            printf("Thermal Regulation Status: %x\r\n", power_dev->BQ_Dev.BQ_CHGStatus.treg_stat);
            printf("D+/D- Detection Status: %x\r\n", power_dev->BQ_Dev.BQ_CHGStatus.dpdm_stat);
            printf("Battery Present Status: %x\r\n", power_dev->BQ_Dev.BQ_CHGStatus.vbat_present_stat);
            printf("ACFET 2 Status: %x\r\n", power_dev->BQ_Dev.BQ_CHGStatus.acrb2_stat);
            printf("ACFET 1 Status: %x\r\n", power_dev->BQ_Dev.BQ_CHGStatus.acrb1_stat);
            printf("ADC Conversion Status: %x\r\n", power_dev->BQ_Dev.BQ_CHGStatus.adc_done_stat);
            printf("VSYS Regulation Status: %x\r\n", power_dev->BQ_Dev.BQ_CHGStatus.vsys_stat);
            printf("Fast charge timer Status: %x\r\n", power_dev->BQ_Dev.BQ_CHGStatus.chg_tmr_stat);
            printf("Trickle charge timer Status: %x\r\n", power_dev->BQ_Dev.BQ_CHGStatus.trichg_tmr_stat);
            printf("Pre-charge timer Status: %x\r\n", power_dev->BQ_Dev.BQ_CHGStatus.prechg_tmr_stat);
            printf("VBAT too low for OTG: %x\r\n", power_dev->BQ_Dev.BQ_CHGStatus.vbatotg_low_stat);
            printf("Bat Temp is cold: %x\r\n", power_dev->BQ_Dev.BQ_CHGStatus.ts_cold_stat);
            printf("Bat Temp is cool: %x\r\n", power_dev->BQ_Dev.BQ_CHGStatus.ts_cool_stat);
            printf("Bat Temp is warm: %x\r\n", power_dev->BQ_Dev.BQ_CHGStatus.ts_warm_stat);
            printf("Bat Temp is hot: %x\r\n", power_dev->BQ_Dev.BQ_CHGStatus.ts_hot_stat);
            printf("----------------------------------------------\r\n");
            printf("Fault Flags:\r\n");
            printf("ibat_reg_flag: %x\r\n", power_dev->BQ_Dev.BQ_FaultFlags.ibat_reg_flag);
            printf("vbus_ovp_flag: %x\r\n", power_dev->BQ_Dev.BQ_FaultFlags.vbus_ovp_flag);
            printf("vbat_ovp_flag: %x\r\n", power_dev->BQ_Dev.BQ_FaultFlags.vbat_ovp_flag);
            printf("ibus_ocp_flag: %x\r\n", power_dev->BQ_Dev.BQ_FaultFlags.ibus_ocp_flag);
            printf("ibat_ocp_flag: %x\r\n", power_dev->BQ_Dev.BQ_FaultFlags.ibat_ocp_flag);
            printf("conv_ocp_flag: %x\r\n", power_dev->BQ_Dev.BQ_FaultFlags.conv_ocp_flag);
            printf("vac2_ovp_flag: %x\r\n", power_dev->BQ_Dev.BQ_FaultFlags.vac2_ovp_flag);
            printf("vac1_ovp_flag: %x\r\n", power_dev->BQ_Dev.BQ_FaultFlags.vac1_ovp_flag);
            printf("vsys_short_flag: %x\r\n", power_dev->BQ_Dev.BQ_FaultFlags.vsys_short_flag);
            printf("vsys_ovp_flag: %x\r\n", power_dev->BQ_Dev.BQ_FaultFlags.vsys_ovp_flag);
            printf("otg_ovp_flag: %x\r\n", power_dev->BQ_Dev.BQ_FaultFlags.otg_ovp_flag);
            printf("otg_uvp_flag: %x\r\n", power_dev->BQ_Dev.BQ_FaultFlags.otg_uvp_flag);
            printf("tshut_flag: %x\r\n", power_dev->BQ_Dev.BQ_FaultFlags.tshut_flag);
            printf("----------------------------------------------\r\n");
        }
        power_dev->powerLED.led_parameters[PATTERN_ERROR].enable = 1;
        break;

    default:
        syscon_error_str(ENTERED_UNDEFINED_STATE);
        power_dev->cur_chg_state = syscon_state_str(STATE_CHARGE_ERROR, power_dev->cur_chg_state);
    }
}

syscon_state_t run_state_get_power_status(struct BQ25792_Dev *BQ_Dev)
{

    if ((BQ_Dev->BQ_CHGStatus.vbus_present_stat == 1) && (BQ_Dev->BQ_CHGStatus.vbat_present_stat == 1))
        return STATE_CHARGE_INIT;

    return STATE_GET_POWER_STATUS;
}

syscon_state_t run_state_charge_init(struct STUSB4500_Dev *STUSB_Dev, struct BQ25792_Dev *BQ_Dev, struct SysCon_Pins *io)
{
    /*tell the STUSB which PD settings we want*/
    /*is this really needed when all settings are in NVM already?*/
    // if (STUSB_init(STUSB_Dev) != ERROR_OK)
    //     return STATE_CHARGE_ERROR;
    /*give the STUSB some time to renegotiate if needed*/
    /*actually this delay is needed, otherwise the portable will not charge*/
    delay_ms(1000);
    /*first get the USB C current limit*/
    if (get_input_current_limit(STUSB_Dev) != ERROR_OK)
        return STATE_CHARGE_ERROR;
    /*update the input current limit accordingly*/
    if (BQ_set_input_current_limit(BQ_Dev, STUSB_Dev->RDO_status.operating_current_mA) != ERROR_OK)
        return STATE_CHARGE_ERROR;
    /*enable the BQ ADC*/
    if (BQ_CHG_EnDi(BQ_Dev, CHG_ENABLE) != ERROR_OK)
        return STATE_CHARGE_ERROR;

    /*enable charging by setting this pin LOW*/
    sys_set_pin(io->pin[PIN_CHARGE_EN], SYSPIN_LOW);

    /*to make sure the charger status is up to date for the next state*/
    delay_us(POWER_POLL_INTERVAL_US);

    return STATE_CHARGE;
}

syscon_state_t get_charger_status(struct BQ25792_Dev *BQ_Dev)
{
    /*the status is sampled by the power polling function, which is run by a timer*/
    switch (BQ_Dev->BQ_CHGStatus.chg_stat)
    {
    case BQ_CHG_CHARGE_DONE:
        return STATE_CHARGE;
    case BQ_CHG_NOT_CHARGING:
        if (BQ_Dev->BQ_CHGStatus.vbus_present_stat == 0)
            return STATE_GET_POWER_STATUS;
    case BQ_CHG_RESERVED:
        printf("CHG_STAT ERROR: %u", BQ_Dev->BQ_CHGStatus.chg_stat);
        return STATE_CHARGE_ERROR;
    default:
        return STATE_CHARGE;
    }
}
#include "MAX17320.h"

syscon_error_t MAX_update_nvconfig(struct MAX17320_Dev *MAX_Dev)
{
    static uint8_t rw_buffer[2 * MAX_CONFIG_SIZE] = {0};

    /*Write 0x0000 to the CommStat register (0x61) two times in a row to unlock write protection*/
    if (MAX_unlock_write_protection(MAX_Dev) != ERROR_OK)
        return MAX_FAILED_TO_UPDATE_NVCONFIG;

    /*Write desired memory locations to new values*/
    if (MAX_fill_rw_array(rw_buffer, NV_Config, (2 * MAX_CONFIG_SIZE), MAX_CONFIG_SIZE) != ERROR_OK)
        return MAX_FAILED_TO_UPDATE_NVCONFIG;

    if (MAX_set_regs(MAX_Dev, MAX_SLAVE_ADDRESS_NVM, N_XTABLE0_ADDRESS, rw_buffer, 224) != ERROR_OK)
        return MAX_FAILED_TO_UPDATE_NVCONFIG;

    /*Write 0x0000 to the CommStat register (0x61) one more time to clear CommStat.NVError bit*/
    rw_buffer[0] = 0;
    rw_buffer[1] = 0;
    if (MAX_set_regs(MAX_Dev, MAX_SLAVE_ADDRESS, REG61_COMMSTATUS, rw_buffer, 2) != ERROR_OK)
        return MAX_FAILED_TO_UPDATE_NVCONFIG;

    /*Perform soft reset*/
    if (MAX_soft_reset(MAX_Dev) != ERROR_OK)
        return MAX_FAILED_TO_UPDATE_NVCONFIG;


#if defined(MAX_DEBUG)
    if (MAX_get_regs(MAX_Dev, MAX_SLAVE_ADDRESS_NVM, N_XTABLE0_ADDRESS, rw_buffer, 224) != ERROR_OK)
        return MAX_FAILED_TO_UPDATE_NVCONFIG;
    uint16_t config = 0;
    printf("------\r\n");
    for (uint8_t i = 0; i < MAX_CONFIG_SIZE; i++)
    {
        config = MAX_convert_to_word(rw_buffer[i * 2], rw_buffer[(i * 2) + 1]);
        printf("%x\r\n", config);
    }
#endif

    return ERROR_OK;
}

syscon_error_t MAX_write_nvconfig(struct MAX17320_Dev *MAX_Dev)
{
    uint8_t rw_buffer[2] = {0};
    uint8_t remaining_cycles = 0;

    if (MAX_get_remaining_write_cycles(MAX_Dev, &remaining_cycles) != ERROR_OK)
        return MAX_FAILED_TO_GET_NVM_CYCLES;

    if (remaining_cycles == 0)
        return MAX_NO_NVM_CYCLES_REMAINING;

    do
    {
        /*update NVM shadow RAM to desired values*/
        if (MAX_update_nvconfig(MAX_Dev) != ERROR_OK)
            return MAX_FAILED_TO_UPDATE_NVCONFIG;
        /*Write 0xE904 to the Command register 0x060 to initiate a block copy*/
        rw_buffer[0] = 0x04;
        rw_buffer[1] = 0xE9;
        if (MAX_set_regs(MAX_Dev, MAX_SLAVE_ADDRESS, REG60_COMMAND, rw_buffer, 2) != ERROR_OK)
            return MAX_FAILED_TO_WRITE_NVCONFIG;
        /*Wait tBLOCK for the copy to complete*/
        delay_ms(7360);
        /*Check the CommStat.NVError bit. If set, repeat the process. If clear, continue*/
        if (MAX_get_regs(MAX_Dev, MAX_SLAVE_ADDRESS, REG61_COMMSTATUS, rw_buffer, 2) != ERROR_OK)
            return MAX_FAILED_TO_WRITE_NVCONFIG;

        /*the watchdog would trigger if the error bit wasn't reset after the first try, so we make sure this doesn't happen;*/
        #ifdef WATCHDOG_EN
            watchdog_update();
        #endif

    } while (MAX17320_GET_BITS(rw_buffer[0], rw_buffer[1], REG61_NV_ERROR));

    /*
    Write 0x000F to the Command register 0x060 to send the full reset command to the IC
    Wait 10ms for the IC to reset. Write protection resets after the full reset command
    */
    if (MAX_full_reset(MAX_Dev) != ERROR_OK)
        return MAX_FAILED_TO_WRITE_NVCONFIG;

    /*Write 0x0000 to the CommStat register (0x61) two times in a row to unlock write protection*/
    if (MAX_unlock_write_protection(MAX_Dev) != ERROR_OK)
        return MAX_FAILED_TO_WRITE_NVCONFIG;
    /*
    Write 0x8000 to the Config2 register 0x0AB to reset firmware.
    Wait for the POR_CMD bit (bit 15) of the Config2 register to be cleared to indicate that the POR sequence is complete
    */
    if (MAX_soft_reset(MAX_Dev) != ERROR_OK)
        return MAX_FAILED_TO_WRITE_NVCONFIG;
    /*Write 0x00F9 to the CommStat register (0x61) two times in a row to lock write protection*/
    if (MAX_lock_write_protection(MAX_Dev) != ERROR_OK)
        return MAX_FAILED_TO_WRITE_NVCONFIG;

    return ERROR_OK;
}

syscon_error_t MAX_soft_reset(struct MAX17320_Dev *MAX_Dev)
{
    uint8_t rw_buffer[2] = {0x00, 0x80};

    if (MAX_set_regs(MAX_Dev, MAX_SLAVE_ADDRESS, REGAB_CONFIG2, rw_buffer, 2) != ERROR_OK)
        return MAX_SOFT_RESET_FAILED;
    do
    {
        /*wait for the POR_CMD to be cleared by the chip to indicate successful reset*/
        if (MAX_get_regs(MAX_Dev, MAX_SLAVE_ADDRESS, REGAB_CONFIG2, rw_buffer, 2) != ERROR_OK)
            return MAX_SOFT_RESET_FAILED;
    } while (MAX17320_GET_BITS(rw_buffer[0], rw_buffer[1], REGAB_POR_CMD));

    return ERROR_OK;
}

syscon_error_t MAX_full_reset(struct MAX17320_Dev *MAX_Dev)
{
    uint8_t rw_buffer[2] = {0x0F, 0x00};

    if (MAX_set_regs(MAX_Dev, MAX_SLAVE_ADDRESS, REGAB_CONFIG2, rw_buffer, 2) != ERROR_OK)
        return MAX_FULL_RESET_FAILED;
    delay_ms(10);

    return ERROR_OK;
}

static syscon_error_t MAX_unlock_write_protection(struct MAX17320_Dev *MAX_Dev)
{
    uint8_t rw_buffer[2] = {0x00, 0x00};
    for (uint8_t i = 0; i < 2; i++)
    {
        if (MAX_set_regs(MAX_Dev, MAX_SLAVE_ADDRESS, REG61_COMMSTATUS, rw_buffer, 2) != ERROR_OK)
            return MAX_UNLOCK_FAILED;
        delay_ms(10);
    }

#if defined(MAX_DEBUG)
    printf("MAX unlocked!:\r\n");
#endif
    return ERROR_OK;
}

static syscon_error_t MAX_lock_write_protection(struct MAX17320_Dev *MAX_Dev)
{
    uint8_t rw_buffer[2] = {0xF9, 0x00};
    for (uint8_t i = 0; i < 2; i++)
    {
        if (MAX_set_regs(MAX_Dev, MAX_SLAVE_ADDRESS, REG61_COMMSTATUS, rw_buffer, 2) != ERROR_OK)
            return MAX_UNLOCK_FAILED;
        delay_ms(10);
    }

#if defined(MAX_DEBUG)
    printf("MAX locked!:\r\n");
#endif
    return ERROR_OK;
}

static syscon_error_t MAX_get_remaining_write_cycles(struct MAX17320_Dev *MAX_Dev, uint8_t *remaining_cycles)
{
    uint8_t rw_buffer[2] = {0x00, 0x00};
    uint8_t temp_remaining_cycles = 0;

    if (MAX_unlock_write_protection(MAX_Dev) != ERROR_OK)
        return MAX_FAILED_TO_GET_NVM_CYCLES;

    rw_buffer[0] = 0x9B;
    rw_buffer[1] = 0xE2;
    if (MAX_set_regs(MAX_Dev, MAX_SLAVE_ADDRESS, REG60_COMMAND, rw_buffer, 2) != ERROR_OK)
        return MAX_FAILED_TO_GET_NVM_CYCLES;

    delay_ms(5); // t_RECALL

    if (MAX_set_regs(MAX_Dev, MAX_SLAVE_ADDRESS_NVM, REG61_COMMSTATUS, rw_buffer, 2) != ERROR_OK)
        return MAX_FAILED_TO_GET_NVM_CYCLES;

    temp_remaining_cycles = rw_buffer[0] | rw_buffer[1];

    for (uint8_t i = 0; i < 7; i++)
    {
        if ((temp_remaining_cycles >> i) != 0)
        {
            *remaining_cycles = i + 1;
        }
    }

    if (MAX_lock_write_protection(MAX_Dev) != ERROR_OK)
        return MAX_FAILED_TO_GET_NVM_CYCLES;

#if defined(MAX_DEBUG)
    printf("MAX remaining write cycles: %x\r\n", *remaining_cycles);
#endif

    return ERROR_OK;
}

syscon_error_t MAX_get_batt_properties(struct MAX17320_Dev *MAX_Dev)
{
    uint8_t rw_buffer[6] = {0x00};

    if (MAX_get_regs(MAX_Dev, MAX_SLAVE_ADDRESS, REG05_REPCAP, rw_buffer, 4) != ERROR_OK)
        return MAX_FAILED_TO_GET_PROPERTIES;
    MAX_Dev->MAX_batt_properties.remaining_capacity = MAX_convert_to_capacity(rw_buffer[0], rw_buffer[1]);
    MAX_Dev->MAX_batt_properties.state_of_charge = MAX_convert_to_percentage(rw_buffer[2], rw_buffer[3]);

    if (MAX_get_regs(MAX_Dev, MAX_SLAVE_ADDRESS, REG1A_VCELL, rw_buffer, 6) != ERROR_OK)
        return MAX_FAILED_TO_GET_PROPERTIES;
    MAX_Dev->MAX_batt_properties.cell_voltage = MAX_convert_to_voltage(rw_buffer[0], rw_buffer[1]);
    MAX_Dev->MAX_batt_properties.temperature = MAX_convert_to_temperature(rw_buffer[2], rw_buffer[3]);
    MAX_Dev->MAX_batt_properties.battery_current_mA = MAX_convert_to_current(rw_buffer[4], rw_buffer[5]);

    if (MAX_get_regs(MAX_Dev, MAX_SLAVE_ADDRESS, REG11_TIME_TO_EMPTY, rw_buffer, 2) != ERROR_OK)
        return MAX_FAILED_TO_GET_PROPERTIES;
    MAX_Dev->MAX_batt_properties.time_to_empty = MAX_convert_to_time(rw_buffer[0], rw_buffer[1]);

    if (MAX_get_regs(MAX_Dev, MAX_SLAVE_ADDRESS, REG20_TIME_TO_FULL, rw_buffer, 2) != ERROR_OK)
        return MAX_FAILED_TO_GET_PROPERTIES;
    MAX_Dev->MAX_batt_properties.time_to_full = MAX_convert_to_time(rw_buffer[0], rw_buffer[1]);

    if (MAX_get_regs(MAX_Dev, MAX_SLAVE_ADDRESS, REGDB_PCKP, rw_buffer, 2) != ERROR_OK)
        return MAX_FAILED_TO_GET_PROPERTIES;
    MAX_Dev->MAX_batt_properties.pack_voltage = MAX_convert_to_special(rw_buffer[0], rw_buffer[1]);
    MAX_Dev->MAX_batt_properties.battery_power = MAX_Dev->MAX_batt_properties.pack_voltage * (MAX_Dev->MAX_batt_properties.battery_current_mA / 1000);

#if defined(MAX_DEBUG)
    printf("----------------------------------------------\r\n");
    printf("MAX battery properties:\r\n");
    printf("remaining capacity: %f mAh\r\n", MAX_Dev->MAX_batt_properties.remaining_capacity);
    printf("state of charge: %f %%\r\n", MAX_Dev->MAX_batt_properties.state_of_charge);
    printf("cell voltage: %f V\r\n", MAX_Dev->MAX_batt_properties.cell_voltage);
    printf("temperature: %f °C\r\n", MAX_Dev->MAX_batt_properties.temperature);
    printf("battery current: %f mA\r\n", MAX_Dev->MAX_batt_properties.battery_current_mA);
    printf("time to empty: %f min\r\n", MAX_Dev->MAX_batt_properties.time_to_empty);
    printf("time to full: %f min\r\n", MAX_Dev->MAX_batt_properties.time_to_full);
    printf("pack voltage: %f V\r\n", MAX_Dev->MAX_batt_properties.pack_voltage);
    printf("total battery power: %f W\r\n", MAX_Dev->MAX_batt_properties.battery_power);
    printf("----------------------------------------------\r\n");
#endif

    return ERROR_OK;
}

syscon_error_t MAX_get_status(struct MAX17320_Dev *MAX_Dev)
{

    uint8_t rw_buffer[2] = {0};
    uint16_t buffer = 0;

    if (MAX_get_regs(MAX_Dev, MAX_SLAVE_ADDRESS, REG00_STATUS1, rw_buffer, 2) != ERROR_OK)
        return MAX_FAILED_TO_GET_PROPERTIES;

    MAX_Dev->MAX_status.por = MAX17320_GET_BITS(rw_buffer[0], rw_buffer[1], REG00_POR);
    MAX_Dev->MAX_status.min_curr_alert = MAX17320_GET_BITS(rw_buffer[0], rw_buffer[1], REG00_IMN);
    MAX_Dev->MAX_status.max_curr_alert = MAX17320_GET_BITS(rw_buffer[0], rw_buffer[1], REG00_IMX);
    MAX_Dev->MAX_status.soc_change_alert = MAX17320_GET_BITS(rw_buffer[0], rw_buffer[1], REG00_DSOCI);
    MAX_Dev->MAX_status.vcell_min_alert = MAX17320_GET_BITS(rw_buffer[0], rw_buffer[1], REG00_VMN);
    MAX_Dev->MAX_status.min_temp_alert = MAX17320_GET_BITS(rw_buffer[0], rw_buffer[1], REG00_TMN);
    MAX_Dev->MAX_status.min_soc_alert = MAX17320_GET_BITS(rw_buffer[0], rw_buffer[1], REG00_SMN);
    MAX_Dev->MAX_status.vcell_max_alert = MAX17320_GET_BITS(rw_buffer[0], rw_buffer[1], REG00_VMX);
    MAX_Dev->MAX_status.max_temp_alert = MAX17320_GET_BITS(rw_buffer[0], rw_buffer[1], REG00_TMX);
    MAX_Dev->MAX_status.max_soc_alert = MAX17320_GET_BITS(rw_buffer[0], rw_buffer[1], REG00_SMX);
    MAX_Dev->MAX_status.protection_alert = MAX17320_GET_BITS(rw_buffer[0], rw_buffer[1], REG00_PA);

    if (MAX_get_regs(MAX_Dev, MAX_SLAVE_ADDRESS, REGB0_STATUS2, rw_buffer, 2) != ERROR_OK)
        return MAX_FAILED_TO_GET_PROPERTIES;
    
    MAX_Dev->MAX_status.hibernate = MAX17320_GET_BITS(rw_buffer[0], rw_buffer[1], REGB0_HIB);

    if (MAX_get_regs(MAX_Dev, MAX_SLAVE_ADDRESS, REGD9_PROTSTATUS, rw_buffer, 2) != ERROR_OK)
        return MAX_FAILED_TO_GET_PROPERTIES;

    MAX_Dev->MAX_prot_status.ship_state = MAX17320_GET_BITS(rw_buffer[0], rw_buffer[1], REGD9_SHIP);
    MAX_Dev->MAX_prot_status.resdfault = MAX17320_GET_BITS(rw_buffer[0], rw_buffer[1], REGD9_RESDFAULT);
    MAX_Dev->MAX_prot_status.overdischarge_curr_fault = MAX17320_GET_BITS(rw_buffer[0], rw_buffer[1], REGD9_ODCP);
    MAX_Dev->MAX_prot_status.undervoltage_fault = MAX17320_GET_BITS(rw_buffer[0], rw_buffer[1], REGD9_UVP);
    MAX_Dev->MAX_prot_status.overtemp_discharge_fault = MAX17320_GET_BITS(rw_buffer[0], rw_buffer[1], REGD9_TOOHOTD);
    MAX_Dev->MAX_prot_status.overtemp_die_fault = MAX17320_GET_BITS(rw_buffer[0], rw_buffer[1], REGD9_DIEHOT);
    MAX_Dev->MAX_prot_status.permfail = MAX17320_GET_BITS(rw_buffer[0], rw_buffer[1], REGD9_PERMFAIL);
    MAX_Dev->MAX_prot_status.multicell_imbalance = MAX17320_GET_BITS(rw_buffer[0], rw_buffer[1], REGD9_IMBALANCE);
    MAX_Dev->MAX_prot_status.prequal_timeout = MAX17320_GET_BITS(rw_buffer[0], rw_buffer[1], REGD9_PREQF);
    MAX_Dev->MAX_prot_status.capacity_overflow = MAX17320_GET_BITS(rw_buffer[0], rw_buffer[1], REGD9_QOVFLW);
    MAX_Dev->MAX_prot_status.overcharge_curr_fault = MAX17320_GET_BITS(rw_buffer[0], rw_buffer[1], REGD9_OCCP);
    MAX_Dev->MAX_prot_status.overvoltage_fault = MAX17320_GET_BITS(rw_buffer[0], rw_buffer[1], REGD9_OVP);
    MAX_Dev->MAX_prot_status.undertemp_charge_fault = MAX17320_GET_BITS(rw_buffer[0], rw_buffer[1], REGD9_TOOCOLDC);
    MAX_Dev->MAX_prot_status.batt_full_detect = MAX17320_GET_BITS(rw_buffer[0], rw_buffer[1], REGD9_FULL);
    MAX_Dev->MAX_prot_status.overtemp_charge_fault = MAX17320_GET_BITS(rw_buffer[0], rw_buffer[1], REGD9_TOOHOTC);
    MAX_Dev->MAX_prot_status.watchdog_timeout = MAX17320_GET_BITS(rw_buffer[0], rw_buffer[1], REGD9_CHGWDT);

    if (MAX_get_regs(MAX_Dev, MAX_SLAVE_ADDRESS, REGAF_PROTALRT, rw_buffer, 2) != ERROR_OK)
        return MAX_FAILED_TO_GET_PROPERTIES;

    MAX_Dev->MAX_prot_alert.leakage_detection_fault = MAX17320_GET_BITS(rw_buffer[0], rw_buffer[1], REGAF_LDET);
    MAX_Dev->MAX_prot_alert.resdfault = MAX17320_GET_BITS(rw_buffer[0], rw_buffer[1], REGAF_RESDFAULT);
    MAX_Dev->MAX_prot_alert.overdischarge_curr_fault = MAX17320_GET_BITS(rw_buffer[0], rw_buffer[1], REGAF_ODCP);
    MAX_Dev->MAX_prot_alert.undervoltage_fault = MAX17320_GET_BITS(rw_buffer[0], rw_buffer[1], REGAF_UVP);
    MAX_Dev->MAX_prot_alert.overtemp_discharge_fault = MAX17320_GET_BITS(rw_buffer[0], rw_buffer[1], REGAF_TOOHOTD);
    MAX_Dev->MAX_prot_alert.overtemp_die_fault = MAX17320_GET_BITS(rw_buffer[0], rw_buffer[1], REGAF_DIEHOT);
    MAX_Dev->MAX_prot_alert.permfail = MAX17320_GET_BITS(rw_buffer[0], rw_buffer[1], REGAF_PERMFAIL);
    MAX_Dev->MAX_prot_alert.multicell_imbalance = MAX17320_GET_BITS(rw_buffer[0], rw_buffer[1], REGAF_IMBALANCE);
    MAX_Dev->MAX_prot_alert.prequal_timeout = MAX17320_GET_BITS(rw_buffer[0], rw_buffer[1], REGAF_PREQF);
    MAX_Dev->MAX_prot_alert.capacity_overflow = MAX17320_GET_BITS(rw_buffer[0], rw_buffer[1], REGAF_QOVFLW);
    MAX_Dev->MAX_prot_alert.overcharge_curr_fault = MAX17320_GET_BITS(rw_buffer[0], rw_buffer[1], REGAF_OCCP);
    MAX_Dev->MAX_prot_alert.overvoltage_fault = MAX17320_GET_BITS(rw_buffer[0], rw_buffer[1], REGAF_OVP);
    MAX_Dev->MAX_prot_alert.undertemp_charge_fault = MAX17320_GET_BITS(rw_buffer[0], rw_buffer[1], REGAF_TOOCOLDC);
    MAX_Dev->MAX_prot_alert.batt_full_detect = MAX17320_GET_BITS(rw_buffer[0], rw_buffer[1], REGAF_FULL);
    MAX_Dev->MAX_prot_alert.overtemp_charge_fault = MAX17320_GET_BITS(rw_buffer[0], rw_buffer[1], REGAF_TOOHOTC);
    MAX_Dev->MAX_prot_alert.watchdog_timeout = MAX17320_GET_BITS(rw_buffer[0], rw_buffer[1], REGAF_CHGWDT);
    

    return ERROR_OK;
}

syscon_error_t MAX_get_chip_id(struct MAX17320_Dev *MAX_Dev)
{
    uint8_t rw_buffer[4] = {0};
    uint16_t buffer = 0;
    if (MAX_get_regs(MAX_Dev, MAX_SLAVE_ADDRESS, 0x21, rw_buffer, 4) != ERROR_OK)
        return MAX_FAILED_TO_GET_PROPERTIES;

    buffer = MAX_convert_to_word(rw_buffer[0], rw_buffer[1]);
    MAX_Dev->MAX_Chip_ID = buffer;

#if defined(MAX_DEBUG)
    printf("ID: %x\r\n", buffer);
#endif

    return ERROR_OK;
}

syscon_error_t MAX_enter_ship_mode(struct MAX17320_Dev *MAX_Dev)
{
    uint8_t rw_buffer[2] = {0};
    uint16_t buffer = 0;

    if (MAX_unlock_write_protection(MAX_Dev) != ERROR_OK)
        return MAX_FAILED_TO_SET_SHIPMODE;

    if (MAX_get_regs(MAX_Dev, MAX_SLAVE_ADDRESS, REG0B_CONFIG, rw_buffer, 2) != ERROR_OK)
        return MAX_FAILED_TO_SET_SHIPMODE;

    buffer = (uint16_t)rw_buffer[0] | (((uint16_t)rw_buffer[1]) << 8);
    MAX17320_SET_BITS(buffer, REG0B_SHIP, MAX_REG_TRUE);
    rw_buffer[0] = (uint8_t)buffer;
    rw_buffer[1] = (uint8_t)(buffer >> 8);

    if (MAX_set_regs(MAX_Dev, MAX_SLAVE_ADDRESS, REG0B_CONFIG, rw_buffer, 2) != ERROR_OK)
        return MAX_FAILED_TO_SET_SHIPMODE;

    if (MAX_lock_write_protection(MAX_Dev) != ERROR_OK)
        return MAX_FAILED_TO_SET_SHIPMODE;

    return ERROR_OK;
}

uint16_t MAX_convert_to_word(uint8_t raw_l, uint8_t raw_h)
{
    uint16_t raw = (uint16_t)raw_l | (((uint16_t)raw_h) << 8);
    return raw;
}

static syscon_error_t MAX_fill_rw_array(uint8_t *rw_array, uint16_t *input_array, uint16_t output_array_size, uint16_t input_array_size)
{
    /*check that the sizes make sense, as we split the 16 bit value into 2 8 bit values, the output size must always be 2x the input size*/
    if (output_array_size != (2 * input_array_size))
        return GENERIC_ERROR;

    for (uint16_t i = 0; i < input_array_size; i++)
    {
        rw_array[i * 2] = (uint8_t)input_array[i];
        rw_array[(i * 2) + 1] |= (uint8_t)(input_array[i] >> 8);
    }

    return ERROR_OK;
}

syscon_error_t MAX_get_regs(struct MAX17320_Dev *MAX_Dev, uint8_t device_address, uint8_t start_address, uint8_t *rw_buffer, uint8_t no_of_regs)
{
    int8_t result = ERROR_OK;

    MAX_Dev->MAX_busy = 1;
    if (i2c_read(MAX_Dev->interface, device_address, start_address, rw_buffer, no_of_regs) != I2C_SUCCESS)
        result = GENERIC_ERROR;
    MAX_Dev->MAX_busy = 0;
    return result;
}

static syscon_error_t MAX_set_regs(struct MAX17320_Dev *MAX_Dev, uint8_t device_address, uint8_t start_address, uint8_t *rw_buffer, uint8_t no_of_regs)
{
    int8_t result = ERROR_OK;

    MAX_Dev->MAX_busy = 1;
    if (i2c_write(MAX_Dev->interface, device_address, start_address, rw_buffer, no_of_regs) != I2C_SUCCESS)
        result = GENERIC_ERROR;
    MAX_Dev->MAX_busy = 0;
    return result;
}

syscon_error_t MAX_dump_settings_uart(struct MAX17320_Dev *MAX_Dev)
{
   //debug only - dump MAX settings
   static uint8_t rw_buffer[2 * MAX_CONFIG_SIZE] = {0};
   if (MAX_get_regs(MAX_Dev, MAX_SLAVE_ADDRESS_NVM, N_XTABLE0_ADDRESS, rw_buffer, 224) != ERROR_OK)
       return MAX_FAILED_TO_UPDATE_NVCONFIG;

   uint16_t config = 0;
   printf("------\r\n");
   for (uint8_t i = 0; i < MAX_CONFIG_SIZE; i++)
   {
       config = MAX_convert_to_word(rw_buffer[i * 2], rw_buffer[(i * 2) + 1]);
       printf("0x%04X\r\n", config);
   }

}


static float MAX_convert_to_percentage(uint8_t raw_l, uint8_t raw_h)
{
    uint16_t raw = MAX_convert_to_word(raw_l, raw_h);
    float result = (float)raw / 256.0;
    return result;
}
static float MAX_convert_to_current(uint8_t raw_l, uint8_t raw_h)
{
    int16_t raw = (int16_t)MAX_convert_to_word(raw_l, raw_h);
    float result = (float)raw * 0.15625;
    return result;
}
static float MAX_convert_to_time(uint8_t raw_l, uint8_t raw_h)
{
    uint16_t raw = MAX_convert_to_word(raw_l, raw_h);
    float result = raw * (5.625 / 60);
    return result;
}
static float MAX_convert_to_capacity(uint8_t raw_l, uint8_t raw_h)
{
    uint16_t raw = MAX_convert_to_word(raw_l, raw_h);
    float result = raw * 0.5;
    return result;
}
static float MAX_convert_to_temperature(uint8_t raw_l, uint8_t raw_h)
{
    int16_t raw = (int16_t)MAX_convert_to_word(raw_l, raw_h);
    float result = raw / 256.0;
    return result;
}
static float MAX_convert_to_voltage(uint8_t raw_l, uint8_t raw_h)
{
    uint16_t raw = MAX_convert_to_word(raw_l, raw_h);
    float result = (float)raw * 0.078125 / 1000.0;
    return result;
}
static float MAX_convert_to_special(uint8_t raw_l, uint8_t raw_h)
{
    uint16_t raw = MAX_convert_to_word(raw_l, raw_h);
    float result = (float)raw * 0.3125 / 1000.0;
    return result;
}
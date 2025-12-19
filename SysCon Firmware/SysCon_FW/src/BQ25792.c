#include "BQ25792.h"

void BQ_init_structs(struct BQ25792_Dev *BQ_Dev)
{
    BQ_Dev->BQ_Chip_ID = 0;
    BQ_Dev->BQ_busy = 0;
    BQ_Dev->Charge_Enable_Flag = 0;
    BQ_Dev->ADC_Enable_Flag = 0;

    // init config data
    BQ_Dev->BQ_Charge_Config.vsysmin = 0x12;        // 7V
    BQ_Dev->BQ_Charge_Config.vreg = 0x348;          // 8,4V
    BQ_Dev->BQ_Charge_Config.ichg = 0x64;           // 3A = 0x12C
    BQ_Dev->BQ_Charge_Config.vindpm = 0x24;         // 22V
    BQ_Dev->BQ_Charge_Config.iindpm = IINDPM_500MA; // 3A/500mA
    BQ_Dev->BQ_Charge_Config.vbat_lowv = 0x3;       // 71,4%*VREG= 6V
    BQ_Dev->BQ_Charge_Config.iprechg = 0x3;         // 120mA
    // BQ_Dev->BQ_Charge_Config.reg_rst=             0x0;    //0 = no reset
    BQ_Dev->BQ_Charge_Config.iterm = 0x5;           // 200mA
    BQ_Dev->BQ_Charge_Config.cell = 0x1;            // 2S
    BQ_Dev->BQ_Charge_Config.trechg = 0x2;          // 1024ms
    BQ_Dev->BQ_Charge_Config.vrechg = 0x3;          // 200mV
    BQ_Dev->BQ_Charge_Config.votg = 0xDC;           // 5V
    BQ_Dev->BQ_Charge_Config.prechg_tmr = 0x0;      // 2h
    BQ_Dev->BQ_Charge_Config.iotg = 0x19;           // 1A
    BQ_Dev->BQ_Charge_Config.topoff_tmr = 0x0;      // disabled
    BQ_Dev->BQ_Charge_Config.en_trichg_tmr = 0x1;   // enabled
    BQ_Dev->BQ_Charge_Config.en_prechg_tmr = 0x1;   // enabled
    BQ_Dev->BQ_Charge_Config.en_chg_tmr = 0x1;      // enabled
    BQ_Dev->BQ_Charge_Config.chg_tmr = 0x2;         // 12h
    BQ_Dev->BQ_Charge_Config.tmr2x = 0x1;           // slow down 2x in thermal regulation
    BQ_Dev->BQ_Charge_Config.en_auto_ibatdis = 0x1; // auto discharge in OVP
    // BQ_Dev->BQ_Charge_Config.force_ibatdis=       0x0;    //no
    // BQ_Dev->BQ_Charge_Config.en_chg=              0x1;    //charge EN using EN pin
    BQ_Dev->BQ_Charge_Config.en_ico = 0x0;    // disable
    BQ_Dev->BQ_Charge_Config.force_ico = 0x0; // no
    BQ_Dev->BQ_Charge_Config.en_hiz = 0x1;    // HiZ
    BQ_Dev->BQ_Charge_Config.en_term = 0x1;   // enable termination
    BQ_Dev->BQ_Charge_Config.vac_ovp = 0x0;   // 26V
    // BQ_Dev->BQ_Charge_Config.wd_rst=              0x0;    //no reset, is done separately
    BQ_Dev->BQ_Charge_Config.watchdog = 0x0; // watchdog disabled
    // BQ_Dev->BQ_Charge_Config.force_ident=         0x0;    //no
    BQ_Dev->BQ_Charge_Config.auto_indet_en = 0x0;    // disabled
    BQ_Dev->BQ_Charge_Config.en_12v = 0x0;           // disabled
    BQ_Dev->BQ_Charge_Config.en_9v = 0x0;            // disabled
    BQ_Dev->BQ_Charge_Config.hvdcp_en = 0x0;         // disabled
    BQ_Dev->BQ_Charge_Config.sdrv_ctrl = 0x0;        // stay in idle
    BQ_Dev->BQ_Charge_Config.sdrv_dly = 0x0;         // 10s delay for shipfet, not used anyway
    BQ_Dev->BQ_Charge_Config.dis_acdrv = 0x0;        // do not force to 0
    BQ_Dev->BQ_Charge_Config.en_otg = 0x0;           // OTG disabled
    BQ_Dev->BQ_Charge_Config.pfm_otg_dis = 0x0;      // enabled
    BQ_Dev->BQ_Charge_Config.pfm_fwd_dis = 0x0;      // enabled
    BQ_Dev->BQ_Charge_Config.wkup_dly = 0x0;         // 1s wakeup delay; not used
    BQ_Dev->BQ_Charge_Config.dis_ldo = 0x0;          // enabled
    BQ_Dev->BQ_Charge_Config.dis_otg_ooa = 0x0;      // enabled
    BQ_Dev->BQ_Charge_Config.dis_fwd_ooa = 0x0;      // enabled
    BQ_Dev->BQ_Charge_Config.en_acdrv2 = 0x0;        // no FETS populated
    BQ_Dev->BQ_Charge_Config.en_acdrv1 = 0x0;        // no FETS populated
    BQ_Dev->BQ_Charge_Config.pwm_freq = 0x0;         // 1,5MHz
    BQ_Dev->BQ_Charge_Config.dis_stat = 0x0;         // STAT pin enabled
    BQ_Dev->BQ_Charge_Config.dis_vsys_short = 0x0;   // protection enabled
    BQ_Dev->BQ_Charge_Config.dis_votg_uvp = 0x0;     // UVP enabled
    BQ_Dev->BQ_Charge_Config.force_vindpm_det = 0x0; // do not force it
    BQ_Dev->BQ_Charge_Config.en_ibus_ocp = 0x1;      // OCP enabled
    BQ_Dev->BQ_Charge_Config.sfet_present = 0x1;     // no shipfet populated
    BQ_Dev->BQ_Charge_Config.en_ibat = 0x1;          // IBAT sensing enabled
    BQ_Dev->BQ_Charge_Config.ibat_reg = 0x2;         // 5A
    BQ_Dev->BQ_Charge_Config.en_iindpm = 0x1;        // enabled; takes current from IINDPM register
    BQ_Dev->BQ_Charge_Config.en_extilim = 0x1;       // enabled; take ILIM_HIZ input
    BQ_Dev->BQ_Charge_Config.en_batoc = 0x0;         // cannot be enabled without shipfet
    BQ_Dev->BQ_Charge_Config.treg = 0x3;             // 120°C
    BQ_Dev->BQ_Charge_Config.tshut = 0x0;            // 150°C
    BQ_Dev->BQ_Charge_Config.vbus_pd_en = 0x0;       // disabled
    BQ_Dev->BQ_Charge_Config.vac1_pd_en = 0x0;       // disabled
    BQ_Dev->BQ_Charge_Config.vac2_pd_en = 0x0;       // disabled
    BQ_Dev->BQ_Charge_Config.jeita_vset = 0x3;       // VREG-400mV from twarn - thot
    BQ_Dev->BQ_Charge_Config.jeita_iseth = 0x3;      // unchanged
    BQ_Dev->BQ_Charge_Config.jeita_isetc = 0x1;      // set to 20% ICHG from tcold - tcool
    BQ_Dev->BQ_Charge_Config.ts_cool = 0x1;          // 10°C with values from DS
    BQ_Dev->BQ_Charge_Config.ts_warm = 0x1;          // 45°C with values from DS
    BQ_Dev->BQ_Charge_Config.bhot = 0x1;             // 55°C
    BQ_Dev->BQ_Charge_Config.bcold = 0x0;            //-10°C
    BQ_Dev->BQ_Charge_Config.ts_ignore = 0x0;        // do not ignore TS

    /*initial ADC control--------------------------------------------------------------------------------------*/

    BQ_Dev->BQ_ADC_Config.adc_rate = 0x1;                  // single shot
    BQ_Dev->BQ_ADC_Config.adc_sample = 0x2;                // 13 bit resolution
    BQ_Dev->BQ_ADC_Config.adc_avg = 0x0;                   // single value readings, no average
    BQ_Dev->BQ_ADC_Config.adc_avg_init = 0x1;              // initial avg value = ADC reading
    BQ_Dev->BQ_ADC_Config.ibus_adc_dis = ADC_FUNC_ENABLE;  // IBUS ADC EN
    BQ_Dev->BQ_ADC_Config.en_ibat = ADC_FUNC_ENABLE;       // IBAT EN
    BQ_Dev->BQ_ADC_Config.ibat_adc_dis = ADC_FUNC_ENABLE;  // IBAT ADC EN
    BQ_Dev->BQ_ADC_Config.vbus_adc_dis = ADC_FUNC_ENABLE;  // VBUS ADC EN
    BQ_Dev->BQ_ADC_Config.vbat_adc_dis = ADC_FUNC_ENABLE;  // VBAT ADC EN
    BQ_Dev->BQ_ADC_Config.vsys_adc_dis = ADC_FUNC_ENABLE;  // VSYS ADC EN
    BQ_Dev->BQ_ADC_Config.ts_adc_dis = ADC_FUNC_ENABLE;    // TS ADC EN
    BQ_Dev->BQ_ADC_Config.tdie_adc_dis = ADC_FUNC_ENABLE;  // TDIE ADC EN
    BQ_Dev->BQ_ADC_Config.dp_adc_dis = ADC_FUNC_DISABLE;   // D+ ADC DI
    BQ_Dev->BQ_ADC_Config.dm_adc_dis = ADC_FUNC_DISABLE;   // D- ADC DI
    BQ_Dev->BQ_ADC_Config.vac2_adc_dis = ADC_FUNC_DISABLE; // VAC2 ADC DI
    BQ_Dev->BQ_ADC_Config.vac1_adc_dis = ADC_FUNC_DISABLE; // VAC1 ADC DI
}

syscon_error_t BQ_set_config(struct BQ25792_Dev *BQ_Dev)
{
    uint8_t rw_buffer[21] = {0};

    /*reading the config registers for modification*/
    if (BQ_get_regs(BQ_Dev, REG00_MINIMAL_SYSTEM_VOLTAGE, rw_buffer, 21) != ERROR_OK)
        return BQ_FAILED_TO_GET_CURRENT_CONFIG;

    /*Setting the corresponding bits in the register structure*/
    rw_buffer[0] = BQ25792_SET_BITS(rw_buffer[0], REG00_VSYSMIN, BQ_Dev->BQ_Charge_Config.vsysmin);
    rw_buffer[1] = BQ25792_SET_BITS_UPPER(rw_buffer[1], REG01_VREG, BQ_Dev->BQ_Charge_Config.vreg);
    rw_buffer[2] = BQ25792_SET_BITS(rw_buffer[2], REG02_VREG, BQ_Dev->BQ_Charge_Config.vreg);
    rw_buffer[3] = BQ25792_SET_BITS_UPPER(rw_buffer[3], REG03_ICHG, BQ_Dev->BQ_Charge_Config.ichg);
    rw_buffer[4] = BQ25792_SET_BITS(rw_buffer[4], REG04_ICHG, BQ_Dev->BQ_Charge_Config.ichg);
    rw_buffer[5] = BQ25792_SET_BITS(rw_buffer[5], REG05_VINDPM, BQ_Dev->BQ_Charge_Config.vindpm);
    rw_buffer[6] = BQ25792_SET_BITS_UPPER(rw_buffer[6], REG06_IINDPM, BQ_Dev->BQ_Charge_Config.iindpm);
    rw_buffer[7] = BQ25792_SET_BITS(rw_buffer[7], REG07_IINDPM, BQ_Dev->BQ_Charge_Config.iindpm);
    rw_buffer[8] = BQ25792_SET_BITS(rw_buffer[8], REG08_VBATLOWV, BQ_Dev->BQ_Charge_Config.vbat_lowv);
    rw_buffer[8] = BQ25792_SET_BITS(rw_buffer[8], REG08_IPRECHG, BQ_Dev->BQ_Charge_Config.iprechg);
    rw_buffer[9] = BQ25792_SET_BITS(rw_buffer[9], REG09_ITERM, BQ_Dev->BQ_Charge_Config.iterm);
    rw_buffer[10] = BQ25792_SET_BITS(rw_buffer[10], REG0A_CELL, BQ_Dev->BQ_Charge_Config.cell);
    rw_buffer[10] = BQ25792_SET_BITS(rw_buffer[10], REG0A_TRECHG, BQ_Dev->BQ_Charge_Config.trechg);
    rw_buffer[10] = BQ25792_SET_BITS(rw_buffer[10], REG0A_VRECHG, BQ_Dev->BQ_Charge_Config.vrechg);
    rw_buffer[11] = BQ25792_SET_BITS_UPPER(rw_buffer[11], REG0B_VOTG, BQ_Dev->BQ_Charge_Config.votg);
    rw_buffer[12] = BQ25792_SET_BITS(rw_buffer[12], REG0C_VOTG, BQ_Dev->BQ_Charge_Config.votg);
    rw_buffer[13] = BQ25792_SET_BITS(rw_buffer[13], REG0D_PRECHG_TMR, BQ_Dev->BQ_Charge_Config.prechg_tmr);
    rw_buffer[13] = BQ25792_SET_BITS(rw_buffer[14], REG0D_IOTG, BQ_Dev->BQ_Charge_Config.iotg);
    rw_buffer[14] = BQ25792_SET_BITS(rw_buffer[14], REG0E_TOPOFF_TMR, BQ_Dev->BQ_Charge_Config.topoff_tmr);
    rw_buffer[14] = BQ25792_SET_BITS(rw_buffer[14], REG0E_EN_TRICHG_TMR, BQ_Dev->BQ_Charge_Config.en_trichg_tmr);
    rw_buffer[14] = BQ25792_SET_BITS(rw_buffer[14], REG0E_EN_PRECHG_TMR, BQ_Dev->BQ_Charge_Config.en_prechg_tmr);
    rw_buffer[14] = BQ25792_SET_BITS(rw_buffer[14], REG0E_EN_CHG_TMR, BQ_Dev->BQ_Charge_Config.en_chg_tmr);
    rw_buffer[14] = BQ25792_SET_BITS(rw_buffer[14], REG0E_CHG_TMR, BQ_Dev->BQ_Charge_Config.chg_tmr);
    rw_buffer[14] = BQ25792_SET_BITS(rw_buffer[14], REG0E_TMR2X_EN, BQ_Dev->BQ_Charge_Config.tmr2x);
    rw_buffer[15] = BQ25792_SET_BITS(rw_buffer[15], REG0F_EN_AUTO_IBATDIS, BQ_Dev->BQ_Charge_Config.en_auto_ibatdis);
    rw_buffer[15] = BQ25792_SET_BITS(rw_buffer[15], REG0F_EN_ICO, BQ_Dev->BQ_Charge_Config.en_ico);
    rw_buffer[15] = BQ25792_SET_BITS(rw_buffer[15], REG0F_FORCE_ICO, BQ_Dev->BQ_Charge_Config.force_ico);
    rw_buffer[15] = BQ25792_SET_BITS(rw_buffer[15], REG0F_EN_HIZ, BQ_Dev->BQ_Charge_Config.en_hiz);
    rw_buffer[15] = BQ25792_SET_BITS(rw_buffer[15], REG0F_EN_TERM, BQ_Dev->BQ_Charge_Config.en_term);
    rw_buffer[16] = BQ25792_SET_BITS(rw_buffer[16], REG10_VAC_OVP, BQ_Dev->BQ_Charge_Config.vac_ovp);
    rw_buffer[16] = BQ25792_SET_BITS(rw_buffer[16], REG10_WATCHDOG, BQ_Dev->BQ_Charge_Config.watchdog);
    rw_buffer[17] = BQ25792_SET_BITS(rw_buffer[17], REG11_AUTO_INDET_EN, BQ_Dev->BQ_Charge_Config.auto_indet_en);
    rw_buffer[17] = BQ25792_SET_BITS(rw_buffer[17], REG11_EN_12V, BQ_Dev->BQ_Charge_Config.en_12v);
    rw_buffer[17] = BQ25792_SET_BITS(rw_buffer[17], REG11_EN_9V, BQ_Dev->BQ_Charge_Config.en_9v);
    rw_buffer[17] = BQ25792_SET_BITS(rw_buffer[17], REG11_HVDCP_EN, BQ_Dev->BQ_Charge_Config.hvdcp_en);
    rw_buffer[17] = BQ25792_SET_BITS(rw_buffer[17], REG11_SDRV_CTRL, BQ_Dev->BQ_Charge_Config.sdrv_ctrl);
    rw_buffer[17] = BQ25792_SET_BITS(rw_buffer[17], REG11_SDRV_DLY, BQ_Dev->BQ_Charge_Config.sdrv_dly);
    rw_buffer[18] = BQ25792_SET_BITS(rw_buffer[18], REG12_DIS_ACDRV, BQ_Dev->BQ_Charge_Config.dis_acdrv);
    rw_buffer[18] = BQ25792_SET_BITS(rw_buffer[18], REG12_EN_OTG, BQ_Dev->BQ_Charge_Config.en_otg);
    rw_buffer[18] = BQ25792_SET_BITS(rw_buffer[18], REG12_PFM_OTG_DIS, BQ_Dev->BQ_Charge_Config.pfm_otg_dis);
    rw_buffer[18] = BQ25792_SET_BITS(rw_buffer[18], REG12_PFM_FWD_DIS, BQ_Dev->BQ_Charge_Config.pfm_fwd_dis);
    rw_buffer[18] = BQ25792_SET_BITS(rw_buffer[18], REG12_WKUP_DLY, BQ_Dev->BQ_Charge_Config.wkup_dly);
    rw_buffer[18] = BQ25792_SET_BITS(rw_buffer[18], REG12_DIS_LDO, BQ_Dev->BQ_Charge_Config.dis_ldo);
    rw_buffer[18] = BQ25792_SET_BITS(rw_buffer[18], REG12_DIS_OTG_OOA, BQ_Dev->BQ_Charge_Config.dis_otg_ooa);
    rw_buffer[18] = BQ25792_SET_BITS(rw_buffer[18], REG12_DIS_FWD_OOA, BQ_Dev->BQ_Charge_Config.dis_fwd_ooa);
    rw_buffer[19] = BQ25792_SET_BITS(rw_buffer[19], REG13_EN_ACDRV2, BQ_Dev->BQ_Charge_Config.en_acdrv2);
    rw_buffer[19] = BQ25792_SET_BITS(rw_buffer[19], REG13_EN_ACDRV1, BQ_Dev->BQ_Charge_Config.en_acdrv1);
    rw_buffer[19] = BQ25792_SET_BITS(rw_buffer[19], REG13_PWM_FREQ, BQ_Dev->BQ_Charge_Config.pwm_freq);
    rw_buffer[19] = BQ25792_SET_BITS(rw_buffer[19], REG13_DIS_STAT, BQ_Dev->BQ_Charge_Config.dis_stat);
    rw_buffer[19] = BQ25792_SET_BITS(rw_buffer[19], REG13_DIS_VSYS_SHORT, BQ_Dev->BQ_Charge_Config.dis_vsys_short);
    rw_buffer[19] = BQ25792_SET_BITS(rw_buffer[19], REG13_DIS_VOTG_UVP, BQ_Dev->BQ_Charge_Config.dis_votg_uvp);
    rw_buffer[19] = BQ25792_SET_BITS(rw_buffer[19], REG13_EN_IBUS_OCP, BQ_Dev->BQ_Charge_Config.en_ibus_ocp);
    rw_buffer[19] = BQ25792_SET_BITS(rw_buffer[19], REG13_FORCE_VINDPM_DET, BQ_Dev->BQ_Charge_Config.force_vindpm_det);
    rw_buffer[20] = BQ25792_SET_BITS(rw_buffer[20], REG14_SFET_PRESENT, BQ_Dev->BQ_Charge_Config.sfet_present);
    rw_buffer[20] = BQ25792_SET_BITS(rw_buffer[20], REG14_EN_IBAT, BQ_Dev->BQ_Charge_Config.en_ibat);
    rw_buffer[20] = BQ25792_SET_BITS(rw_buffer[20], REG14_IBAT_REG, BQ_Dev->BQ_Charge_Config.ibat_reg);
    rw_buffer[20] = BQ25792_SET_BITS(rw_buffer[20], REG14_EN_IINDPM, BQ_Dev->BQ_Charge_Config.en_iindpm);
    rw_buffer[20] = BQ25792_SET_BITS(rw_buffer[20], REG14_EN_EXTILIM, BQ_Dev->BQ_Charge_Config.en_extilim);
    rw_buffer[20] = BQ25792_SET_BITS(rw_buffer[20], REG14_EN_BATOC, BQ_Dev->BQ_Charge_Config.en_batoc);

    if (BQ_set_regs(BQ_Dev, REG00_MINIMAL_SYSTEM_VOLTAGE, rw_buffer, 21) != ERROR_OK)
        return BQ_FAILED_TO_SET_NEW_CONFIG;

    /*doing the second config block separately to avoid the reserved reg 15*/
    if (BQ_get_regs(BQ_Dev, REG16_TEMPERATURE_CONTROL, rw_buffer, 3) != ERROR_OK)
        return BQ_FAILED_TO_GET_CURRENT_CONFIG;

    rw_buffer[0] = BQ25792_SET_BITS(rw_buffer[0], REG16_TREG, BQ_Dev->BQ_Charge_Config.treg);
    rw_buffer[0] = BQ25792_SET_BITS(rw_buffer[0], REG16_TSHUT, BQ_Dev->BQ_Charge_Config.tshut);
    rw_buffer[0] = BQ25792_SET_BITS(rw_buffer[0], REG16_VBUS_PD_EN, BQ_Dev->BQ_Charge_Config.vbus_pd_en);
    rw_buffer[0] = BQ25792_SET_BITS(rw_buffer[0], REG16_VAC1_PD_EN, BQ_Dev->BQ_Charge_Config.vac1_pd_en);
    rw_buffer[0] = BQ25792_SET_BITS(rw_buffer[0], REG16_VAC2_PD_EN, BQ_Dev->BQ_Charge_Config.vac2_pd_en);
    rw_buffer[1] = BQ25792_SET_BITS(rw_buffer[1], REG17_JEITA_VSET, BQ_Dev->BQ_Charge_Config.jeita_vset);
    rw_buffer[1] = BQ25792_SET_BITS(rw_buffer[1], REG17_JEITA_ISETH, BQ_Dev->BQ_Charge_Config.jeita_iseth);
    rw_buffer[1] = BQ25792_SET_BITS(rw_buffer[1], REG17_JEITA_ISETC, BQ_Dev->BQ_Charge_Config.jeita_isetc);
    rw_buffer[2] = BQ25792_SET_BITS(rw_buffer[2], REG18_TS_COOL, BQ_Dev->BQ_Charge_Config.ts_cool);
    rw_buffer[2] = BQ25792_SET_BITS(rw_buffer[2], REG18_TS_WARM, BQ_Dev->BQ_Charge_Config.ts_warm);
    rw_buffer[2] = BQ25792_SET_BITS(rw_buffer[2], REG18_BHOT, BQ_Dev->BQ_Charge_Config.bhot);
    rw_buffer[2] = BQ25792_SET_BITS(rw_buffer[2], REG18_BCOLD, BQ_Dev->BQ_Charge_Config.bcold);
    rw_buffer[2] = BQ25792_SET_BITS(rw_buffer[2], REG18_TS_IGNORE, BQ_Dev->BQ_Charge_Config.ts_ignore);

    if (BQ_set_regs(BQ_Dev, REG16_TEMPERATURE_CONTROL, rw_buffer, 3) != ERROR_OK)
        return BQ_FAILED_TO_SET_NEW_CONFIG;

    /*return OK if everything was successful*/
    return ERROR_OK;
}

syscon_error_t BQ_getID(struct BQ25792_Dev *BQ_Dev)
{
    uint8_t rw_buffer[1] = {0};

    if (BQ_get_regs(BQ_Dev, REG48_PART_INFORMATION, rw_buffer, 1) != ERROR_OK)
        return BQ_FAILED_TO_GET_CHIP_ID;

    BQ_Dev->BQ_Chip_ID = BQ25792_GET_BITS(rw_buffer[0], REG48_PN);
    return ERROR_OK;
}

static syscon_error_t BQ_get_regs(struct BQ25792_Dev *BQ_Dev, uint8_t start_address, uint8_t *rw_buffer, uint8_t no_of_regs)
{
    int8_t result = ERROR_OK;

    BQ_Dev->BQ_busy = 1;
    if (i2c_read(BQ_Dev->interface, BQ25792_SLAVE_ADDRESS, start_address, rw_buffer, no_of_regs) != I2C_SUCCESS)
        result = GENERIC_ERROR;
    BQ_Dev->BQ_busy = 0;
    return result;
}

static syscon_error_t BQ_set_regs(struct BQ25792_Dev *BQ_Dev, uint8_t start_address, uint8_t *rw_buffer, uint8_t no_of_regs)
{
    int8_t result = ERROR_OK;

    BQ_Dev->BQ_busy = 1;
    if (i2c_write(BQ_Dev->interface, BQ25792_SLAVE_ADDRESS, start_address, rw_buffer, no_of_regs) != I2C_SUCCESS)
        result = GENERIC_ERROR;
    BQ_Dev->BQ_busy = 0;
    return result;
}

syscon_error_t BQ_CHG_EnDi(struct BQ25792_Dev *BQ_Dev, uint8_t ENorDI)
{
    uint8_t rw_buffer[1] = {0};

    /*read charge control 0 register to get current values*/
    if (BQ_get_regs(BQ_Dev, REG0F_CHARGER_CONTROL_0, rw_buffer, 1) != ERROR_OK)
        return BQ_FAILED_TO_SET_CHARGE_EN;

    /*set or reset EN_CHG bit in REG0F */
    if (ENorDI == ENABLE)
        rw_buffer[0] = BQ25792_SET_BITS(rw_buffer[0], REG0F_EN_CHG, CHG_ENABLE);
    else if (ENorDI == DISABLE)
        rw_buffer[0] = BQ25792_SET_BITS(rw_buffer[0], REG0F_EN_CHG, CHG_DISABLE);
    else
        return BQ_WRONG_INPUT_PARAMETER;

    /*write new register value to BQ*/
    if (BQ_set_regs(BQ_Dev, REG0F_CHARGER_CONTROL_0, rw_buffer, 1) != ERROR_OK)
        return BQ_FAILED_TO_SET_CHARGE_EN;

    BQ_Dev->Charge_Enable_Flag = ENorDI;
    return ERROR_OK;
}

syscon_error_t BQ_ADC_EnDi(struct BQ25792_Dev *BQ_Dev, uint8_t ENorDI)
{
    uint8_t rw_buffer[1] = {0};

    /*read adc control register to get current values*/
    if (BQ_get_regs(BQ_Dev, REG2E_ADC_CONTROL, rw_buffer, 1) != ERROR_OK)
        return BQ_FAILED_TO_SET_ADC_EN;

    /*set or reset EN_CHG bit in REG0F */
    if (ENorDI == ENABLE)
        rw_buffer[0] = BQ25792_SET_BITS(rw_buffer[0], REG2E_ADC_EN, ADC_ENABLE);
    else if (ENorDI == DISABLE)
        rw_buffer[0] = BQ25792_SET_BITS(rw_buffer[0], REG2E_ADC_EN, ADC_DISABLE);
    else
        return BQ_WRONG_INPUT_PARAMETER;

    /*write new register value to BQ*/
    if (BQ_set_regs(BQ_Dev, REG2E_ADC_CONTROL, rw_buffer, 1) != ERROR_OK)
        return BQ_FAILED_TO_SET_ADC_EN;

    BQ_Dev->ADC_Enable_Flag = ENorDI;
    return ERROR_OK;
}

syscon_error_t BQ_write_ADC_config(struct BQ25792_Dev *BQ_Dev)
{
    uint8_t rw_buffer[3] = {0};
    /*reading the config registers for modification*/
    if (BQ_get_regs(BQ_Dev, REG2E_ADC_CONTROL, rw_buffer, 3) != ERROR_OK)
        return BQ_FAILED_TO_GET_CURRENT_ADC_CONFIG;

    rw_buffer[0] = BQ25792_SET_BITS(rw_buffer[0], REG2E_ADC_RATE, BQ_Dev->BQ_ADC_Config.adc_rate);
    rw_buffer[0] = BQ25792_SET_BITS(rw_buffer[0], REG2E_ADC_SAMPLE, BQ_Dev->BQ_ADC_Config.adc_sample);
    rw_buffer[0] = BQ25792_SET_BITS(rw_buffer[0], REG2E_ADC_AVG, BQ_Dev->BQ_ADC_Config.adc_avg);
    rw_buffer[0] = BQ25792_SET_BITS(rw_buffer[0], REG2E_ADC_AVG_INIT, BQ_Dev->BQ_ADC_Config.adc_avg_init);
    rw_buffer[1] = BQ25792_SET_BITS(rw_buffer[1], REG2F_IBUS_ADC_DIS, BQ_Dev->BQ_ADC_Config.ibus_adc_dis);
    rw_buffer[1] = BQ25792_SET_BITS(rw_buffer[1], REG2F_IBAT_ADC_DIS, BQ_Dev->BQ_ADC_Config.ibat_adc_dis);
    rw_buffer[1] = BQ25792_SET_BITS(rw_buffer[1], REG2F_VBUS_ADC_DIS, BQ_Dev->BQ_ADC_Config.vbus_adc_dis);
    rw_buffer[1] = BQ25792_SET_BITS(rw_buffer[1], REG2F_VBAT_ADC_DIS, BQ_Dev->BQ_ADC_Config.vbat_adc_dis);
    rw_buffer[1] = BQ25792_SET_BITS(rw_buffer[1], REG2F_VSYS_ADC_DIS, BQ_Dev->BQ_ADC_Config.vsys_adc_dis);
    rw_buffer[1] = BQ25792_SET_BITS(rw_buffer[1], REG2F_TS_ADC_DIS, BQ_Dev->BQ_ADC_Config.ts_adc_dis);
    rw_buffer[1] = BQ25792_SET_BITS(rw_buffer[1], REG2F_TDIE_ADC_DIS, BQ_Dev->BQ_ADC_Config.tdie_adc_dis);
    rw_buffer[2] = BQ25792_SET_BITS(rw_buffer[2], REG30_DP_ADC_DIS, BQ_Dev->BQ_ADC_Config.dp_adc_dis);
    rw_buffer[2] = BQ25792_SET_BITS(rw_buffer[2], REG30_DM_ADC_DIS, BQ_Dev->BQ_ADC_Config.dm_adc_dis);
    rw_buffer[2] = BQ25792_SET_BITS(rw_buffer[2], REG30_VAC2_ADC_DIS, BQ_Dev->BQ_ADC_Config.vac2_adc_dis);
    rw_buffer[2] = BQ25792_SET_BITS(rw_buffer[2], REG30_VAC1_ADC_DIS, BQ_Dev->BQ_ADC_Config.vac1_adc_dis);

    /*writing the updated registers to the chip*/
    if (BQ_set_regs(BQ_Dev, REG2E_ADC_CONTROL, rw_buffer, 3) != ERROR_OK)
        return BQ_FAILED_TO_SET_CURRENT_ADC_CONFIG;

    if (BQ_get_regs(BQ_Dev, REG14_CHARGER_CONTROL_5, rw_buffer, 1) != ERROR_OK)
        return BQ_FAILED_TO_GET_CURRENT_ADC_CONFIG;

    rw_buffer[0] = BQ25792_SET_BITS(rw_buffer[0], REG14_EN_IBAT, BQ_Dev->BQ_ADC_Config.en_ibat);

    if (BQ_set_regs(BQ_Dev, REG14_CHARGER_CONTROL_5, rw_buffer, 1) != ERROR_OK)
        return BQ_FAILED_TO_SET_CURRENT_ADC_CONFIG;

    /*return OK if everything was successful*/
    return ERROR_OK;
}

syscon_error_t BQ_read_ADC(struct BQ25792_Dev *BQ_Dev)
{
    uint8_t rw_buffer[22] = {0};
    /*reading the ADC registers*/
    if (BQ_get_regs(BQ_Dev, REG31_IBUS_ADC_H, rw_buffer, 22) != ERROR_OK)
        return BQ_FAILED_TO_GET_ADC_READINGS;

    if (BQ_ADC_EnDi(BQ_Dev, ADC_ENABLE) != ERROR_OK)
        return BQ_FAILED_TO_GET_ADC_READINGS;

    BQ_Dev->BQ_ADC_Readings.ibus_adc = (uint16_t)rw_buffer[1] | (((uint16_t)rw_buffer[0] << 8));
    BQ_Dev->BQ_ADC_Readings.ibat_adc = (uint16_t)rw_buffer[3] | (((uint16_t)rw_buffer[2]) << 8);
    BQ_Dev->BQ_ADC_Readings.vbus_adc = (uint16_t)rw_buffer[5] | (((uint16_t)rw_buffer[4]) << 8);
    BQ_Dev->BQ_ADC_Readings.vac1_adc = (uint16_t)rw_buffer[7] | (((uint16_t)rw_buffer[6]) << 8);
    BQ_Dev->BQ_ADC_Readings.vac2_adc = (uint16_t)rw_buffer[9] | (((uint16_t)rw_buffer[8]) << 8);
    BQ_Dev->BQ_ADC_Readings.vbat_adc = (uint16_t)rw_buffer[11] | (((uint16_t)rw_buffer[10]) << 8);
    BQ_Dev->BQ_ADC_Readings.vsys_adc = (uint16_t)rw_buffer[13] | (((uint16_t)rw_buffer[12]) << 8);
    BQ_Dev->BQ_ADC_Readings.ts_adc = (uint16_t)rw_buffer[15] | (((uint16_t)rw_buffer[14]) << 8);
    BQ_Dev->BQ_ADC_Readings.tdie_adc = (uint16_t)rw_buffer[17] | (((uint16_t)rw_buffer[16]) << 8);
    BQ_Dev->BQ_ADC_Readings.dp_adc = (uint16_t)rw_buffer[19] | (((uint16_t)rw_buffer[18]) << 8);
    BQ_Dev->BQ_ADC_Readings.dm_adc = (uint16_t)rw_buffer[21] | (((uint16_t)rw_buffer[20]) << 8);

#if defined(BQ_DEBUG)
    printf("----------------------------------------------\r\n");
    printf("ADC READINGS:\r\n");
    if (BQ_Dev->BQ_ADC_Config.ibus_adc_dis == ADC_FUNC_ENABLE)
        printf("IBUS: %dmA\r\n", BQ_Dev->BQ_ADC_Readings.ibus_adc);
    if ((BQ_Dev->BQ_ADC_Config.ibat_adc_dis == ADC_FUNC_ENABLE) && (BQ_Dev->BQ_ADC_Config.en_ibat == ADC_FUNC_ENABLE))
        printf("IBAT: %dmA\r\n", BQ_Dev->BQ_ADC_Readings.ibat_adc);
    if (BQ_Dev->BQ_ADC_Config.vbus_adc_dis == ADC_FUNC_ENABLE)
        printf("VBUS: %umV\r\n", BQ_Dev->BQ_ADC_Readings.vbus_adc);
    if (BQ_Dev->BQ_ADC_Config.vbat_adc_dis == ADC_FUNC_ENABLE)
        printf("VBAT: %umV\r\n", BQ_Dev->BQ_ADC_Readings.vbat_adc);
    if (BQ_Dev->BQ_ADC_Config.vsys_adc_dis == ADC_FUNC_ENABLE)
        printf("VSYS: %umV\r\n", BQ_Dev->BQ_ADC_Readings.vsys_adc);
    if (BQ_Dev->BQ_ADC_Config.ts_adc_dis == ADC_FUNC_ENABLE)
        printf("TempSens: %f%%\r\n", (float)(BQ_Dev->BQ_ADC_Readings.ts_adc) * 0.0976563f);
    if (BQ_Dev->BQ_ADC_Config.tdie_adc_dis == ADC_FUNC_ENABLE)
        printf("DieTemp: %f°C\r\n", (float)(BQ_Dev->BQ_ADC_Readings.tdie_adc * 0.5));
    if (BQ_Dev->BQ_ADC_Config.dp_adc_dis == ADC_FUNC_ENABLE)
        printf("D+: %umV\r\n", BQ_Dev->BQ_ADC_Readings.dp_adc);
    if (BQ_Dev->BQ_ADC_Config.dm_adc_dis == ADC_FUNC_ENABLE)
        printf("D-: %umV\r\n", BQ_Dev->BQ_ADC_Readings.dm_adc);
    if (BQ_Dev->BQ_ADC_Config.vac2_adc_dis == ADC_FUNC_ENABLE)
        printf("VAC2: %umV\r\n", BQ_Dev->BQ_ADC_Readings.vac2_adc);
    if (BQ_Dev->BQ_ADC_Config.vac1_adc_dis == ADC_FUNC_ENABLE)
        printf("VAC1: %umV\r\n", BQ_Dev->BQ_ADC_Readings.vac1_adc);
    printf("----------------------------------------------\r\n");
#endif
    return ERROR_OK;
}

syscon_error_t BQ_getCHG_Status(struct BQ25792_Dev *BQ_Dev)
{
    uint8_t rw_buffer[5] = {0};
    /*reading the ADC registers*/
    if (BQ_get_regs(BQ_Dev, REG1B_CHARGER_STATUS_0, rw_buffer, 5) != ERROR_OK)
        return BQ_FAILED_TO_GET_STATUS;

    /*status 0 register*/
    BQ_Dev->BQ_CHGStatus.iindpm_stat = BQ25792_GET_BITS(rw_buffer[0], REG1B_IINDPM_STAT);
    BQ_Dev->BQ_CHGStatus.vindpm_stat = BQ25792_GET_BITS(rw_buffer[0], REG1B_VINDPM_STAT);
    BQ_Dev->BQ_CHGStatus.wd_stat = BQ25792_GET_BITS(rw_buffer[0], REG1B_WD_STAT);
    BQ_Dev->BQ_CHGStatus.poorsrc_stat = BQ25792_GET_BITS(rw_buffer[0], REG1B_POORSRC_STAT);
    BQ_Dev->BQ_CHGStatus.pg_stat = BQ25792_GET_BITS(rw_buffer[0], REG1B_PG_STAT);
    BQ_Dev->BQ_CHGStatus.ac2_present_stat = BQ25792_GET_BITS(rw_buffer[0], REG1B_AC2_PRESENT_STAT);
    BQ_Dev->BQ_CHGStatus.ac1_present_stat = BQ25792_GET_BITS(rw_buffer[0], REG1B_AC1_PRESENT_STAT);
    BQ_Dev->BQ_CHGStatus.vbus_present_stat = BQ25792_GET_BITS(rw_buffer[0], REG1B_VBUS_PRESENT_STAT);
    /*status 1 register*/
    BQ_Dev->BQ_CHGStatus.chg_stat = BQ25792_GET_BITS(rw_buffer[1], REG1C_CHG_STAT);
    BQ_Dev->BQ_CHGStatus.vbus_stat = BQ25792_GET_BITS(rw_buffer[1], REG1C_VBUS_STAT);
    BQ_Dev->BQ_CHGStatus.bc12_done_stat = BQ25792_GET_BITS(rw_buffer[1], REG1C_BC12_DONE_STAT);
    /*status 2 register*/
    BQ_Dev->BQ_CHGStatus.ico_stat = BQ25792_GET_BITS(rw_buffer[2], REG1D_ICO_STAT);
    BQ_Dev->BQ_CHGStatus.treg_stat = BQ25792_GET_BITS(rw_buffer[2], REG1D_TREG_STAT);
    BQ_Dev->BQ_CHGStatus.dpdm_stat = BQ25792_GET_BITS(rw_buffer[2], REG1D_DPDM_STAT);
    BQ_Dev->BQ_CHGStatus.vbat_present_stat = BQ25792_GET_BITS(rw_buffer[2], REG1D_VBAT_PRESENT_STAT);
    /*status 3 register*/
    BQ_Dev->BQ_CHGStatus.acrb2_stat = BQ25792_GET_BITS(rw_buffer[3], REG1E_ACRB2_STAT);
    BQ_Dev->BQ_CHGStatus.acrb1_stat = BQ25792_GET_BITS(rw_buffer[3], REG1E_ACRB1_STAT);
    BQ_Dev->BQ_CHGStatus.adc_done_stat = BQ25792_GET_BITS(rw_buffer[3], REG1E_ADC_DONE_STAT);
    BQ_Dev->BQ_CHGStatus.vsys_stat = BQ25792_GET_BITS(rw_buffer[3], REG1E_VSYS_STAT);
    BQ_Dev->BQ_CHGStatus.chg_tmr_stat = BQ25792_GET_BITS(rw_buffer[3], REG1E_CHG_TMR_STAT);
    BQ_Dev->BQ_CHGStatus.trichg_tmr_stat = BQ25792_GET_BITS(rw_buffer[3], REG1E_TRICHG_TMR_STAT);
    BQ_Dev->BQ_CHGStatus.prechg_tmr_stat = BQ25792_GET_BITS(rw_buffer[3], REG1E_PRECHG_TMR_STAT);
    /*status 4 register*/
    BQ_Dev->BQ_CHGStatus.vbatotg_low_stat = BQ25792_GET_BITS(rw_buffer[4], REG1F_VBATOTG_LOW_STAT);
    BQ_Dev->BQ_CHGStatus.ts_cold_stat = BQ25792_GET_BITS(rw_buffer[4], REG1F_TS_COLD_STAT);
    BQ_Dev->BQ_CHGStatus.ts_cool_stat = BQ25792_GET_BITS(rw_buffer[4], REG1F_TS_COOL_STAT);
    BQ_Dev->BQ_CHGStatus.ts_warm_stat = BQ25792_GET_BITS(rw_buffer[4], REG1F_TS_WARM_STAT);
    BQ_Dev->BQ_CHGStatus.ts_hot_stat = BQ25792_GET_BITS(rw_buffer[4], REG1F_TS_HOT_STAT);

#if defined(BQ_DEBUG)
    printf("----------------------------------------------\r\n");
    printf("CHG Status:\r\n");
    printf("IINDPM Status: %x\r\n", BQ_Dev->BQ_CHGStatus.iindpm_stat);
    printf("VINDPM Status: %x\r\n", BQ_Dev->BQ_CHGStatus.vindpm_stat);
    printf("Watchdog Status: %x\r\n", BQ_Dev->BQ_CHGStatus.wd_stat);
    printf("Poor Source Status: %x\r\n", BQ_Dev->BQ_CHGStatus.poorsrc_stat);
    printf("Power Good Status: %x\r\n", BQ_Dev->BQ_CHGStatus.pg_stat);
    printf("VAC2 Insert Status: %x\r\n", BQ_Dev->BQ_CHGStatus.ac2_present_stat);
    printf("VAC1 Insert Status: %x\r\n", BQ_Dev->BQ_CHGStatus.ac1_present_stat);
    printf("VBUS Present Status: %x\r\n", BQ_Dev->BQ_CHGStatus.vbus_present_stat);
    printf("Charge status bits: %x\r\n", BQ_Dev->BQ_CHGStatus.chg_stat);
    printf("VBUS status bits: %x\r\n", BQ_Dev->BQ_CHGStatus.vbus_stat);
    printf("BC1.2 status bit: %x\r\n", BQ_Dev->BQ_CHGStatus.bc12_done_stat);
    printf("ICO Status: %x\r\n", BQ_Dev->BQ_CHGStatus.ico_stat);
    printf("Thermal Regulation Status: %x\r\n", BQ_Dev->BQ_CHGStatus.treg_stat);
    printf("D+/D- Detection Status: %x\r\n", BQ_Dev->BQ_CHGStatus.dpdm_stat);
    printf("Battery Present Status: %x\r\n", BQ_Dev->BQ_CHGStatus.vbat_present_stat);
    printf("ACFET 2 Status: %x\r\n", BQ_Dev->BQ_CHGStatus.acrb2_stat);
    printf("ACFET 1 Status: %x\r\n", BQ_Dev->BQ_CHGStatus.acrb1_stat);
    printf("ADC Conversion Status: %x\r\n", BQ_Dev->BQ_CHGStatus.adc_done_stat);
    printf("VSYS Regulation Status: %x\r\n", BQ_Dev->BQ_CHGStatus.vsys_stat);
    printf("Fast charge timer Status: %x\r\n", BQ_Dev->BQ_CHGStatus.chg_tmr_stat);
    printf("Trickle charge timer Status: %x\r\n", BQ_Dev->BQ_CHGStatus.trichg_tmr_stat);
    printf("Pre-charge timer Status: %x\r\n", BQ_Dev->BQ_CHGStatus.prechg_tmr_stat);
    printf("VBAT too low for OTG: %x\r\n", BQ_Dev->BQ_CHGStatus.vbatotg_low_stat);
    printf("Bat Temp is cold: %x\r\n", BQ_Dev->BQ_CHGStatus.ts_cold_stat);
    printf("Bat Temp is cool: %x\r\n", BQ_Dev->BQ_CHGStatus.ts_cool_stat);
    printf("Bat Temp is warm: %x\r\n", BQ_Dev->BQ_CHGStatus.ts_warm_stat);
    printf("Bat Temp is hot: %x\r\n", BQ_Dev->BQ_CHGStatus.ts_hot_stat);
    printf("----------------------------------------------\r\n");
#endif

    return ERROR_OK;
}

syscon_error_t BQ_getFault_Flag(struct BQ25792_Dev *BQ_Dev)
{
    uint8_t rw_buffer[2] = {0};

    if (BQ_get_regs(BQ_Dev, REG26_FAULT_FLAG_0, rw_buffer, 2) != ERROR_OK)
        return BQ_FAILED_TO_GET_FAULT_FLAGS;

    BQ_Dev->BQ_FaultFlags.ibat_reg_flag = BQ25792_GET_BITS(rw_buffer[0], REG26_IBAT_REG_FLAG);
    BQ_Dev->BQ_FaultFlags.vbus_ovp_flag = BQ25792_GET_BITS(rw_buffer[0], REG26_VBUS_OVP_FLAG);
    BQ_Dev->BQ_FaultFlags.vbat_ovp_flag = BQ25792_GET_BITS(rw_buffer[0], REG26_VBAT_OVP_FLAG);
    BQ_Dev->BQ_FaultFlags.ibus_ocp_flag = BQ25792_GET_BITS(rw_buffer[0], REG26_IBUS_OCP_FLAG);
    BQ_Dev->BQ_FaultFlags.ibat_ocp_flag = BQ25792_GET_BITS(rw_buffer[0], REG26_IBAT_OCP_FLAG);
    BQ_Dev->BQ_FaultFlags.conv_ocp_flag = BQ25792_GET_BITS(rw_buffer[0], REG26_CONV_OCP_FLAG);
    BQ_Dev->BQ_FaultFlags.vac2_ovp_flag = BQ25792_GET_BITS(rw_buffer[0], REG26_VAC2_OVP_FLAG);
    BQ_Dev->BQ_FaultFlags.vac1_ovp_flag = BQ25792_GET_BITS(rw_buffer[0], REG26_VAC1_OVP_FLAG);
    BQ_Dev->BQ_FaultFlags.vsys_short_flag = BQ25792_GET_BITS(rw_buffer[1], REG27_VSYS_SHORT_FLAG);
    BQ_Dev->BQ_FaultFlags.vsys_ovp_flag = BQ25792_GET_BITS(rw_buffer[1], REG27_VSYS_OVP_FLAG);
    BQ_Dev->BQ_FaultFlags.otg_ovp_flag = BQ25792_GET_BITS(rw_buffer[1], REG27_OTG_OVP_FLAG);
    BQ_Dev->BQ_FaultFlags.otg_uvp_flag = BQ25792_GET_BITS(rw_buffer[1], REG27_OTG_UVP_FLAG);
    BQ_Dev->BQ_FaultFlags.tshut_flag = BQ25792_GET_BITS(rw_buffer[1], REG27_TSHUT_FLAG);

#if defined(BQ_DEBUG)
    printf("----------------------------------------------\r\n");
    printf("Fault Flags:\r\n");
    printf("ibat_reg_flag: %x\r\n", BQ_Dev->BQ_FaultFlags.ibat_reg_flag);
    printf("vbus_ovp_flag: %x\r\n", BQ_Dev->BQ_FaultFlags.vbus_ovp_flag);
    printf("vbat_ovp_flag: %x\r\n", BQ_Dev->BQ_FaultFlags.vbat_ovp_flag);
    printf("ibus_ocp_flag: %x\r\n", BQ_Dev->BQ_FaultFlags.ibus_ocp_flag);
    printf("ibat_ocp_flag: %x\r\n", BQ_Dev->BQ_FaultFlags.ibat_ocp_flag);
    printf("conv_ocp_flag: %x\r\n", BQ_Dev->BQ_FaultFlags.conv_ocp_flag);
    printf("vac2_ovp_flag: %x\r\n", BQ_Dev->BQ_FaultFlags.vac2_ovp_flag);
    printf("vac1_ovp_flag: %x\r\n", BQ_Dev->BQ_FaultFlags.vac1_ovp_flag);
    printf("vsys_short_flag: %x\r\n", BQ_Dev->BQ_FaultFlags.vsys_short_flag);
    printf("vsys_ovp_flag: %x\r\n", BQ_Dev->BQ_FaultFlags.vsys_ovp_flag);
    printf("otg_ovp_flag: %x\r\n", BQ_Dev->BQ_FaultFlags.otg_ovp_flag);
    printf("otg_uvp_flag: %x\r\n", BQ_Dev->BQ_FaultFlags.otg_uvp_flag);
    printf("tshut_flag: %x\r\n", BQ_Dev->BQ_FaultFlags.tshut_flag);
    printf("----------------------------------------------\r\n");
#endif
    return ERROR_OK;
}

syscon_error_t BQ_resetREGS(struct BQ25792_Dev *BQ_Dev)
{
    uint8_t rw_buffer[1] = {0};

    if (BQ_get_regs(BQ_Dev, REG09_TERMINATION_CONTROL, rw_buffer, 1) != ERROR_OK)
        return BQ_FAILED_TO_RESET_REGS;

    rw_buffer[0] = BQ25792_SET_BITS(rw_buffer[0], REG09_REG_RST, TRUE);

    if (BQ_set_regs(BQ_Dev, REG09_TERMINATION_CONTROL, rw_buffer, 1) != ERROR_OK)
        return BQ_FAILED_TO_RESET_REGS;

    return ERROR_OK;
}

syscon_error_t BQ_set_input_current_limit(struct BQ25792_Dev *BQ_Dev, uint16_t current_limit)
{
    uint8_t rw_buffer[2] = {0};

    if (BQ_get_regs(BQ_Dev, REG06_INPUT_CURRENT_LIMIT, rw_buffer, 2) != ERROR_OK)
        return BQ_FAILED_TO_SET_INPUT_CURRENT_LIMIT;

    rw_buffer[0] = BQ25792_SET_BITS_UPPER(rw_buffer[0], REG06_IINDPM, current_limit);
    rw_buffer[1] = BQ25792_SET_BITS(rw_buffer[1], REG07_IINDPM, current_limit);

    if (BQ_set_regs(BQ_Dev, REG06_INPUT_CURRENT_LIMIT, rw_buffer, 2) != ERROR_OK)
        return BQ_FAILED_TO_SET_INPUT_CURRENT_LIMIT;

#if defined(BQ_DEBUG)
    printf("BQ update input current limit: %umA\r\n", current_limit);
#endif
    return ERROR_OK;
}

syscon_error_t BQ_check_config_present(struct BQ25792_Dev *BQ_Dev)
{
    uint8_t rw_buffer = 0;

    /*reading the config registers for modification*/
    if (BQ_get_regs(BQ_Dev, REG0D_IOTG_REGULATION, &rw_buffer, 1) != ERROR_OK)
        return BQ_FAILED_TO_GET_CURRENT_CONFIG;

    /*Setting the corresponding bits in the register structure*/
    rw_buffer = BQ25792_GET_BITS(rw_buffer, REG0D_IOTG);

    if(BQ_Dev->BQ_Charge_Config.iotg != rw_buffer) return BQ_CONFIG_MISMATCH;
    else return ERROR_OK;

}


#ifndef BQ25792_h
#define BQ25792_h

#include "BQ25792_Registers.h"
#include "MCU_interface.h"

#define BQ25792_SLAVE_ADDRESS UINT8_C(0xD6>>1) // 0xD7 / 0xD6
#define CHG_ENABLE UINT8_C(0x01)
#define CHG_DISABLE UINT8_C(0x00)
#define ADC_ENABLE UINT8_C(0x01)
#define ADC_DISABLE UINT8_C(0x00)
#define ADC_FUNC_ENABLE UINT8_C(0x00)
#define ADC_FUNC_DISABLE UINT8_C(0x01)

/*BQ charge status for REG1C_CHG_STAT*/
#define BQ_CHG_NOT_CHARGING 0
#define BQ_CHG_TRICKLE_CHARGE   1
#define BQ_CHG_PRE_CHARGE   2
#define BQ_CHG_FAST_CHARGE  3
#define BQ_CHG_TAPER_CHARGE 4
#define BQ_CHG_RESERVED 5
#define BQ_CHG_TOP_OFF  6
#define BQ_CHG_CHARGE_DONE  7

#define VSYSMIN_2S                          UINT8_C(28)
#define VREG_2S                             UINT16_C(840)
#define ICHG_3A                             UINT16_C(300)
#define VINDPM_18V                          UINT8_C(180)
#define IINDPM_3A                           UINT16_C(300)
#define IINDPM_500MA                        UINT16_C(50)
#define FASTCHARGE_FROM_71PCT               UINT8_C(0x3)
#define IPRECHG_200MA                       UINT8_C(0x5)
#define ITERM_200MA                         UINT8_C(0x5)
#define NR_CELL_2S                          UINT8_C(0x1)
#define TRECHG_1024MS                       UINT8_C(0x2)
#define VRECHG_300MV                        UINT8_C(0x5)
#define VRECHG_300MV                        UINT8_C(0x5)




struct BQ25792_Charge_Config
{
    // Main Charge Settings REG00 - REG05
	uint8_t vsysmin;
    uint16_t vreg;
    uint16_t ichg;
    uint8_t vindpm;
    uint16_t iindpm;
    // Precharge Control REG08
    uint8_t vbat_lowv;
    uint8_t iprechg;
    // Termination Control REG09
    //uint8_t reg_rst;
    uint8_t iterm;
    // Recharge Control REG0A
    uint8_t cell;
    uint8_t trechg;
    uint8_t vrechg;
    // OTG Regulation REG0B - REG0D;
    uint16_t votg;
    // REG0D
    uint8_t prechg_tmr;
    uint8_t iotg;
    // Timer Control REG0E
    uint8_t topoff_tmr;
    uint8_t en_trichg_tmr;
    uint8_t en_prechg_tmr;
    uint8_t en_chg_tmr;
    uint8_t chg_tmr;
    uint8_t tmr2x;
    // Charge Control REG0F-REG14
    uint8_t en_auto_ibatdis;
    //uint8_t force_ibatdis;
    //uint8_t en_chg;
    uint8_t en_ico;
    uint8_t force_ico;
    uint8_t en_hiz;
    uint8_t en_term;
    // REG10
    uint8_t vac_ovp;
    //uint8_t wd_rst;
    uint8_t watchdog;
    // REG11
    //uint8_t force_ident;
    uint8_t auto_indet_en;
    uint8_t en_12v;
    uint8_t en_9v;
    uint8_t hvdcp_en;
    uint8_t sdrv_ctrl;
    uint8_t sdrv_dly;
    // REG12
    uint8_t dis_acdrv;
    uint8_t en_otg;
    uint8_t pfm_otg_dis;
    uint8_t pfm_fwd_dis;
    uint8_t wkup_dly;
    uint8_t dis_ldo;
    uint8_t dis_otg_ooa;
    uint8_t dis_fwd_ooa;
    // REG13
    uint8_t en_acdrv2;
    uint8_t en_acdrv1;
    uint8_t pwm_freq;
    uint8_t dis_stat;
    uint8_t dis_vsys_short;
    uint8_t dis_votg_uvp;
    uint8_t force_vindpm_det;
    uint8_t en_ibus_ocp;
    // REG14
    uint8_t sfet_present;
    uint8_t en_ibat;
    uint8_t ibat_reg;
    uint8_t en_iindpm;
    uint8_t en_extilim;
    uint8_t en_batoc;
    // Temperature Control REG16
    uint8_t treg;
    uint8_t tshut;
    uint8_t vbus_pd_en;
    uint8_t vac1_pd_en;
    uint8_t vac2_pd_en;
    // NTC control REG 17 and REG18
    uint8_t jeita_vset;
    uint8_t jeita_iseth;
    uint8_t jeita_isetc;
    uint8_t ts_cool;
    uint8_t ts_warm;
    uint8_t bhot;
    uint8_t bcold;
    uint8_t ts_ignore;
};

struct BQ25792_ADC_Config
{
    uint8_t adc_rate;     // ADC conversion rate control
    uint8_t adc_sample;   // ADC sample speed
    uint8_t adc_avg;      // ADC average control
    uint8_t adc_avg_init; // ADC average initial value control
    uint8_t ibus_adc_dis; // IBUS ADC control
    uint8_t en_ibat;      // IBAT discharge current sensing enable for ADC
    uint8_t ibat_adc_dis; // IBAT ADC control
    uint8_t vbus_adc_dis; // VBUS ADC control
    uint8_t vbat_adc_dis; // VBAT ADC control
    uint8_t vsys_adc_dis; // VSYS ADC control
    uint8_t ts_adc_dis;   // TS ADC control
    uint8_t tdie_adc_dis; // TDIE ADC control
    uint8_t dp_adc_dis;   // D+ ADC Control
    uint8_t dm_adc_dis;   // D- ADC Control
    uint8_t vac2_adc_dis; // VAC2 ADC Control
    uint8_t vac1_adc_dis; // VAC1 ADC Control
};

struct BQ25792_ADC_Readings
{
    int16_t ibus_adc;
    int16_t ibat_adc;
    uint16_t vbus_adc;
    uint16_t vac1_adc;
    uint16_t vac2_adc;
    uint16_t vbat_adc;
    uint16_t vsys_adc;
    uint16_t ts_adc;
    int16_t tdie_adc;
    uint16_t dp_adc;
    uint16_t dm_adc;
};

struct BQ25792_CHGStatus
{
    uint8_t iindpm_stat;       // IINDPM status (forward mode) or IOTG status (OTG mode)
    uint8_t vindpm_stat;       // VINDPM status (forward mode) or VOTG status (OTG mode)
    uint8_t wd_stat;           // I2C watch dog timer status
    uint8_t poorsrc_stat;      // Poor source detection status
    uint8_t pg_stat;            // Power Good Status
    uint8_t ac2_present_stat;  // VAC2 insert status
    uint8_t ac1_present_stat;  // VAC1 insert status
    uint8_t vbus_present_stat; // VBUS present status
    uint8_t chg_stat;          // Charge Status bits
    uint8_t vbus_stat;         // VBUS status bits
    uint8_t bc12_done_stat;    // BC1.2 status bit
    uint8_t ico_stat;          // Input Current Optimizer (ICO) status
    uint8_t treg_stat;         // IC thermal regulation status
    uint8_t dpdm_stat;         // D+/D- detection status bits
    uint8_t vbat_present_stat; // Battery present status (VBAT > VBAT_UVLOZ)
    uint8_t acrb2_stat;        // The ACFET2-RBFET2 status
    uint8_t acrb1_stat;        // The ACFET1-RBFET1 status
    uint8_t adc_done_stat;     // ADC Conversion Status (in one-shot mode only)
    uint8_t vsys_stat;         // VSYS Regulation Status (forward mode)
    uint8_t chg_tmr_stat;      // Fast charge timer status
    uint8_t trichg_tmr_stat;   // Trickle charge timer status
    uint8_t prechg_tmr_stat;   // Pre-charge timer status
    uint8_t vbatotg_low_stat;  // The battery voltage is too low to enable OTG mode.
    uint8_t ts_cold_stat;      // The TS temperature is in the cold range, lower than T1.
    uint8_t ts_cool_stat;      // The TS temperature is in the cool range, between T1 and T2.
    uint8_t ts_warm_stat;      // The TS temperature is in the warm range, between T3 and T5.
    uint8_t ts_hot_stat;       // The TS temperature is in the hot range, higher than T5.
};

struct BQ25792_CHGFlag
{
    uint8_t iindpm_flag;       // 1h = IINDPM / IOTG signal rising edge detected
    uint8_t vindpm_flag;       // 1h = VINDPM / VOTG regulation signal rising edge detected
    uint8_t wd_flag;           // 1h = WD timer signal rising edge detected
    uint8_t poorsrc_flag;      // 1h = Poor source status rising edge detected
    uint8_t pg_flag;           // Power Good; 1h = Any change in PG_STAT even (adapter good qualification or adapter good going away)
    uint8_t ac2_present_flag;  // 1h = VAC2 present status changed
    uint8_t ac1_present_flag;  // 1h = VAC1 present status changed
    uint8_t vbus_present_flag; // 1h = VBUS present status changed
    uint8_t chg_flag;          // 1h = Charge status changed
    uint8_t ico_flag;          // 1h = ICO status changed
    uint8_t vbus_flag;         // 1h = VBUS status changed
    uint8_t treg_flag;         // IC thermal regulation flag; 1h = TREG signal rising threshold detected
    uint8_t vbat_present_flag; // 1h = VBAT present status changed
    uint8_t bc12_done_flag;    // 1h = BC1.2 detection status changed
    uint8_t dpdm_done_flag;    // 1h = D+/D- detection is completed
    uint8_t adc_done_flag;     // ADC conversion flag; 1h = Conversion completed
    uint8_t vsys_flag;         // 1h = Entered or existed VSYSMIN regulation
    uint8_t chg_tmr_flag;      // 1h = Fast charge timer expired rising edge detected
    uint8_t trichg_tmr_flag;   // 1h = Trickle charger timer expired rising edge detected
    uint8_t prechg_tmr_flag;   // 1h = Pre-charge timer expired rising edge detected
    uint8_t topoff_tmr_flag;   // 1h = Top off timer expired rising edge detected
    uint8_t vbatotg_low_flag;  // 1h = VBAT falls below the threshold to enable the OTG mode
    uint8_t ts_cold_flag;      // 1h = TS across cold temperature (T1) is detected
    uint8_t ts_cool_flag;      // 1h = TS across cool temperature (T2) is detected
    uint8_t ts_warm_flag;      // 1h = TS across warm temperature (T3) is detected
    uint8_t ts_hot_flag;       // 1h = TS across hot temperature (T5) is detected
};

struct BQ25792_FaultStatus
{
    uint8_t vsys_short_stat; // VSYS short circuit status
    uint8_t vsys_ovp_stat;   // VSYS over-voltage status
    uint8_t otg_ovp_stat;    // OTG over voltage status
    uint8_t otg_uvp_stat;    // OTG under voltage status.
    uint8_t tshut_stat;      // IC temperature shutdown status
};

struct BQ25792_FaultFlags
{
    uint8_t ibat_reg_flag;   // 1h = Enter or exit IBAT regulation
    uint8_t vbus_ovp_flag;   // 1h = Enter VBUS OVP
    uint8_t vbat_ovp_flag;   // 1h = Enter VBAT OVP
    uint8_t ibus_ocp_flag;   // 1h = Enter IBUS OCP
    uint8_t ibat_ocp_flag;   // 1h = Enter discharged OCP
    uint8_t conv_ocp_flag;   // 1h = Enter converter OCP
    uint8_t vac2_ovp_flag;   // 1h = Enter VAC2 OVP
    uint8_t vac1_ovp_flag;   // 1h = Enter VAC1 OVP
    uint8_t vsys_short_flag; // 1h = Stop switching due to system short
    uint8_t vsys_ovp_flag;   // 1h = Stop switching due to system over-voltage
    uint8_t otg_ovp_flag;    // 1h = Stop OTG due to VBUS over voltage
    uint8_t otg_uvp_flag;    // 1h = Stop OTG due to VBUS under-voltage
    uint8_t tshut_flag;      // 1h = TS shutdown signal rising threshold detected
};

struct BQ25792_INTMasks
{
    uint8_t iindpm_int;       // 1h = Enter IINDPM / IOTG does NOT produce INT pulse
    uint8_t vindpm_int;       // 1h = Enter VINDPM / VOTG does NOT produce INT pulse
    uint8_t wd_int;           // 1h = I2C watch dog timer expired does NOT produce INT pulse
    uint8_t poorsrc_int;      // 1h = Poor source detected does NOT produce INT
    uint8_t pg_int;           // 1h = PG toggle does NOT produce INT
    uint8_t ac2_present_int;  // 1h = VAC2 present status change does NOT produce INT
    uint8_t ac1_present_int;  // 1h = VAC1 present status change does NOT produce INT
    uint8_t vbus_present_int; // 1h = VBUS present status change does NOT produce INT
    uint8_t chg_int;          // 1h = Charging status change does NOT produce INT
    uint8_t ico_int;          // 1h = ICO status change does NOT produce INT
    uint8_t vbus_int;         // 1h = VBUS status change does NOT produce INT
    uint8_t treg_intk;        // 1h = entering TREG does NOT produce INT
    uint8_t vbat_present_int; // 1h = VBAT present status change does NOT produce INT
    uint8_t bc12_done_int;    // 1h = BC1.2 status change does NOT produce INT
    uint8_t dpdm_done_int;    // 1h = D+/D- detection done does NOT produce INT pulse
    uint8_t adc_done_int;     // 1h = ADC conversion done does NOT produce INT pulse
    uint8_t vsys_int;         // 1h = enter or exit VSYSMIN regulation does NOT produce INT pulse
    uint8_t chg_tmr_int;      // 1h = Fast charge timer expire does NOT produce INT
    uint8_t trichg_tmr_int;   // 1h = Trickle charge timer expire does NOT produce INT
    uint8_t prechg_tmr_int;   // 1h = Pre-charge timer expire does NOT produce INT
    uint8_t topoff_tmr_int;   // 1h = Top off timer expire does NOT produce INT
    uint8_t vbatotg_low_int;  // 1h = VBAT falling below the threshold to enable the OTG mode, does NOT produce INT
    uint8_t ts_cold_int;      // 1h = TS across cold temperature (T1) does NOT produce INT
    uint8_t ts_cool_int;      // 1h = TS across cool temperature (T2) does NOT produce INT
    uint8_t ts_warm_int;      // 1h = TS across warm temperature (T3) does NOT produce INT
    uint8_t ts_hot_int;       // 1h = TS across hot temperature (T5) does NOT produce INT
    // fault masks
    uint8_t ibat_reg_mask;   // 1h = enter or exit IBAT regulation does NOT produce INT
    uint8_t vbus_ovp_mask;   // 1h = entering VBUS OVP does NOT produce INT
    uint8_t vbat_ovp_mask;   // 1h = entering VBAT OVP does NOT produce INT
    uint8_t ibus_ocp_mask;   // 1h = IBUS OCP fault does NOT produce INT
    uint8_t ibat_ocp_mask;   // 1h = IBAT OCP fault does NOT produce INT
    uint8_t conv_ocp_mask;   // 1h = Converter OCP fault does NOT produce INT
    uint8_t vac2_ovp_mask;   // 1h = entering VAC2 OVP does NOT produce INT
    uint8_t vac1_ovp_mask;   // 1h = entering VAC1 OVP does NOT produce INT
    uint8_t vsys_short_mask; // 1h = System short fault does NOT produce INT
    uint8_t vsys_ovp_mask;   // 1h = System over-voltage fault does NOT produce INT
    uint8_t otg_ovp_mask;    // 1h = OTG VBUS over-voltage fault does NOT produce INT
    uint8_t otg_uvp_mask;    // 1h = OTG VBUS under voltage fault does NOT produce INT
    uint8_t tshut_mask;      // 1h = TSHUT does NOT produce INT
};

struct BQ25792_Dev
{
    /*interface pointer used to access I2C*/
    i2c_inst_t *interface;
    /*chip ID of the BQ25792*/
    uint8_t BQ_Chip_ID;
    /*indicates when comunication is happening*/
    uint8_t BQ_busy;
    /*Charge EN flag: 1 of charging; 0 if not charging*/
    uint8_t Charge_Enable_Flag;
    /*ADC EN flag: 1 if enabled; 0 if disabled*/
    uint8_t ADC_Enable_Flag;
    
    /*struct containing the charging configuration*/
    struct BQ25792_Charge_Config BQ_Charge_Config;
    /*struct containing the ADC configuration and which channels to sample*/
    struct BQ25792_ADC_Config BQ_ADC_Config;
    /*struct containing the ADC readings; updated when calling the read ADC function*/
    struct BQ25792_ADC_Readings BQ_ADC_Readings;

    struct BQ25792_CHGStatus BQ_CHGStatus;
    struct BQ25792_CHGFlag BQ_CHGFLAG;
    struct BQ25792_FaultStatus BQ_FaultStatus;
    struct BQ25792_FaultFlags BQ_FaultFlags;
    struct BQ25792_INTMasks BQ_INTMasks;
    
};


void BQ_init_structs(struct BQ25792_Dev *BQ_Dev);
syscon_error_t BQ_init_bits(struct BQ25792_Dev *BQ_Dev);
syscon_error_t BQ_set_config(struct BQ25792_Dev *BQ_Dev);
static syscon_error_t BQ_get_regs(struct BQ25792_Dev *BQ_Dev, uint8_t start_address, uint8_t *rw_buffer, uint8_t no_of_regs);
static syscon_error_t BQ_set_regs(struct BQ25792_Dev *BQ_Dev, uint8_t start_address, uint8_t *rw_buffer, uint8_t no_of_regs);
syscon_error_t BQ_getID(struct BQ25792_Dev *BQ_Dev);
syscon_error_t BQ_CHG_EnDi(struct BQ25792_Dev *BQ_Dev, uint8_t ENorDI);
syscon_error_t BQ_ADC_EnDi(struct BQ25792_Dev *BQ_Dev, uint8_t ENorDI);
syscon_error_t BQ_set_input_current_limit(struct BQ25792_Dev *BQ_Dev, uint16_t current_limit);
syscon_error_t BQ_read_ADC(struct BQ25792_Dev *BQ_Dev);
syscon_error_t BQ_write_ADC_config(struct BQ25792_Dev *BQ_Dev);
syscon_error_t BQ_getFault_Flag(struct BQ25792_Dev *BQ_Dev);
syscon_error_t BQ_getCHG_Status(struct BQ25792_Dev *BQ_Dev);
syscon_error_t BQ_resetREGS(struct BQ25792_Dev *BQ_Dev);
syscon_error_t BQ_check_config_present(struct BQ25792_Dev *BQ_Dev);


/*
int8_t BQ_getCHG_Flag(struct BQ25792_Dev *BQ_Dev);
int8_t BQ_getFault_Status(struct BQ25792_Dev *BQ_Dev);
int8_t BQ_setINTMasks(struct BQ25792_Dev *BQ_Dev);
int8_t BQ_Reset_Watchdog(void);
int8_t BQ_SetWatchdogInterval();
*/

#endif

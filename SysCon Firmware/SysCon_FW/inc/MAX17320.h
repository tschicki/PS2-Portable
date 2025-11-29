#ifndef MAX17320_h
#define MAX17320_h

#include "MAX17320_Registers.h"
#include "MAX17320_NVConfig.h"
#include "MCU_interface.h"


#define MAX_SLAVE_ADDRESS           UINT8_C(0x6C>>1)	//0x6C / 0x6D
#define MAX_SLAVE_ADDRESS_NVM       UINT8_C(0x16>>1)	//0x16 / 0x17
#define MAX_CONFIG_SIZE             112
#define MAX_REG_TRUE				UINT16_C(0x0001)

// set bits for the 8 bit regs and lower byte of registers that contain values >8bit
#define MAX17320_SET_BITS(reg_data, bitname, data) \
    ((reg_data & ~(bitname##_MASK)) | \
     ((data << bitname##_POS) & bitname##_MASK))

#define MAX17320_GET_BITS(reg_data_lsb, reg_data_msb, bitname) \
    ((((uint16_t)reg_data_lsb | ((uint16_t)reg_data_msb << 8)) & (bitname##_MASK)) >>       \
     (bitname##_POS))

struct MAX_batt_properties
{
    float remaining_capacity;
    float state_of_charge;
    float cell_voltage;
    float temperature;
    float battery_current_mA;
    float time_to_empty;
    float time_to_full;
    float battery_cycles;
    float pack_voltage;
    float battery_power;
};
struct MAX_status
{
    uint8_t por;
    uint8_t min_curr_alert;
    uint8_t max_curr_alert;
    uint8_t soc_change_alert;
    uint8_t vcell_min_alert;
    uint8_t min_temp_alert;
    uint8_t min_soc_alert;
    uint8_t vcell_max_alert;
    uint8_t max_temp_alert;
    uint8_t max_soc_alert;
    uint8_t protection_alert;
    uint8_t hibernate;
};
struct MAX_prot_status
{
    uint8_t ship_state;
    uint8_t resdfault;
    uint8_t overdischarge_curr_fault;
    uint8_t undervoltage_fault;
    uint8_t overtemp_discharge_fault;
    uint8_t overtemp_die_fault;
    uint8_t permfail;
    uint8_t multicell_imbalance;
    uint8_t prequal_timeout;
    uint8_t capacity_overflow;
    uint8_t overcharge_curr_fault;
    uint8_t overvoltage_fault;
    uint8_t undertemp_charge_fault;
    uint8_t batt_full_detect;
    uint8_t overtemp_charge_fault;
    uint8_t watchdog_timeout;
};
struct MAX_prot_alert
{
    uint8_t leakage_detection_fault;
    uint8_t resdfault;
    uint8_t overdischarge_curr_fault;
    uint8_t undervoltage_fault;
    uint8_t overtemp_discharge_fault;
    uint8_t overtemp_die_fault;
    uint8_t permfail;
    uint8_t multicell_imbalance;
    uint8_t prequal_timeout;
    uint8_t capacity_overflow;
    uint8_t overcharge_curr_fault;
    uint8_t overvoltage_fault;
    uint8_t undertemp_charge_fault;
    uint8_t batt_full_detect;
    uint8_t overtemp_charge_fault;
    uint8_t watchdog_timeout;
};
struct MAX_batt_status
{
    uint8_t leakage_current;
    uint8_t checksum_failure;
    uint8_t leakage_detection_fault;
    uint8_t fet_failure_open;
    uint8_t discharge_fet_short;
    uint8_t charge_fet_short;
    uint8_t severe_otp_failure;
    uint8_t severe_ovp_failure;
    uint8_t permfail;
};

struct MAX17320_Dev
{
    /*interface pointer used to access I2C*/
    i2c_inst_t *interface;
    /*chip ID of the MAX17320*/
    uint16_t MAX_Chip_ID;
    /*busy flag for communication*/
    uint8_t MAX_busy;
    /*contains most important battery properties to read during runtime*/
    struct MAX_batt_properties MAX_batt_properties; 
    struct MAX_status MAX_status;
    struct MAX_prot_status MAX_prot_status;
    struct MAX_prot_alert MAX_prot_alert;
    struct MAX_batt_status MAX_batt_status;
};

syscon_error_t MAX_get_regs(struct MAX17320_Dev *MAX_Dev, uint8_t device_address, uint8_t start_address, uint8_t *rw_buffer, uint8_t no_of_regs);
static syscon_error_t MAX_set_regs(struct MAX17320_Dev *MAX_Dev, uint8_t device_address, uint8_t start_address, uint8_t *rw_buffer, uint8_t no_of_regs);
syscon_error_t MAX_update_nvconfig(struct MAX17320_Dev *MAX_Dev);
syscon_error_t MAX_write_nvconfig(struct MAX17320_Dev *MAX_Dev);
static syscon_error_t MAX_get_remaining_write_cycles(struct MAX17320_Dev *MAX_Dev, uint8_t *remaining_cycles);
syscon_error_t MAX_soft_reset(struct MAX17320_Dev *MAX_Dev);
syscon_error_t MAX_full_reset(struct MAX17320_Dev *MAX_Dev);
static syscon_error_t MAX_unlock_write_protection(struct MAX17320_Dev *MAX_Dev);
static syscon_error_t MAX_lock_write_protection(struct MAX17320_Dev *MAX_Dev);

syscon_error_t MAX_get_batt_properties(struct MAX17320_Dev *MAX_Dev);
syscon_error_t MAX_get_status(struct MAX17320_Dev *MAX_Dev);
syscon_error_t MAX_get_prot_status(struct MAX17320_Dev *MAX_Dev);
syscon_error_t MAX_get_prot_alert(struct MAX17320_Dev *MAX_Dev);
syscon_error_t MAX_get_batt_status(struct MAX17320_Dev *MAX_Dev);
syscon_error_t MAX_get_chip_id(struct MAX17320_Dev *MAX_Dev);
static float MAX_convert_to_percentage(uint8_t raw_l, uint8_t raw_h);
static float MAX_convert_to_time(uint8_t raw_l, uint8_t raw_h);
static float MAX_convert_to_voltage(uint8_t raw_l, uint8_t raw_h);
static float MAX_convert_to_capacity(uint8_t raw_l, uint8_t raw_h);
static float MAX_convert_to_current(uint8_t raw_l, uint8_t raw_h);
static float MAX_convert_to_temperature(uint8_t raw_l, uint8_t raw_h);
static float MAX_convert_to_special(uint8_t raw_l, uint8_t raw_h);

uint16_t MAX_convert_to_word(uint8_t raw_l, uint8_t raw_h);
static syscon_error_t MAX_fill_rw_array(uint8_t *rw_array, uint16_t *input_array, uint16_t output_array_size, uint16_t input_array_size);

syscon_error_t MAX_get_nvm_registers(struct MAX17320_Dev *MAX_Dev);
syscon_error_t MAX_get_chip_id(struct MAX17320_Dev *MAX_Dev);
syscon_error_t MAX_enter_ship_mode(struct MAX17320_Dev *MAX_Dev);

syscon_error_t MAX_dump_settings_uart(struct MAX17320_Dev *MAX_Dev);

#endif

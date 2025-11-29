#ifndef STUSB4500_h
#define STUSB4500_h

#include "STUSB4500_Registers.h"
#include "STUSB4500_NVConfig.h"
#include "MCU_interface.h"

/*device slave address acc. to schematics; needs to be left shifted for HAL*/
#define STUSB4500_SLAVE_ADDRESS UINT8_C(0x50>>1) // 0x50 / 0x51
/*parameters for PDO configuration*/
#define PDO_VOLTAGE_5V UINT16_C(0b0001100100)
#define PDO_VOLTAGE_9V UINT16_C(0b0010110100)
#define PDO_VOLTAGE_15V UINT16_C(0b0100101100)
#define PDO_CURRENT_500MA UINT8_C(0x32)
#define PDO_CURRENT_3A UINT16_C(0x12C)
#define STUSB_ENABLE UINT8_C(0x1)
#define STUSB_DISABLE UINT8_C(0x0)
#define FAST_ROLE_SWAP_NOT_SUPPORTED UINT8_C(0x0)
#define SOFT_RESET_MESSAGE UINT8_C(0x0D)
#define SEND_COMMAND UINT8_C(0x26)

//definitions for CC_STATUS register
#define STUSB4500_CC_NOT_LOOKING 0
#define STUSB4500_CC_LOOKING 1

#define STUSB4500_PRESENT_RP 0 //Pull-Up resistor
#define STUSB4500_PRESENT_RD 1 //Pull-Down resistor

#define STUSB4500_CC_SNK_Open 0
#define STUSB4500_CC_SNK_Default 1
#define STUSB4500_CC_SNK_Power15A 2
#define STUSB4500_CC_SNK_Power30A 3


#define STUSB_CURRENT_500MA     50
#define STUSB_CURRENT_1500MA    150
#define STUSB_CURRENT_3000MA    300

struct PDO_Config
{
    uint16_t current;
    uint16_t voltage;
    uint8_t reserved;
    uint8_t FastRoleSwap;
    uint8_t dual_role_data;
    uint8_t usb_communication_capable;
    uint8_t unconstrained_power;
    uint8_t higher_capability;
    uint8_t dual_role_power;
    uint8_t fixed_supply;
};

struct RDO_Status
{
    uint16_t max_current_mA;
    uint16_t operating_current_mA;
    uint8_t unchunked_msg_supported;
    uint8_t no_usb_suspend;
    uint8_t usb_comm_capable;
    uint8_t capability_mismatch;
    uint8_t giveback_flag;
    uint8_t object_position;
};

struct cc_status
{
    uint8_t cc1_state;
    uint8_t cc2_state;
    uint8_t connect_result;
    uint8_t looking_4_connection;
};

struct STUSB4500_Dev
{
    /*interface pointer used to access I2C*/
    i2c_inst_t *interface;
    /*chip ID of the STUSB4500*/
    uint8_t STUSB_Chip_ID;
    /*indicates when comunication is happening*/
    uint8_t STUSB_busy;
    /*Charger attach flag: 1 if charger attached; 0 if no charger attached*/
    uint8_t Attach_Flag;
    /*PDO configurations for USB PD*/
    struct PDO_Config PDO_configs[3];
    /*USB PD status*/
    struct RDO_Status RDO_status;

    struct cc_status cc_status;

    uint8_t current_PDO;

    uint16_t input_current;

};

syscon_error_t STUSB_init(struct STUSB4500_Dev *STUSB_Dev);
syscon_error_t STUSB_set_PDO(struct STUSB4500_Dev *STUSB_Dev);
static syscon_error_t STUSB_get_regs(struct STUSB4500_Dev *STUSB_Dev, uint8_t start_address, uint8_t *rw_buffer, uint8_t no_of_regs);
static syscon_error_t STUSB_set_regs(struct STUSB4500_Dev *STUSB_Dev, uint8_t start_address, uint8_t *rw_buffer, uint8_t no_of_regs);
syscon_error_t STUSB_re_negotiate(struct STUSB4500_Dev *STUSB_Dev);
syscon_error_t STUSB_get_status(struct STUSB4500_Dev *STUSB_Dev);
syscon_error_t STUSB_clear_interrupts(struct STUSB4500_Dev *STUSB_Dev);
syscon_error_t STUSB_getID(struct STUSB4500_Dev *STUSB_Dev);

syscon_error_t STUSB_get_RDO(struct STUSB4500_Dev *STUSB_Dev);
syscon_error_t STUSB_get_current_at_5v(struct STUSB4500_Dev *STUSB_Dev);
syscon_error_t STUSB_get_current_PDO_number(struct STUSB4500_Dev *STUSB_Dev);
syscon_error_t get_input_current_limit(struct STUSB4500_Dev *STUSB_Dev);

void STUSB_nvm_flash(struct STUSB4500_Dev *STUSB_Dev);
static void STUSB_EnterWriteMode(struct STUSB4500_Dev *STUSB_Dev, uint8_t ErasedSector);
static void STUSB_WriteSector(struct STUSB4500_Dev *STUSB_Dev, char SectorNum, unsigned char *SectorData);
static void STUSB_ExitTestMode(struct STUSB4500_Dev *STUSB_Dev);

#endif

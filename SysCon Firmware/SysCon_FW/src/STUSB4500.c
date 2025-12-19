#include "STUSB4500.h"

syscon_error_t STUSB_init(struct STUSB4500_Dev *STUSB_Dev)
{
    uint8_t rw_buffer[1] = {0};

    /*init PDO 1*/
    STUSB_Dev->PDO_configs[0].current = PDO_CURRENT_500MA;
    STUSB_Dev->PDO_configs[0].voltage = PDO_VOLTAGE_5V;
    STUSB_Dev->PDO_configs[0].FastRoleSwap = FAST_ROLE_SWAP_NOT_SUPPORTED;
    STUSB_Dev->PDO_configs[0].dual_role_data = STUSB_DISABLE;
    STUSB_Dev->PDO_configs[0].usb_communication_capable = STUSB_DISABLE;
    STUSB_Dev->PDO_configs[0].unconstrained_power = STUSB_DISABLE;
    STUSB_Dev->PDO_configs[0].higher_capability = STUSB_DISABLE;
    STUSB_Dev->PDO_configs[0].dual_role_power = STUSB_DISABLE;
    STUSB_Dev->PDO_configs[0].fixed_supply = STUSB_ENABLE;

    /*init PDO 2*/
    STUSB_Dev->PDO_configs[1].current = PDO_CURRENT_3A;
    STUSB_Dev->PDO_configs[1].voltage = PDO_VOLTAGE_9V;
    STUSB_Dev->PDO_configs[1].FastRoleSwap = FAST_ROLE_SWAP_NOT_SUPPORTED;
    STUSB_Dev->PDO_configs[1].dual_role_data = STUSB_DISABLE;
    STUSB_Dev->PDO_configs[1].usb_communication_capable = STUSB_DISABLE;
    STUSB_Dev->PDO_configs[1].unconstrained_power = STUSB_DISABLE;
    STUSB_Dev->PDO_configs[1].higher_capability = STUSB_DISABLE;
    STUSB_Dev->PDO_configs[1].dual_role_power = STUSB_DISABLE;
    STUSB_Dev->PDO_configs[1].fixed_supply = STUSB_ENABLE;

    /*init PDO 3*/
    STUSB_Dev->PDO_configs[2].current = PDO_CURRENT_3A;
    STUSB_Dev->PDO_configs[2].voltage = PDO_VOLTAGE_15V;
    STUSB_Dev->PDO_configs[2].FastRoleSwap = FAST_ROLE_SWAP_NOT_SUPPORTED;
    STUSB_Dev->PDO_configs[2].dual_role_data = STUSB_DISABLE;
    STUSB_Dev->PDO_configs[2].usb_communication_capable = STUSB_DISABLE;
    STUSB_Dev->PDO_configs[2].unconstrained_power = STUSB_DISABLE;
    STUSB_Dev->PDO_configs[2].higher_capability = STUSB_DISABLE;
    STUSB_Dev->PDO_configs[2].dual_role_power = STUSB_DISABLE;
    STUSB_Dev->PDO_configs[2].fixed_supply = STUSB_ENABLE;

    delay_ms(30); // delay to let the STUSB initialize the registers; taken from the datasheet

    if (STUSB_clear_interrupts(STUSB_Dev) != ERROR_OK)
        return STUSB_FAILED_TO_CLEAR_INTERRUPTS;

    /*set interrupt masks*/
    if (STUSB_get_regs(STUSB_Dev, ALERT_STATUS_1_MASK, rw_buffer, 1) != ERROR_OK)
        return STUSB_FAILED_TO_GET_INT_MASKS;

    rw_buffer[0] = STUSB_SET_BITS(rw_buffer[0], PRT_STATUS_AL_INT, STUSB_ENABLE);
    rw_buffer[0] = STUSB_SET_BITS(rw_buffer[0], CC_FAULT_STATUS_AL_INT, STUSB_ENABLE);
    rw_buffer[0] = STUSB_SET_BITS(rw_buffer[0], TYPEC_MONITORING_STATUS_INT, STUSB_ENABLE);
    rw_buffer[0] = STUSB_SET_BITS(rw_buffer[0], PORT_STATUS_AL_INT, STUSB_ENABLE);

    if (STUSB_set_regs(STUSB_Dev, ALERT_STATUS_1_MASK, rw_buffer, 1) != ERROR_OK)
        return STUSB_FAILED_TO_SET_INT_MASKS;

    return ERROR_OK;
}

syscon_error_t STUSB_set_PDO(struct STUSB4500_Dev *STUSB_Dev)
{
    uint8_t rw_buffer[12] = {0};

    /*reading the config registers for modification*/
    if (STUSB_get_regs(STUSB_Dev, DPM_SNK_PDO1_0, rw_buffer, 12) != ERROR_OK)
        return STUSB_FAILED_TO_GET_CURRENT_PDO;

    /*PDO1*/
    rw_buffer[0] = STUSB_SET_BITS(rw_buffer[0], PDO_CURRENT_L, STUSB_Dev->PDO_configs[0].current);
    rw_buffer[1] = STUSB_SET_BITS_UPPER(rw_buffer[1], PDO_CURRENT_H, STUSB_Dev->PDO_configs[0].current);
    rw_buffer[1] = STUSB_SET_BITS(rw_buffer[1], PDO_VOLTAGE_L, STUSB_Dev->PDO_configs[0].voltage);
    rw_buffer[2] = STUSB_SET_BITS_UPPER(rw_buffer[2], PDO_VOLTAGE_H, STUSB_Dev->PDO_configs[0].voltage);
    rw_buffer[2] = STUSB_SET_BITS(rw_buffer[2], PDO_FASTROLESWAP_L, STUSB_Dev->PDO_configs[0].FastRoleSwap);
    rw_buffer[3] = STUSB_SET_BITS_UPPER(rw_buffer[3], PDO_FASTROLESWAP_H, STUSB_Dev->PDO_configs[0].FastRoleSwap);
    rw_buffer[3] = STUSB_SET_BITS(rw_buffer[3], PDO_DUAL_ROLE_DATA, STUSB_Dev->PDO_configs[0].dual_role_data);
    rw_buffer[3] = STUSB_SET_BITS(rw_buffer[3], PDO_USB_COMM_CAPABLE, STUSB_Dev->PDO_configs[0].usb_communication_capable);
    rw_buffer[3] = STUSB_SET_BITS(rw_buffer[3], PDO_UNCONSTRAINED_POWER, STUSB_Dev->PDO_configs[0].unconstrained_power);
    rw_buffer[3] = STUSB_SET_BITS(rw_buffer[3], PDO_HIGHER_CAPABILITY, STUSB_Dev->PDO_configs[0].higher_capability);
    rw_buffer[3] = STUSB_SET_BITS(rw_buffer[3], PDO_DUAL_ROLE_POWER, STUSB_Dev->PDO_configs[0].dual_role_power);
    rw_buffer[3] = STUSB_SET_BITS(rw_buffer[3], PDO_FIXED_SUPPLY, STUSB_Dev->PDO_configs[0].fixed_supply);
    /*PDO2*/
    rw_buffer[4] = STUSB_SET_BITS(rw_buffer[4], PDO_CURRENT_L, STUSB_Dev->PDO_configs[1].current);
    rw_buffer[5] = STUSB_SET_BITS_UPPER(rw_buffer[5], PDO_CURRENT_H, STUSB_Dev->PDO_configs[1].current);
    rw_buffer[5] = STUSB_SET_BITS(rw_buffer[5], PDO_VOLTAGE_L, STUSB_Dev->PDO_configs[1].voltage);
    rw_buffer[6] = STUSB_SET_BITS_UPPER(rw_buffer[6], PDO_VOLTAGE_H, STUSB_Dev->PDO_configs[1].voltage);
    rw_buffer[6] = STUSB_SET_BITS(rw_buffer[6], PDO_FASTROLESWAP_L, STUSB_Dev->PDO_configs[1].FastRoleSwap);
    rw_buffer[7] = STUSB_SET_BITS_UPPER(rw_buffer[7], PDO_FASTROLESWAP_H, STUSB_Dev->PDO_configs[1].FastRoleSwap);
    rw_buffer[7] = STUSB_SET_BITS(rw_buffer[7], PDO_DUAL_ROLE_DATA, STUSB_Dev->PDO_configs[1].dual_role_data);
    rw_buffer[7] = STUSB_SET_BITS(rw_buffer[7], PDO_USB_COMM_CAPABLE, STUSB_Dev->PDO_configs[1].usb_communication_capable);
    rw_buffer[7] = STUSB_SET_BITS(rw_buffer[7], PDO_UNCONSTRAINED_POWER, STUSB_Dev->PDO_configs[1].unconstrained_power);
    rw_buffer[7] = STUSB_SET_BITS(rw_buffer[7], PDO_HIGHER_CAPABILITY, STUSB_Dev->PDO_configs[1].higher_capability);
    rw_buffer[7] = STUSB_SET_BITS(rw_buffer[7], PDO_DUAL_ROLE_POWER, STUSB_Dev->PDO_configs[1].dual_role_power);
    rw_buffer[7] = STUSB_SET_BITS(rw_buffer[7], PDO_FIXED_SUPPLY, STUSB_Dev->PDO_configs[1].fixed_supply);
    /*PDO3*/
    rw_buffer[8] = STUSB_SET_BITS(rw_buffer[8], PDO_CURRENT_L, STUSB_Dev->PDO_configs[2].current);
    rw_buffer[9] = STUSB_SET_BITS_UPPER(rw_buffer[9], PDO_CURRENT_H, STUSB_Dev->PDO_configs[2].current);
    rw_buffer[9] = STUSB_SET_BITS(rw_buffer[9], PDO_VOLTAGE_L, STUSB_Dev->PDO_configs[2].voltage);
    rw_buffer[10] = STUSB_SET_BITS_UPPER(rw_buffer[10], PDO_VOLTAGE_H, STUSB_Dev->PDO_configs[2].voltage);
    rw_buffer[10] = STUSB_SET_BITS(rw_buffer[10], PDO_FASTROLESWAP_L, STUSB_Dev->PDO_configs[2].FastRoleSwap);
    rw_buffer[11] = STUSB_SET_BITS_UPPER(rw_buffer[11], PDO_FASTROLESWAP_H, STUSB_Dev->PDO_configs[2].FastRoleSwap);
    rw_buffer[11] = STUSB_SET_BITS(rw_buffer[11], PDO_DUAL_ROLE_DATA, STUSB_Dev->PDO_configs[2].dual_role_data);
    rw_buffer[11] = STUSB_SET_BITS(rw_buffer[11], PDO_USB_COMM_CAPABLE, STUSB_Dev->PDO_configs[2].usb_communication_capable);
    rw_buffer[11] = STUSB_SET_BITS(rw_buffer[11], PDO_UNCONSTRAINED_POWER, STUSB_Dev->PDO_configs[2].unconstrained_power);
    rw_buffer[11] = STUSB_SET_BITS(rw_buffer[11], PDO_HIGHER_CAPABILITY, STUSB_Dev->PDO_configs[2].higher_capability);
    rw_buffer[11] = STUSB_SET_BITS(rw_buffer[11], PDO_DUAL_ROLE_POWER, STUSB_Dev->PDO_configs[2].dual_role_power);
    rw_buffer[11] = STUSB_SET_BITS(rw_buffer[11], PDO_FIXED_SUPPLY, STUSB_Dev->PDO_configs[2].fixed_supply);

    if (STUSB_set_regs(STUSB_Dev, DPM_SNK_PDO1_0, rw_buffer, 12) != ERROR_OK)
        return STUSB_FAILED_TO_SET_NEW_PDO;

    return ERROR_OK;
}

static syscon_error_t STUSB_get_regs(struct STUSB4500_Dev *STUSB_Dev, uint8_t start_address, uint8_t *rw_buffer, uint8_t no_of_regs)
{
    int8_t result = ERROR_OK;

    STUSB_Dev->STUSB_busy = 1;
    if (i2c_read(STUSB_Dev->interface, STUSB4500_SLAVE_ADDRESS, start_address, rw_buffer, no_of_regs) != I2C_SUCCESS)
        result = GENERIC_ERROR;
    STUSB_Dev->STUSB_busy = 0;
    return result;
}

static syscon_error_t STUSB_set_regs(struct STUSB4500_Dev *STUSB_Dev, uint8_t start_address, uint8_t *rw_buffer, uint8_t no_of_regs)
{
    int8_t result = ERROR_OK;

    STUSB_Dev->STUSB_busy = 1;
    if (i2c_write(STUSB_Dev->interface, STUSB4500_SLAVE_ADDRESS, start_address, rw_buffer, no_of_regs) != I2C_SUCCESS)
        result = GENERIC_ERROR;
    STUSB_Dev->STUSB_busy = 0;
    return result;
}

syscon_error_t STUSB_re_negotiate(struct STUSB4500_Dev *STUSB_Dev)
{
    uint8_t rw_buffer[2] = {0};

    rw_buffer[0] = SOFT_RESET_MESSAGE;
    if (STUSB_set_regs(STUSB_Dev, TX_HEADER_LOW, rw_buffer, 1) != ERROR_OK)
        return STUSB_FAILED_TO_RENEGOTIATE;

    rw_buffer[0] = SEND_COMMAND;
    if (STUSB_set_regs(STUSB_Dev, PD_COMMAND_CTRL, rw_buffer, 1) != ERROR_OK)
        return STUSB_FAILED_TO_RENEGOTIATE;

    return ERROR_OK;
}

syscon_error_t STUSB_clear_interrupts(struct STUSB4500_Dev *STUSB_Dev)
{
    int8_t result = ERROR_OK;
    uint8_t rw_buffer[10] = {0};

    result = STUSB_get_regs(STUSB_Dev, PORT_STATUS_0, rw_buffer, 10) != ERROR_OK;

    return result;
}

void STUSB_nvm_flash(struct STUSB4500_Dev *STUSB_Dev)
{
    /*these following functons were taken from STMicro example code and modified to fit the application*/
    STUSB_EnterWriteMode(STUSB_Dev, SECTOR_0 | SECTOR_1 | SECTOR_2 | SECTOR_3 | SECTOR_4);

    STUSB_WriteSector(STUSB_Dev, 0, Sector0);
    STUSB_WriteSector(STUSB_Dev, 1, Sector1);
    STUSB_WriteSector(STUSB_Dev, 2, Sector2);
    STUSB_WriteSector(STUSB_Dev, 3, Sector3);
    STUSB_WriteSector(STUSB_Dev, 4, Sector4);

    STUSB_ExitTestMode(STUSB_Dev);
}

static void STUSB_EnterWriteMode(struct STUSB4500_Dev *STUSB_Dev, uint8_t ErasedSector)
{
    unsigned char Buffer[2];

    Buffer[0] = FTP_CUST_PASSWORD; /* Set Password*/
    i2c_write(STUSB_Dev->interface, STUSB4500_SLAVE_ADDRESS, FTP_CUST_PASSWORD_REG, Buffer, 1);

    Buffer[0] = 0; /* this register must be NULL for Partial Erase feature */
    i2c_write(STUSB_Dev->interface, STUSB4500_SLAVE_ADDRESS, RW_BUFFER, Buffer, 1);

    {
        // NVM Power-up Sequence
        // After STUSB start-up sequence, the NVM is powered off.

        Buffer[0] = 0; /* NVM internal controller reset */
        i2c_write(STUSB_Dev->interface, STUSB4500_SLAVE_ADDRESS, FTP_CTRL_0, Buffer, 1);

        Buffer[0] = FTP_CUST_PWR | FTP_CUST_RST_N; /* Set PWR and RST_N bits */
        i2c_write(STUSB_Dev->interface, STUSB4500_SLAVE_ADDRESS, FTP_CTRL_0, Buffer, 1);
    }

    Buffer[0] = ((ErasedSector << 3) & FTP_CUST_SER) | (WRITE_SER & FTP_CUST_OPCODE); /* Load 0xF1 to erase all sectors of FTP and Write SER Opcode */
    i2c_write(STUSB_Dev->interface, STUSB4500_SLAVE_ADDRESS, FTP_CTRL_1, Buffer, 1);  /* Set Write SER Opcode */

    Buffer[0] = FTP_CUST_PWR | FTP_CUST_RST_N | FTP_CUST_REQ;
    i2c_write(STUSB_Dev->interface, STUSB4500_SLAVE_ADDRESS, FTP_CTRL_0, Buffer, 1); /* Load Write SER Opcode */

    do
    {
        i2c_read(STUSB_Dev->interface, STUSB4500_SLAVE_ADDRESS, FTP_CTRL_0, Buffer, 1); /* Wait for execution */
    } while (Buffer[0] & FTP_CUST_REQ);
    Buffer[0] = SOFT_PROG_SECTOR & FTP_CUST_OPCODE;
    i2c_write(STUSB_Dev->interface, STUSB4500_SLAVE_ADDRESS, FTP_CTRL_1, Buffer, 1); /* Set Soft Prog Opcode */

    Buffer[0] = FTP_CUST_PWR | FTP_CUST_RST_N | FTP_CUST_REQ;
    i2c_write(STUSB_Dev->interface, STUSB4500_SLAVE_ADDRESS, FTP_CTRL_0, Buffer, 1); /* Load Soft Prog Opcode */

    do
    {
        i2c_read(STUSB_Dev->interface, STUSB4500_SLAVE_ADDRESS, FTP_CTRL_0, Buffer, 1); /* Wait for execution */
    } while (Buffer[0] & FTP_CUST_REQ);
    Buffer[0] = ERASE_SECTOR & FTP_CUST_OPCODE;
    i2c_write(STUSB_Dev->interface, STUSB4500_SLAVE_ADDRESS, FTP_CTRL_1, Buffer, 1); /* Set Erase Sectors Opcode */

    Buffer[0] = FTP_CUST_PWR | FTP_CUST_RST_N | FTP_CUST_REQ;
    i2c_write(STUSB_Dev->interface, STUSB4500_SLAVE_ADDRESS, FTP_CTRL_0, Buffer, 1); /* Load Erase Sectors Opcode */

    do
    {
        i2c_read(STUSB_Dev->interface, STUSB4500_SLAVE_ADDRESS, FTP_CTRL_0, Buffer, 1); /* Wait for execution */
    } while (Buffer[0] & FTP_CUST_REQ);

}

static void STUSB_WriteSector(struct STUSB4500_Dev *STUSB_Dev, char SectorNum, unsigned char *SectorData)
{
    unsigned char Buffer[2];

    // Write the 64-bit data to be written in the sector
    i2c_write(STUSB_Dev->interface, STUSB4500_SLAVE_ADDRESS, RW_BUFFER, SectorData, 8);

    Buffer[0] = FTP_CUST_PWR | FTP_CUST_RST_N; /*Set PWR and RST_N bits*/
    i2c_write(STUSB_Dev->interface, STUSB4500_SLAVE_ADDRESS, FTP_CTRL_0, Buffer, 1);

    // NVM Program Load Register to write with the 64-bit data to be written in sector
    Buffer[0] = (WRITE_PL & FTP_CUST_OPCODE); /*Set Write to PL Opcode*/
    i2c_write(STUSB_Dev->interface, STUSB4500_SLAVE_ADDRESS, FTP_CTRL_1, Buffer, 1);

    Buffer[0] = FTP_CUST_PWR | FTP_CUST_RST_N | FTP_CUST_REQ; /* Load Write to PL Sectors Opcode */
    i2c_write(STUSB_Dev->interface, STUSB4500_SLAVE_ADDRESS, FTP_CTRL_0, Buffer, 1);

    do
    {
        i2c_read(STUSB_Dev->interface, STUSB4500_SLAVE_ADDRESS, FTP_CTRL_0, Buffer, 1); /* Wait for execution */
    } while (Buffer[0] & FTP_CUST_REQ);                                                 // FTP_CUST_REQ clear by NVM controller

    Buffer[0] = (PROG_SECTOR & FTP_CUST_OPCODE);
    i2c_write(STUSB_Dev->interface, STUSB4500_SLAVE_ADDRESS, FTP_CTRL_1, Buffer, 1); /*Set Prog Sectors Opcode*/

    Buffer[0] = (SectorNum & FTP_CUST_SECT) | FTP_CUST_PWR | FTP_CUST_RST_N | FTP_CUST_REQ;
    i2c_write(STUSB_Dev->interface, STUSB4500_SLAVE_ADDRESS, FTP_CTRL_0, Buffer, 1); /* Load Prog Sectors Opcode */

    do
    {
        i2c_read(STUSB_Dev->interface, STUSB4500_SLAVE_ADDRESS, FTP_CTRL_0, Buffer, 1); /* Wait for execution */
    } while (Buffer[0] & FTP_CUST_REQ);                                                 // FTP_CUST_REQ clear by NVM controller

}

static void STUSB_ExitTestMode(struct STUSB4500_Dev *STUSB_Dev)
{
    unsigned char Buffer[2];

    Buffer[0] = FTP_CUST_RST_N;
    Buffer[1] = 0x00; /* clear registers */
    i2c_write(STUSB_Dev->interface, STUSB4500_SLAVE_ADDRESS, FTP_CTRL_0, Buffer, 2);
    Buffer[0] = 0x00;
    i2c_write(STUSB_Dev->interface, STUSB4500_SLAVE_ADDRESS, FTP_CUST_PASSWORD_REG, Buffer, 1); /* Clear Password */

}

syscon_error_t STUSB_getID(struct STUSB4500_Dev *STUSB_Dev)
{
    uint8_t rw_buffer[1] = {0};

    if (STUSB_get_regs(STUSB_Dev, CHIP_ID, rw_buffer, 1) != ERROR_OK)
        return STUSB_FAILED_TO_GET_CHIP_ID;

    STUSB_Dev->STUSB_Chip_ID = STUSB_GET_BITS(rw_buffer[0], DEVICE_ID);

#if defined(STUSB_DEBUG)
    printf("STUSB device ID: %x\r\n", STUSB_Dev->STUSB_Chip_ID);
#endif
    return ERROR_OK;
}

syscon_error_t STUSB_get_RDO(struct STUSB4500_Dev *STUSB_Dev)
{
    uint8_t rw_buffer[4] = {0};

    /*reading the config registers for modification*/
    if (STUSB_get_regs(STUSB_Dev, RDO_REG_STATUS_0, rw_buffer, 4) != ERROR_OK)
        return STUSB_FAILED_TO_GET_RDO;

    STUSB_Dev->RDO_status.object_position = STUSB_GET_BITS(rw_buffer[3], RDO_OBJECT_POSITION);
    /*current needs to be multiplied by 10 to get real current; without multiplying it can be used directly in the BQ25792; same for max_current*/
    STUSB_Dev->RDO_status.operating_current_mA = ((uint16_t)STUSB_GET_BITS(rw_buffer[1], RDO_CURRENT_L) | ((uint16_t)STUSB_GET_BITS(rw_buffer[2], RDO_CURRENT_H) << 6));

    if (STUSB_Dev->RDO_status.object_position == 0)
    {
        STUSB_get_current_at_5v(STUSB_Dev);
        STUSB_Dev->RDO_status.operating_current_mA = STUSB_Dev->input_current;
    }

    STUSB_Dev->RDO_status.max_current_mA = ((uint16_t)STUSB_GET_BITS(rw_buffer[0], RDO_MAXCURRENT_L) | ((uint16_t)STUSB_GET_BITS(rw_buffer[1], RDO_MAXCURRENT_H) << 8));
    STUSB_Dev->RDO_status.unchunked_msg_supported = STUSB_GET_BITS(rw_buffer[2], RDO_UNCHUNKED_MSG_SUPPORTED);
    STUSB_Dev->RDO_status.no_usb_suspend = STUSB_GET_BITS(rw_buffer[3], RDO_NO_USB_SUSPEND);
    STUSB_Dev->RDO_status.usb_comm_capable = STUSB_GET_BITS(rw_buffer[3], RDO_USB_COMM_CAPABLE);
    STUSB_Dev->RDO_status.capability_mismatch = STUSB_GET_BITS(rw_buffer[3], RDO_CAPABILITY_MISMATCH);
    STUSB_Dev->RDO_status.giveback_flag = STUSB_GET_BITS(rw_buffer[3], RDO_GIVEBACK_FLAG);

#if defined(STUSB_DEBUG)
    printf("----------------------------------------------\r\n");
    printf("STUSB RDO:\r\n");
    printf("object position: %u\r\n", STUSB_Dev->RDO_status.object_position);
    printf("operating current: %u\r\n", STUSB_Dev->RDO_status.operating_current_mA);
    printf("max current: %u\r\n", STUSB_Dev->RDO_status.max_current_mA);
    printf("unchunked msg support: %x\r\n", STUSB_Dev->RDO_status.unchunked_msg_supported);
    printf("no usb suspend: %x\r\n", STUSB_Dev->RDO_status.no_usb_suspend);
    printf("usb comm capable: %x\r\n", STUSB_Dev->RDO_status.usb_comm_capable);
    printf("capability mismatch: %x\r\n", STUSB_Dev->RDO_status.capability_mismatch);
    printf("giveback flag: %x\r\n", STUSB_Dev->RDO_status.giveback_flag);
    printf("----------------------------------------------\r\n");
#endif

    return ERROR_OK;
}

syscon_error_t STUSB_get_current_at_5v(struct STUSB4500_Dev *STUSB_Dev)
{
    uint8_t rw_buffer[2] = {0};

    // read & update CC Attachement status
    if (STUSB_get_regs(STUSB_Dev, CC_STATUS, rw_buffer, 1) != ERROR_OK)
        return STUSB_FAILED_TO_GET_RDO;

    STUSB_Dev->cc_status.looking_4_connection = STUSB_GET_BITS(rw_buffer[0], LOOKING_4_CONNECTION);
    STUSB_Dev->cc_status.connect_result = STUSB_GET_BITS(rw_buffer[0], CONNECT_RESULT);

    if ((STUSB_Dev->cc_status.looking_4_connection == STUSB4500_CC_NOT_LOOKING) && (STUSB_Dev->cc_status.connect_result == STUSB4500_PRESENT_RD))
    {
        do
        {
            // read & update CC Attachement status
            if (STUSB_get_regs(STUSB_Dev, CC_STATUS, rw_buffer, 1) != ERROR_OK)
                return STUSB_FAILED_TO_GET_RDO;
            STUSB_Dev->cc_status.looking_4_connection = STUSB_GET_BITS(rw_buffer[0], LOOKING_4_CONNECTION);
            STUSB_Dev->cc_status.connect_result = STUSB_GET_BITS(rw_buffer[0], CONNECT_RESULT);
            STUSB_Dev->cc_status.cc1_state = STUSB_GET_BITS(rw_buffer[0], CC1_STATE);
            STUSB_Dev->cc_status.cc2_state = STUSB_GET_BITS(rw_buffer[0], CC2_STATE);

        } while ((STUSB_Dev->cc_status.looking_4_connection == STUSB4500_CC_NOT_LOOKING) &&
                 (STUSB_Dev->cc_status.cc1_state == STUSB4500_CC_SNK_Open) &&
                 (STUSB_Dev->cc_status.cc2_state == STUSB4500_CC_SNK_Open));

        int UsedCCpin_state = -1;

        if ((STUSB_Dev->cc_status.cc1_state == STUSB4500_CC_SNK_Open) && (STUSB_Dev->cc_status.cc2_state != STUSB4500_CC_SNK_Open))
            UsedCCpin_state = STUSB_Dev->cc_status.cc2_state;
        if ((STUSB_Dev->cc_status.cc2_state == STUSB4500_CC_SNK_Open) && (STUSB_Dev->cc_status.cc1_state != STUSB4500_CC_SNK_Open))
            UsedCCpin_state = STUSB_Dev->cc_status.cc1_state;

        if (UsedCCpin_state != -1)
        {
            if (UsedCCpin_state == STUSB4500_CC_SNK_Default)
                STUSB_Dev->input_current = STUSB_CURRENT_500MA;
            else if (UsedCCpin_state == STUSB4500_CC_SNK_Power15A)
                STUSB_Dev->input_current = STUSB_CURRENT_1500MA;
            else if (UsedCCpin_state == STUSB4500_CC_SNK_Power30A)
                STUSB_Dev->input_current = STUSB_CURRENT_3000MA;
        }
    }
    else
        return STUSB_NO_USBC_ATTACHED;

    return ERROR_OK;
}

syscon_error_t STUSB_get_current_PDO_number(struct STUSB4500_Dev *STUSB_Dev)
{
    uint8_t rw_buffer[1] = {0};

    if (STUSB_get_regs(STUSB_Dev, DPM_PDO_NUMB, rw_buffer, 1) != ERROR_OK)
        return STUSB_FAILED_TO_GET_RDO;

    STUSB_Dev->current_PDO = STUSB_GET_BITS(rw_buffer[0], DPM_SNK_PDO_NUMB);

    return ERROR_OK;
}

syscon_error_t get_input_current_limit(struct STUSB4500_Dev *STUSB_Dev)
{
    syscon_error_t result;
    /*get the negotiated contract to update the charger configuration*/
    result = STUSB_get_RDO(STUSB_Dev);

  return result;
}
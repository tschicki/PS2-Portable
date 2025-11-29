#ifndef Error_Codes_h
#define Error_Codes_h
#include <stdio.h>

/// Get the number of elements in any C array
#define ARRAY_LEN(array) (sizeof(array) / sizeof((array)[0]))

/// @brief Error codes for syscon firmware
typedef enum syscon_error_e
{
    ERROR_OK = 0,
    GENERIC_ERROR,
    BQ_FAILED_TO_GET_CURRENT_CONFIG,  
    BQ_FAILED_TO_SET_NEW_CONFIG,         
    BQ_FAILED_TO_GET_CHIP_ID,            
    BQ_FAILED_TO_SET_CHARGE_EN,          
    BQ_WRONG_INPUT_PARAMETER,            
    BQ_FAILED_TO_SET_ADC_EN,             
    BQ_FAILED_TO_GET_CURRENT_ADC_CONFIG, 
    BQ_FAILED_TO_SET_CURRENT_ADC_CONFIG, 
    BQ_FAILED_TO_GET_ADC_READINGS,       
    BQ_FAILED_TO_GET_STATUS,             
    BQ_FAILED_TO_GET_FAULT_FLAGS,        
    BQ_FAILED_TO_RESET_REGS,             
    BQ_FAILED_TO_SET_INPUT_CURRENT_LIMIT,
    BQ_CONFIG_MISMATCH,
/*----------------------------------------------------------*/
    TEMP_ERROR,
/*STUSB4500-------------------------------------------------*/
    STUSB_FAILED_TO_SET_INT_MASKS, 
    STUSB_FAILED_TO_GET_INT_MASKS,  
    STUSB_FAILED_TO_CLEAR_INTERRUPTS,
    STUSB_FAILED_TO_GET_CURRENT_PDO, 
    STUSB_FAILED_TO_SET_NEW_PDO,     
    STUSB_FAILED_TO_GET_CHIP_ID,     
    STUSB_FAILED_TO_RENEGOTIATE,     
    STUSB_FAILED_TO_GET_RDO,         
    STUSB_NO_EXPLICIT_CONTRACT,      
    STUSB_NO_USBC_ATTACHED,          
/*MAX17320--------------------------------------------------*/
    MAX_FAILED_TO_UPDATE_NVCONFIG,
    MAX_FAILED_TO_WRITE_NVCONFIG, 
    MAX_SOFT_RESET_FAILED,        
    MAX_FULL_RESET_FAILED,        
    MAX_UNLOCK_FAILED,            
    MAX_FAILED_TO_GET_NVM_CYCLES, 
    MAX_NO_NVM_CYCLES_REMAINING,  
    MAX_FAILED_TO_GET_PROPERTIES, 
    MAX_FAILED_TO_SET_SHIPMODE,
    INVALID_STATE_CHANGE,	
    INVALID_STATE,			
/*I2C gamepad----------------------------------------------------*/
    AMP_FAILED_TO_CONFIGURE, 
    AMP_FAILED_TO_SET_HP_VOL,
    AMP_FAILED_TO_SET_LS_VOL,
    AMP_FAILED_TO_MUTE,   
    EXTIO_FAILED_TO_GET_INPUTS,
    EXTIO_FAILED_TO_SET_LED,
    EXTIO_FAILED_TO_SET_RUMBLE,
/*states*/
    ENTERED_UNDEFINED_STATE,

/*fpga*/
    FPGA_CONFIG_TIMEOUT,
    FPGA_CONFIG_FAILED,
    FPGA_COMMAND_FAILED,

/*menu*/
    ERROR_MENU_REFRESH,

/*general*/
    ERROR_FLASH_CONFIGS,

    /// Total # of errors in this list (NOT AN ACTUAL ERROR CODE);
    /// NOTE: that for this to work, it assumes your first error code is value 0 and you let it naturally 
    /// increment from there, as is done above, without explicitly altering any error values above
    SYSCON_ERROR_COUNT,
} syscon_error_t;

// Array of strings to map enum error types to printable strings
// - see important NOTE above!
//const char* const SYSCON_ERROR_STRS[] = 
static char* SYSCON_ERROR_STRS[] = 
{
    "ERROR_OK",
    "GENERIC_ERROR",
    "BQ_FAILED_TO_GET_CURRENT_CONFIG",  
    "BQ_FAILED_TO_SET_NEW_CONFIG",         
    "BQ_FAILED_TO_GET_CHIP_ID",            
    "BQ_FAILED_TO_SET_CHARGE_EN",          
    "BQ_WRONG_INPUT_PARAMETER",            
    "BQ_FAILED_TO_SET_ADC_EN",             
    "BQ_FAILED_TO_GET_CURRENT_ADC_CONFIG", 
    "BQ_FAILED_TO_SET_CURRENT_ADC_CONFIG", 
    "BQ_FAILED_TO_GET_ADC_READINGS",       
    "BQ_FAILED_TO_GET_STATUS",             
    "BQ_FAILED_TO_GET_FAULT_FLAGS",        
    "BQ_FAILED_TO_RESET_REGS",             
    "BQ_FAILED_TO_SET_INPUT_CURRENT_LIMIT",
    "BQ_CONFIG_MISMATCH",
/*----------------------------------------------------------*/
    "TEMP_ERROR",
/*STUSB4500-------------------------------------------------*/
    "STUSB_FAILED_TO_SET_INT_MASKS", 
    "STUSB_FAILED_TO_GET_INT_MASKS",  
    "STUSB_FAILED_TO_CLEAR_INTERRUPTS",
    "STUSB_FAILED_TO_GET_CURRENT_PDO", 
    "STUSB_FAILED_TO_SET_NEW_PDO",     
    "STUSB_FAILED_TO_GET_CHIP_ID",     
    "STUSB_FAILED_TO_RENEGOTIATE",     
    "STUSB_FAILED_TO_GET_RDO",         
    "STUSB_NO_EXPLICIT_CONTRACT",      
    "STUSB_NO_USBC_ATTACHED",          
/*MAX17320--------------------------------------------------*/
    "MAX_FAILED_TO_UPDATE_NVCONFIG",
    "MAX_FAILED_TO_WRITE_NVCONFIG", 
    "MAX_SOFT_RESET_FAILED",        
    "MAX_FULL_RESET_FAILED",        
    "MAX_UNLOCK_FAILED",            
    "MAX_FAILED_TO_GET_NVM_CYCLES", 
    "MAX_NO_NVM_CYCLES_REMAINING",  
    "MAX_FAILED_TO_GET_PROPERTIES", 
    "MAX_FAILED_TO_SET_SHIPMODE",
    "INVALID_STATE_CHANGE",	
    "INVALID_STATE",			
/*I2S DAC----------------------------------------------------*/
    "AMP_FAILED_TO_CONFIGURE", 
    "AMP_FAILED_TO_SET_HP_VOL",
    "AMP_FAILED_TO_SET_LS_VOL",
    "AMP_FAILED_TO_MUTE", 
    "EXTIO_FAILED_TO_GET_INPUTS",
    "EXTIO_FAILED_TO_SET_LED", 
    "EXTIO_FAILED_TO_SET_RUMBLE",
/*states*/
    "ENTERED UNDEFINED STATE",  
/*fpga*/
    "FPGA_CONFIG_TIMEOUT",
    "FPGA_CONFIG_FAILED",
    "FPGA_COMMAND_FAILED",
/*menu*/
    "ERROR_MENU_REFRESH",
/*general*/
    "ERROR_FLASH_CONFIGS",
};

_Static_assert(ARRAY_LEN(SYSCON_ERROR_STRS) == SYSCON_ERROR_COUNT,
    "You must keep your `syscon_error_t` enum and your "
    "`SYSCON_ERROR_STRS` array in-sync!");

// To get a printable error string
const char* syscon_error_str(syscon_error_t err);

#endif
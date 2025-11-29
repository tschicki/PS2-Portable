
#include "MCU_interface.h"
#include "flash.h"

static uint32_t flash_settings[] = {
    0,
    AX0_VIRTUAL_AXIS,
    AX0_INVERT,
    AX_DEADZONE,
    AX_CENTER,
    AX_MIN,
    AX_MAX,
    AX1_VIRTUAL_AXIS,
    AX1_INVERT,
    AX_DEADZONE,
    AX_CENTER,
    AX_MIN,
    AX_MAX,
    AX2_VIRTUAL_AXIS,
    AX2_INVERT,
    AX_DEADZONE,
    AX_CENTER,
    AX_MIN,
    AX_MAX,
    AX3_VIRTUAL_AXIS,
    AX3_INVERT,
    AX_DEADZONE,
    AX_CENTER,
    AX_MIN,
    AX_MAX,
    SPEAKER_VOLUME,
    HEADPHONE_VOLUME,
    DISP_BRIGHTNESS,
    FAN_TEMP_LOW_DEFAULT,
    FAN_TEMP_HIGH_DEFAULT, 
    /*start of 512i video cfg*/
    H_IMAGE_OFFSET_512i,    
    H_IMAGE_ACTIVE_512i,    
    H_ACTIVE_PXL_512i,      
    H_PACKED_PXL_512i,      
    V_IMAGE_OFFSET_512i,    
    V_IMAGE_ACTIVE_512i,    
    INTERLACE_512i,         
    SAMPLING_DIVIDER_512i,  
    DEINTERLACER_DEBUG_512i,
    MOTION_THRESHOLD_512i, 
    PHASE_512i, 
    SCALING_X_512i,
    SCALING_Y_512i,
    TARGET_RES_X_512i,
    TARGET_RES_Y_512i,
    /*start of 480i video cfg*/
    H_IMAGE_OFFSET_480i,    
    H_IMAGE_ACTIVE_480i,    
    H_ACTIVE_PXL_480i,      
    H_PACKED_PXL_480i,      
    V_IMAGE_OFFSET_480i,    
    V_IMAGE_ACTIVE_480i,    
    INTERLACE_480i,         
    SAMPLING_DIVIDER_480i,  
    DEINTERLACER_DEBUG_480i,
    MOTION_THRESHOLD_480i, 
    PHASE_480i, 
    SCALING_X_480i,
    SCALING_Y_480i,
    TARGET_RES_X_480i,
    TARGET_RES_Y_480i,
    /*start of 480p video cfg*/
    H_IMAGE_OFFSET_480p,    
    H_IMAGE_ACTIVE_480p,    
    H_ACTIVE_PXL_480p,      
    H_PACKED_PXL_480p,      
    V_IMAGE_OFFSET_480p,    
    V_IMAGE_ACTIVE_480p,    
    INTERLACE_480p,         
    SAMPLING_DIVIDER_480p,  
    DEINTERLACER_DEBUG_480p,
    MOTION_THRESHOLD_480p, 
    PHASE_480p,
    SCALING_X_480p,
    SCALING_Y_480p,
    TARGET_RES_X_480p,
    TARGET_RES_Y_480p,
    /*start of 256p video cfg*/
    H_IMAGE_OFFSET_256p,    
    H_IMAGE_ACTIVE_256p,    
    H_ACTIVE_PXL_256p,      
    H_PACKED_PXL_256p,      
    V_IMAGE_OFFSET_256p,    
    V_IMAGE_ACTIVE_256p,    
    INTERLACE_256p,         
    SAMPLING_DIVIDER_256p,  
    DEINTERLACER_DEBUG_256p,
    MOTION_THRESHOLD_256p, 
    PHASE_256p,
    SCALING_X_256p,
    SCALING_Y_256p,
    TARGET_RES_X_256p,
    TARGET_RES_Y_256p,
    /*start of 240p video cfg*/
    H_IMAGE_OFFSET_240p,    
    H_IMAGE_ACTIVE_240p,    
    H_ACTIVE_PXL_240p,      
    H_PACKED_PXL_240p,      
    V_IMAGE_OFFSET_240p,    
    V_IMAGE_ACTIVE_240p,    
    INTERLACE_240p,         
    SAMPLING_DIVIDER_240p,  
    DEINTERLACER_DEBUG_240p,
    MOTION_THRESHOLD_240p, 
    PHASE_240p,
    SCALING_X_240p,
    SCALING_Y_240p,
    TARGET_RES_X_240p,
    TARGET_RES_Y_240p,
    /*rumble*/
    RUMBLE_INTENSITY_DEF,
};

void flash_init(uint32_t **settings, const uint16_t numEntries, const uint8_t *flash_target_contents, uint32_t magic_number)
{
    *settings = &flash_settings[0]; 
    flash_read(*settings, flash_target_contents, 1);

    /*settings in flash are invalid or missing, so use the default*/
    /*should probably be entered the first time after flashing FW*/
    if(*settings[NO_MAGIC_NUMBER] != magic_number)
    {
        *settings[NO_MAGIC_NUMBER] = magic_number;
        return;
    }
    else
    {
        /*settings are valid, read them*/
        flash_read(*settings, flash_target_contents, numEntries);
    }

}

void flash_store(uint32_t *settings, const uint8_t *base_address, const uint16_t numEntries)
{
    static uint8_t rw_buffer[FLASH_PAGE_SIZE] = {0};
    uint8_t max_entries_per_page = (uint8_t)(FLASH_PAGE_SIZE >> 2); //this should usually be 64 in case of the RP2040
    uint8_t numPages = 0;
    uint32_t *settings_remaining = settings;
    uint16_t numEntries_remaining = numEntries;
    uint16_t numEntries_fill = 0;
    uint32_t flash_page_address = FLASH_TARGET_OFFSET;
    /*safety step to limit total chaos*/
    if(numEntries > MAX_SETTING_COUNT) return;  

    /*first find out how many pages are needed*/
    if(numEntries > max_entries_per_page)
        numPages = ((numEntries / max_entries_per_page) + 1);
    else
        numPages = 1;
    
    /*compare the whole setting array with flash to find a mismatch*/
    //if(flash_compare(settings, base_address, numEntries))
    //{
        /*some setting changed during runtime, so store them*/
        //printf("SETTINGS CHANGED - STORING\n");
    	/*erase the whole sector first - 4096 bytes*/
        flash_range_erase(FLASH_TARGET_OFFSET, FLASH_SECTOR_SIZE);

        for(uint8_t i=0; i < numPages; i++){

            /*first make sure that all pages are fully filled and that the remainings are written to the last page*/
            if(numEntries_remaining > max_entries_per_page){
                numEntries_remaining = numEntries_remaining - max_entries_per_page;
                numEntries_fill = max_entries_per_page;
            }
            else numEntries_fill = numEntries_remaining;

            /*for each pass we fill the 256 byte rw buffer with the respective settings*/
            fill_rw_buffer(settings_remaining, rw_buffer, numEntries_fill);
            //page size is 256 bytes, but buffer has only 124 bytes -> hardfault?
            flash_range_program(flash_page_address, rw_buffer, FLASH_PAGE_SIZE);
            /*increment to the next page of settings and to the next page of flash*/
            settings_remaining = settings_remaining + max_entries_per_page; //FLASH_PAGE_SIZE; /*increment settings pointer by 64 entries (256 bytes)*/
            flash_page_address = flash_page_address + FLASH_PAGE_SIZE;
        }
    //}
}


void flash_read(uint32_t *settings, const uint8_t *base_address, uint16_t readAmount)
{
    /*read the flash contents and write them into the struct*/
    for(uint16_t i = 0; i < (readAmount*4); i = i+4)
    {
        settings[i/4] = (((uint32_t)base_address[i+3]) << 24) | (((uint32_t)base_address[i+2]) << 16) | (((uint32_t)base_address[i+1]) << 8) | (uint32_t)base_address[i];
    }
}

void fill_rw_buffer(uint32_t *settings, uint8_t *buffer, uint16_t storeAmount)
{
    for(uint16_t i = 0; i < (storeAmount*4); i = i+4)
    {
        buffer[i] = (uint8_t)settings[i/4];
        buffer[i+1] = (uint8_t)(settings[i/4] >> 8);
        buffer[i+2] = (uint8_t)(settings[i/4] >> 16);
        buffer[i+3] = (uint8_t)(settings[i/4] >> 24);
    }

}

uint8_t flash_compare(uint32_t *settings, const uint8_t *base_address, uint16_t readAmount)
{
    uint32_t setting_flash = 0;
    uint8_t setting_mismatch = 0;
    /*read the flash contents and write them into the struct*/
    for(uint16_t i = 0; i < (readAmount*4); i = i+4)
    {
        setting_flash = (((uint32_t)base_address[i+3]) << 24) | (((uint32_t)base_address[i+2]) << 16) | (((uint32_t)base_address[i+1]) << 8) | (uint32_t)base_address[i];
        if(settings[i/4] != setting_flash)
            setting_mismatch = 1;
    }

    return setting_mismatch;
}

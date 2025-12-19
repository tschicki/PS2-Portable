#ifndef FLASH_H
#define FLASH_H

// We're going to erase and reprogram a region 500 sectors (of 4096 bytes) from the start of flash.
// Once done, we can access this at XIP_BASE + FLASH_TARGET_OFFSET.
#define FLASH_TARGET_OFFSET (450 * 4096)

#define MAGIC_NUMBER               1234

#define AX_DEADZONE                30
#define AX_CENTER                  2048
#define AX_MAX                     4096
#define AX_MIN                     0

#define AX0_INVERT                  FALSE
#define AX1_INVERT                  FALSE
#define AX2_INVERT                  TRUE
#define AX3_INVERT                  FALSE

#define AX0_VIRTUAL_AXIS            0
#define AX1_VIRTUAL_AXIS            1
#define AX2_VIRTUAL_AXIS            1
#define AX3_VIRTUAL_AXIS            0

#define SPEAKER_VOLUME              10
#define HEADPHONE_VOLUME            10
#define DISP_BRIGHTNESS             100
#define FAN_TEMP_LOW_DEFAULT        25
#define FAN_TEMP_HIGH_DEFAULT       40

/*applies to the interlaced resolutions and 240p/256p:
- first determine the sampling divider
- set horizontal resolution based on the following:
    sampling divider -> H_ACTIVE_PXL
    3->640; 
    4->512; 
    5->448; 
    6->384; 
    7->320; 
    8->undefined?; 
    9->256
- then calculate the horizontal resolution H_IMAGE_ACTIVE: H_ACTIVE_PXL * (sampling divider + 1)
- then calculate the maximum horizontal offset: PAL -> 3456 - H_IMAGE_ACTIVE; NTSC -> 3432 - H_IMAGE_ACTIVE
- then calculate the packed pixels: H_ACTIVE_PXL * 0,75 -> result must be an integer, so H_ACTIVE_PXL must be a multiple of 4
- then set the max phase to  
*/

/*video setting default*/
#define H_IMAGE_OFFSET_512i         412     //MAX: 3456 - H_IMAGE_ACTIVE
#define H_IMAGE_ACTIVE_512i         2560    //H_ACTIVE_PXL * (sampling divider + 1)
#define H_ACTIVE_PXL_512i           640
#define H_PACKED_PXL_512i           480     //H_ACTIVE_PXL * 0,75 -> RESULT MUST BE AN INTEGER!!!!!
#define V_IMAGE_OFFSET_512i         33
#define V_IMAGE_ACTIVE_512i         256
#define INTERLACE_512i              1
#define SAMPLING_DIVIDER_512i       3       //horizontal resolutions: 3->640; 4->512; 5->448; 6->384; 7->320; 8->undefined?; 9->256
#define DEINTERLACER_DEBUG_512i     0
#define MOTION_THRESHOLD_512i       15
#define PHASE_512i                  0
#define SCALING_X_512i              0x0000665e
#define SCALING_Y_512i              0x0000888D
#define TARGET_RES_X_512i           0x00000320
#define TARGET_RES_Y_512i           0x000001E0

#define H_IMAGE_OFFSET_256p         412     //MAX: 3456 - H_IMAGE_ACTIVE
#define H_IMAGE_ACTIVE_256p         2560    //H_ACTIVE_PXL * (sampling divider + 1)
#define H_ACTIVE_PXL_256p           640
#define H_PACKED_PXL_256p           480     //H_ACTIVE_PXL * 0,75 -> RESULT MUST BE AN INTEGER!!!!!
#define V_IMAGE_OFFSET_256p         33
#define V_IMAGE_ACTIVE_256p         256
#define INTERLACE_256p              2
#define SAMPLING_DIVIDER_256p       3       //horizontal resolutions: 3->640; 4->512; 5->448; 6->384; 7->320; 8->undefined?; 9->256
#define DEINTERLACER_DEBUG_256p     0
#define MOTION_THRESHOLD_256p       15
#define PHASE_256p                  0
#define SCALING_X_256p              0x0000665e
#define SCALING_Y_256p              0x0000888D
#define TARGET_RES_X_256p           0x00000320
#define TARGET_RES_Y_256p           0x000001E0

#define H_IMAGE_OFFSET_480i         392
#define H_IMAGE_ACTIVE_480i         2560
#define H_ACTIVE_PXL_480i           640
#define H_PACKED_PXL_480i           480
#define V_IMAGE_OFFSET_480i         22
#define V_IMAGE_ACTIVE_480i         224
#define INTERLACE_480i              1
#define SAMPLING_DIVIDER_480i       3
#define DEINTERLACER_DEBUG_480i     0
#define MOTION_THRESHOLD_480i       15
#define PHASE_480i                  1
#define SCALING_X_480i              0x0000665e//0x00008000;
#define SCALING_Y_480i              0x00007772
#define TARGET_RES_X_480i           0x00000320 //280
#define TARGET_RES_Y_480i           0x000001E0

#define H_IMAGE_OFFSET_240p         392
#define H_IMAGE_ACTIVE_240p         2560
#define H_ACTIVE_PXL_240p           640
#define H_PACKED_PXL_240p           480
#define V_IMAGE_OFFSET_240p         22
#define V_IMAGE_ACTIVE_240p         224
#define INTERLACE_240p              2
#define SAMPLING_DIVIDER_240p       3
#define DEINTERLACER_DEBUG_240p     0
#define MOTION_THRESHOLD_240p       15
#define PHASE_240p                  1
#define SCALING_X_240p              0x0000665e//0x00008000;
#define SCALING_Y_240p              0x00007772
#define TARGET_RES_X_240p           0x00000320 //280
#define TARGET_RES_Y_240p           0x000001E0

#define H_IMAGE_OFFSET_480p         128
#define H_IMAGE_ACTIVE_480p         1280
#define H_ACTIVE_PXL_480p           640
#define H_PACKED_PXL_480p           480
#define V_IMAGE_OFFSET_480p         4
#define V_IMAGE_ACTIVE_480p         480
#define INTERLACE_480p              0
#define SAMPLING_DIVIDER_480p       1
#define DEINTERLACER_DEBUG_480p     0
#define MOTION_THRESHOLD_480p       15
#define PHASE_480p                  1
#define SCALING_X_480p              0x00008000
#define SCALING_Y_480p              0x00008000
#define TARGET_RES_X_480p           0x00000280
#define TARGET_RES_Y_480p           0x000001E0

/*rumble*/
#define RUMBLE_INTENSITY_DEF        3  /*can be 1 - 10*/

/*MAGH Autodetect*/
#define MAGH_AUTODETECT_SET         1


#define MAX_SETTING_COUNT           1024 /*16 pages per sector of 4064 and 64 settings per page (32 bit): 64*16 = 1024*/

typedef enum flash_settings_e
{
    NO_MAGIC_NUMBER = 0,
    NO_AX0_VIRTUAL_AXIS,
    NO_AX0_INVERT,
    NO_AX0_DEADZONE,
    NO_AX0_CENTER,
    NO_AX0_MIN,
    NO_AX0_MAX,
    NO_AX1_VIRTUAL_AXIS,
    NO_AX1_INVERT,
    NO_AX1_DEADZONE,
    NO_AX1_CENTER,
    NO_AX1_MIN,
    NO_AX1_MAX,
    NO_AX2_VIRTUAL_AXIS,
    NO_AX2_INVERT,
    NO_AX2_DEADZONE,
    NO_AX2_CENTER,
    NO_AX2_MIN,
    NO_AX2_MAX,
    NO_AX3_VIRTUAL_AXIS,
    NO_AX3_INVERT,
    NO_AX3_DEADZONE,
    NO_AX3_CENTER,
    NO_AX3_MIN,
    NO_AX3_MAX,
    NO_SPEAKER_VOLUME,
    NO_HEADPHONE_VOLUME,
    NO_DISP_BRIGHTNESS,
    NO_FAN_TEMP_LOW,
    NO_FAN_TEMP_HIGH,
    
    NO_H_IMAGE_OFFSET_512i,    
    NO_H_IMAGE_ACTIVE_512i,    
    NO_H_ACTIVE_PXL_512i,      
    NO_H_PACKED_PXL_512i,      
    NO_V_IMAGE_OFFSET_512i,    
    NO_V_IMAGE_ACTIVE_512i,    
    NO_INTERLACE_512i,         
    NO_SAMPLING_DIVIDER_512i,  
    NO_DEINTERLACER_DEBUG_512i,
    NO_MOTION_THRESHOLD_512i, 
    NO_PHASE_512i,
    NO_SCALING_X_512i,
    NO_SCALING_Y_512i,
    NO_TARGET_RES_X_512i,
    NO_TARGET_RES_Y_512i,

    NO_H_IMAGE_OFFSET_480i,    
    NO_H_IMAGE_ACTIVE_480i,    
    NO_H_ACTIVE_PXL_480i,      
    NO_H_PACKED_PXL_480i,      
    NO_V_IMAGE_OFFSET_480i,    
    NO_V_IMAGE_ACTIVE_480i,    
    NO_INTERLACE_480i,         
    NO_SAMPLING_DIVIDER_480i,  
    NO_DEINTERLACER_DEBUG_480i,
    NO_MOTION_THRESHOLD_480i,  
    NO_PHASE_480i,
    NO_SCALING_X_480i,
    NO_SCALING_Y_480i,
    NO_TARGET_RES_X_480i,
    NO_TARGET_RES_Y_480i,
    
    NO_H_IMAGE_OFFSET_480p,    
    NO_H_IMAGE_ACTIVE_480p,    
    NO_H_ACTIVE_PXL_480p,      
    NO_H_PACKED_PXL_480p,      
    NO_V_IMAGE_OFFSET_480p,    
    NO_V_IMAGE_ACTIVE_480p,    
    NO_INTERLACE_480p,         
    NO_SAMPLING_DIVIDER_480p,  
    NO_DEINTERLACER_DEBUG_480p,
    NO_MOTION_THRESHOLD_480p, 
    NO_PHASE_480p, 
    NO_SCALING_X_480p,
    NO_SCALING_Y_480p,
    NO_TARGET_RES_X_480p,
    NO_TARGET_RES_Y_480p,

    NO_H_IMAGE_OFFSET_256p,    
    NO_H_IMAGE_ACTIVE_256p,    
    NO_H_ACTIVE_PXL_256p,      
    NO_H_PACKED_PXL_256p,      
    NO_V_IMAGE_OFFSET_256p,    
    NO_V_IMAGE_ACTIVE_256p,    
    NO_INTERLACE_256p,         
    NO_SAMPLING_DIVIDER_256p,  
    NO_DEINTERLACER_DEBUG_256p,
    NO_MOTION_THRESHOLD_256p, 
    NO_PHASE_256p, 
    NO_SCALING_X_256p,
    NO_SCALING_Y_256p,
    NO_TARGET_RES_X_256p,
    NO_TARGET_RES_Y_256p,

    NO_H_IMAGE_OFFSET_240p,    
    NO_H_IMAGE_ACTIVE_240p,    
    NO_H_ACTIVE_PXL_240p,      
    NO_H_PACKED_PXL_240p,      
    NO_V_IMAGE_OFFSET_240p,    
    NO_V_IMAGE_ACTIVE_240p,    
    NO_INTERLACE_240p,         
    NO_SAMPLING_DIVIDER_240p,  
    NO_DEINTERLACER_DEBUG_240p,
    NO_MOTION_THRESHOLD_240p, 
    NO_PHASE_240p, 
    NO_SCALING_X_240p,
    NO_SCALING_Y_240p,
    NO_TARGET_RES_X_240p,
    NO_TARGET_RES_Y_240p,
    
    NO_RUMBLE_INTENSITY,
    NO_MAGH_AUTODETECT,

    SETTINGS_COUNT,
} flash_settings_t;

void flash_init(uint32_t **settings, const uint16_t numEntries, const uint8_t *flash_target_contents, uint32_t magic_number);
void fill_rw_buffer(uint32_t *settings, uint8_t *buffer, uint16_t storeAmount);
void flash_store(uint32_t *settings, const uint8_t *base_address, const uint16_t numEntries);
void flash_read(uint32_t *settings, const uint8_t *base_address, uint16_t readAmount);
uint8_t flash_compare(uint32_t *settings, const uint8_t *base_address, uint16_t readAmount);

#endif
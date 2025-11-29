#ifndef FPGA_REGISTERS_H
#define FPGA_REGISTERS_H

/*SPI INTERFACE------------------------------------------------------*/
/*lower level command stuff*/
#define SPI_PACKET_SIZE_BITSTREAM       8
#define SPI_PACKET_SIZE_FONT            8
/*FPGA SPI commands*/
#define CMD_WRITE_REG                   0x01
#define CMD_READ_REG                    0x02

/*banks supported by the FPGA*/
#define RANGE_VIDEO_CONFIG              0x1000
#define RANGE_VIDEO_REGS                0x2000
#define RANGE_SCALER                    0x3000  /*currently not in use*/
#define RANGE_BRIGHTNESS                0x4000  /*brightness adjustment*/
#define RANGE_SPRITE_REG                0x5000
#define RANGE_SPRITE_BUF                0x6000
#define RANGE_SPRITE_RAM                0x7000
/*sprite register addresses*/
#define REG_POSX_LSB                    RANGE_SPRITE_REG + 0
#define REG_POSX_MSB                    RANGE_SPRITE_REG + 1
#define REG_POSY_LSB                    RANGE_SPRITE_REG + 2
#define REG_POSY_MSB                    RANGE_SPRITE_REG + 3
#define REG_COLORA_LSB                  RANGE_SPRITE_REG + 4
#define REG_COLORA_MSB                  RANGE_SPRITE_REG + 5
#define REG_COLORB_LSB                  RANGE_SPRITE_REG + 6
#define REG_COLORB_MSB                  RANGE_SPRITE_REG + 7
#define REG_SCALE                       RANGE_SPRITE_REG + 8
/*video config base addresses for the different resolutions*/
#define REG_SETTINGS_512i               RANGE_VIDEO_CONFIG + 0
#define REG_SETTINGS_480i               RANGE_VIDEO_CONFIG + 32
#define REG_SETTINGS_480p               RANGE_VIDEO_CONFIG + 64
#define REG_SETTINGS_256p               RANGE_VIDEO_CONFIG + 96
#define REG_SETTINGS_240p               RANGE_VIDEO_CONFIG + 128
/*control registers for the video input*/
/*this register returns the currently displayed resolution*/
#define REG_RESOLUTION_INFO             RANGE_VIDEO_REGS + 0
/*this one contains the video_enable flag and config_override flag*/
#define REG_VIDEO_CONTROL               RANGE_VIDEO_REGS + 1
/*gateware version*/
#define REG_VERSION_MAJOR               RANGE_VIDEO_REGS + 2
#define REG_VERSION_MINOR               RANGE_VIDEO_REGS + 3
/*registers for changing the display brightness*/
#define REG_BRIGHTNESS_MAX_COUNT        RANGE_BRIGHTNESS + 0
#define REG_BRIGHTNESS_TRIG_COUNT       RANGE_BRIGHTNESS + 1
/*Video config offsets for each resolution, must be calculated like this: REG_SETTINGS_512i + OFFSET_H_IMAGE_OFFSET*/
typedef enum video_config_buffer
{
    OFFSET_H_IMAGE_OFFSET_LSB = 0,
    OFFSET_H_IMAGE_OFFSET_MSB,
    OFFSET_H_IMAGE_ACTIVE_LSB,
    OFFSET_H_IMAGE_ACTIVE_MSB,
    OFFSET_H_ACTIVE_PXL_LSB,  
    OFFSET_H_ACTIVE_PXL_MSB,  
    OFFSET_H_PACKED_PXL_LSB,  
    OFFSET_H_PACKED_PXL_MSB,  
    OFFSET_V_IMAGE_OFFSET,    
    OFFSET_V_IMAGE_ACTIVE_LSB,
    OFFSET_V_IMAGE_ACTIVE_MSB,
    OFFSET_INTERLACE_DIVIDE,  
    OFFSET_DEINTERLACER,      
    OFFSET_SCALE_X_LSB,       
    OFFSET_SCALE_X_MID,       
    OFFSET_SCALE_X_MSB,       
    OFFSET_SCALE_Y_LSB,       
    OFFSET_SCALE_Y_MID,       
    OFFSET_SCALE_Y_MSB,       
    OFFSET_TARGET_RES_X_LSB,  
    OFFSET_TARGET_RES_X_MSB,  
    OFFSET_TARGET_RES_Y_LSB,  
    OFFSET_TARGET_RES_Y_MSB,  

    VIDEO_CONFIG_BUFFER_LENGTH


}video_config_buffer_t;



#endif
#ifndef FPGA_H
#define FPGA_H
#include "MCU_interface.h"
#include "FPGA_Registers.h"
#include "flash.h"
#include "helper_functions.h"
/*SPI INTERFACE------------------------------------------------------*/
/*lower level command stuff*/
#define SPI_PACKET_SIZE_BITSTREAM       8
#define SPI_PACKET_SIZE_FONT            8
/*FPGA SPI commands*/
#define CMD_WRITE_REG                   0x01
#define CMD_READ_REG                    0x02

/*SPRITE DEFINES------------------------------------------------------*/
#define SPRITE_DEFAULT_SCALE             1   //0 = 8x8 sprites; 1 = 16x16 sprites; everything else is untested...
#define COLOR_FONT_DESEL                 0
#define COLOR_BACK_DESEL                 1
#define COLOR_FONT_SEL                   2
#define COLOR_BACK_SEL                   3
#define COLOR_LSB                        0
#define COLOR_MSB                        1
/*VIDEO CONFIG DEFINES-------------------------------------------------------*/
/*these specify whether to load the settings from flash or store them*/
#define RES_LOAD                         0
#define RES_STORE                        1
/*possible returns when reading resolution register*/
#define RETURN_512i                      0
#define RETURN_480i                      32
#define RETURN_480p                      64
#define RETURN_256p                      96
#define RETURN_240p                      128

/*BACKLIGHT DEFINES-------------------------------------------------------*/
#define BACKLIGHT_MAX_BRIGHTNESS        100
#define BACKLIGHT_MAX_COUNT             BACKLIGHT_MAX_BRIGHTNESS*2     //pwm frequency = 150MHz / (BACKLIGHT_MAX_COUNT<<4); with 200 it is about 46kHz

/*commands for various updating functions*/
#define UPDATE_X                        0
#define UPDATE_Y                        1
#define RES_IN_X                        0
#define RES_IN_Y                        1
#define RES_OUT_X                       2
#define RES_OUT_Y                       3

/*amount of video settings per resolution*/
typedef enum video_settings
{
    NO_H_IMAGE_OFFSET = 0,    
    NO_H_IMAGE_ACTIVE,    
    NO_H_ACTIVE_PXL,      
    NO_H_PACKED_PXL,      
    NO_V_IMAGE_OFFSET,    
    NO_V_IMAGE_ACTIVE,    
    NO_INTERLACE,         
    NO_SAMPLING_DIVIDER,  
    NO_DEINTERLACER_DEBUG,
    NO_MOTION_THRESHOLD,
    NO_PHASE,
    NO_SCALING_X,
    NO_SCALING_Y,
    NO_TARGET_RES_X,
    NO_TARGET_RES_Y,

    /*this must always be the last one in the list*/
    NO_VIDEO_SETTINGS, 
}video_settings_t;


typedef enum video_resolutions
{
    RES_512i = 0,    
    RES_480i,    
    RES_480p, 
    RES_256p,
    RES_240p,     

    /*this must always be the last one in the list*/
    NO_RESOLUTIONS, 
}video_resolutions_t;

struct fpga_dev
{
    /*SPI interface pointer*/
    spi_inst_t *interface;
    /*version*/
    uint8_t version_major;
    uint8_t version_minor;
    /*fpga settings*/
    uint8_t brightness;
    uint8_t font_color[4][2];
    uint32_t video_config[NO_RESOLUTIONS][NO_VIDEO_SETTINGS]; //[NO_RESOLUTIONS][NO_VIDEO_SETTINGS];
    uint8_t scale;
    uint16_t current_resolution;
    uint16_t current_spritebuffer_addr;

};

/*PRIVATE functions*/
static syscon_error_t fpga_transmit_bitstream(struct fpga_dev *fpga_dev, struct SysCon_Pins *io);
static syscon_error_t fpga_transmit_font(struct fpga_dev *fpga_dev, struct SysCon_Pins *io);
static syscon_error_t fpga_copy_resolution_cfg(struct fpga_dev *fpga_dev, uint8_t resolution, uint32_t *flash_setting, uint16_t flash_setting_base, uint8_t load_store);
static syscon_error_t fpga_lock_config_update(struct fpga_dev *fpga_dev, struct SysCon_Pins *io, uint8_t endi);
static void fpga_fill_cfg_buffer(uint8_t *data_buffer, uint32_t *video_config);
static void fpga_calculate_scale(struct fpga_dev *fpga_dev, uint16_t resolution_to_modify, uint32_t *scale_x_out, uint32_t *scale_y_out);
static void fpga_calculate_x_params(struct fpga_dev *fpga_dev, uint16_t resolution_to_modify, uint32_t *h_image_active_out, uint32_t *h_packed_pxl_out, uint32_t *h_offset_out);

/*PUBLIC functions*/
syscon_error_t fpga_init(struct fpga_dev *fpga_dev, struct SysCon_Pins *io);
syscon_error_t fpga_write_address(struct fpga_dev *fpga_dev, struct SysCon_Pins *io, uint16_t address, uint8_t *tx_buffer, uint8_t len);
syscon_error_t fpga_erase_data(struct fpga_dev *fpga_dev, struct SysCon_Pins *io, uint16_t address, uint8_t len);
syscon_error_t fpga_read_address(struct fpga_dev *fpga_dev, struct SysCon_Pins *io, uint16_t address, uint8_t *return_byte);
syscon_error_t fpga_copy_video_cfg(struct fpga_dev *fpga_dev, uint32_t *flash_setting, uint8_t load_store);
syscon_error_t fpga_reload_video_cfg(struct fpga_dev *fpga_dev, struct SysCon_Pins *io, uint16_t fpga_res_base_address, uint32_t *video_config);

syscon_error_t fpga_video_endi(struct fpga_dev *fpga_dev, struct SysCon_Pins *io, uint8_t endi);
syscon_error_t fpga_apply_video_setting(struct fpga_dev *fpga_dev, struct SysCon_Pins *io, uint16_t resolution, uint16_t offset, uint8_t *tx_buffer, uint8_t len);

syscon_error_t fpga_get_version(struct fpga_dev *fpga_dev, struct SysCon_Pins *io, uint8_t *version_major, uint8_t *version_minor);
syscon_error_t fpga_get_current_resolution(struct fpga_dev *fpga_dev, struct SysCon_Pins *io, uint16_t *resolution);
syscon_error_t fpga_set_cursor(struct fpga_dev *fpga_dev, struct SysCon_Pins *io, uint16_t x, uint16_t y);
syscon_error_t fpga_set_image_offset(struct fpga_dev *fpga_dev, struct SysCon_Pins *io, uint8_t direction, uint8_t axis);
syscon_error_t fpga_set_phase(struct fpga_dev *fpga_dev, struct SysCon_Pins *io, uint8_t direction);
syscon_error_t fpga_toggle_deinterlacer_debug(struct fpga_dev *fpga_dev, struct SysCon_Pins *io);
syscon_error_t fpga_set_motion_threshold(struct fpga_dev *fpga_dev, struct SysCon_Pins *io, uint8_t direction);
syscon_error_t fpga_set_brightness(struct fpga_dev *fpga_dev, struct SysCon_Pins *io, uint8_t direction);
syscon_error_t fpga_set_xy_resolution(struct fpga_dev *fpga_dev, struct SysCon_Pins *io, uint8_t direction, uint8_t axis);
syscon_error_t fpga_set_sampling_divider(struct fpga_dev *fpga_dev, struct SysCon_Pins *io, uint8_t direction);




#endif
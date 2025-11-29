#include "fpga.h"

uint8_t __in_flash() bitstream[]= {
#include "video_processor_bitstream.h"
0x00
};

uint8_t __in_flash() font8x8_basic[][8]= {
#include "font8x8_basic.h"
#include "font8x8_control.h"
#include "font8x8_ext_latin.h"
};

const uint32_t bitstream_size = sizeof(bitstream);
const uint16_t font_size = sizeof(font8x8_basic)/8;


syscon_error_t fpga_init(struct fpga_dev *fpga_dev, struct SysCon_Pins *io)
{
    /*assign interface pointer to SPI*/
    fpga_dev->interface = FPGA_SPI;
    /*initialize the scale of the menu to default; THIS IS HARDCODED*/
    fpga_dev->scale = SPRITE_DEFAULT_SCALE;
    /*the address must be initialized to 0; this is the address an entry is written to in the FPGA sprite RAM*/
    fpga_dev->current_spritebuffer_addr = 0;
    /*4 bit per color when LSB & MSB combined, top 4 bit in MSB unused: UUUU BBBB GGGG RRRR*/
    /*colors for unselected entry*/
    fpga_dev->font_color[COLOR_FONT_DESEL][COLOR_LSB] =0xFF;
    fpga_dev->font_color[COLOR_FONT_DESEL][COLOR_MSB] =0xFF;
    fpga_dev->font_color[COLOR_BACK_DESEL][COLOR_LSB] =0x00;
    fpga_dev->font_color[COLOR_BACK_DESEL][COLOR_MSB] =0x00;
    /*colors for selected entry*/
    fpga_dev->font_color[COLOR_FONT_SEL][COLOR_LSB] =0xF0;
    fpga_dev->font_color[COLOR_FONT_SEL][COLOR_MSB] =0xFF;
    fpga_dev->font_color[COLOR_BACK_SEL][COLOR_LSB] =0x00;
    fpga_dev->font_color[COLOR_BACK_SEL][COLOR_MSB] =0x00;
    
    /*load the bitstream to the FPGA, this takes a while*/
    if(fpga_transmit_bitstream(fpga_dev, io) != ERROR_OK) 
        return FPGA_CONFIG_FAILED;
    /*this delay is needed, otherwise the FPGA SPI slave may not react*/
    delay_ms(100);
    /*this loads the font "font8x8_basic" into the sprite RAM of the FPGA*/
    if(fpga_transmit_font(fpga_dev, io) != ERROR_OK) 
        return FPGA_COMMAND_FAILED;
    /*set the default text scale from here on*/
    if(fpga_write_address(fpga_dev, io, REG_SCALE, &fpga_dev->scale, 1) != ERROR_OK)
        return FPGA_COMMAND_FAILED;
    /*load the initial backlight brightness*/
    if(fpga_set_brightness(fpga_dev, io, ACTION_RIGHT) != ERROR_OK)
        return FPGA_COMMAND_FAILED;
    /*load the default video configurations for all resolutions*/
    if(fpga_reload_video_cfg(fpga_dev, io, RES_512i, fpga_dev->video_config[RES_512i]) != ERROR_OK)
        return FPGA_COMMAND_FAILED;
    if(fpga_reload_video_cfg(fpga_dev, io, RES_480i, fpga_dev->video_config[RES_480i]) != ERROR_OK)
        return FPGA_COMMAND_FAILED;
    if(fpga_reload_video_cfg(fpga_dev, io, RES_480p, fpga_dev->video_config[RES_480p]) != ERROR_OK)
        return FPGA_COMMAND_FAILED;
    if(fpga_reload_video_cfg(fpga_dev, io, RES_256p, fpga_dev->video_config[RES_256p]) != ERROR_OK)
        return FPGA_COMMAND_FAILED;
    if(fpga_reload_video_cfg(fpga_dev, io, RES_240p, fpga_dev->video_config[RES_240p]) != ERROR_OK)
        return FPGA_COMMAND_FAILED;
    
    /*enable the video processor, must be done last at startup, AFTER writing all resolutions and configs*/
    if(fpga_video_endi(fpga_dev, io, ENABLE) != ERROR_OK)
        return FPGA_COMMAND_FAILED;
    /*read the gateware version, only need to do this once...*/
    if(fpga_get_version(fpga_dev, io, &fpga_dev->version_major, &fpga_dev->version_minor) != ERROR_OK)
        return FPGA_COMMAND_FAILED;
    
    return ERROR_OK;
}

syscon_error_t fpga_copy_video_cfg(struct fpga_dev *fpga_dev, uint32_t *flash_setting, uint8_t load_store)
{
    /*load each resolution from flash separately, this is easier because the number of settings is identical for each resolution*/
    /*we load them into a separate video config array to not have any conflict with the flash settings array during runtime*/
    if(fpga_copy_resolution_cfg(fpga_dev, RES_512i, flash_setting, NO_H_IMAGE_OFFSET_512i, load_store) != ERROR_OK)
        return FPGA_COMMAND_FAILED;
    if(fpga_copy_resolution_cfg(fpga_dev, RES_480i, flash_setting, NO_H_IMAGE_OFFSET_480i, load_store) != ERROR_OK)
        return FPGA_COMMAND_FAILED;
    if(fpga_copy_resolution_cfg(fpga_dev, RES_480p, flash_setting, NO_H_IMAGE_OFFSET_480p, load_store) != ERROR_OK)
        return FPGA_COMMAND_FAILED;
    if(fpga_copy_resolution_cfg(fpga_dev, RES_256p, flash_setting, NO_H_IMAGE_OFFSET_256p, load_store) != ERROR_OK)
        return FPGA_COMMAND_FAILED;
    if(fpga_copy_resolution_cfg(fpga_dev, RES_240p, flash_setting, NO_H_IMAGE_OFFSET_240p, load_store) != ERROR_OK)
        return FPGA_COMMAND_FAILED;

    return ERROR_OK;
}

syscon_error_t fpga_write_address(struct fpga_dev *fpga_dev, struct SysCon_Pins *io, uint16_t address, uint8_t *tx_buffer, uint8_t len)
{
    uint8_t command_buffer[4] = {0};
    /*header*/
    command_buffer[0] = CMD_WRITE_REG;
    command_buffer[1] = (uint8_t)address;
    command_buffer[2] = (uint8_t)(address>>8);;

    sys_set_pin(io->pin[PIN_T20_CE1], SYSPIN_HIGH);

    /*header*/
    if(spi_write(fpga_dev->interface, 3, command_buffer) != ERROR_OK)
        return FPGA_COMMAND_FAILED;
    /*data*/
    if(spi_write(fpga_dev->interface, len, tx_buffer) != ERROR_OK)
        return FPGA_COMMAND_FAILED;

    sys_set_pin(io->pin[PIN_T20_CE1], SYSPIN_LOW);
    /*tiny bit of delay for the video processor*/
    delay_us(5);

    return ERROR_OK;
}

syscon_error_t fpga_read_address(struct fpga_dev *fpga_dev, struct SysCon_Pins *io, uint16_t address, uint8_t *return_byte)
{
    uint8_t write_buffer[5] = {0};
    uint8_t read_buffer[5] = {0};
    /*header*/
    write_buffer[0] = CMD_READ_REG;
    write_buffer[1] = (uint8_t)address;
    write_buffer[2] = (uint8_t)(address>>8);
    write_buffer[3] = 0x5A; //dummy
    write_buffer[4] = 0x5A; //dummy

    sys_set_pin(io->pin[PIN_T20_CE1], SYSPIN_HIGH);

    if(spi_read_write(fpga_dev->interface, write_buffer, read_buffer, 5) != ERROR_OK)
        return FPGA_COMMAND_FAILED;

    sys_set_pin(io->pin[PIN_T20_CE1], SYSPIN_LOW);
    delay_us(5);

    *return_byte = read_buffer[4];

    return ERROR_OK;
}

syscon_error_t fpga_erase_data(struct fpga_dev *fpga_dev, struct SysCon_Pins *io, uint16_t address, uint8_t len)
{
    uint8_t command_buffer[4] = {0};
    /*command*/
    command_buffer[0] = CMD_WRITE_REG;
    command_buffer[1] = (uint8_t)address;
    command_buffer[2] = (uint8_t)(address>>8);;

    sys_set_pin(io->pin[PIN_T20_CE1], SYSPIN_HIGH);

    /*command*/
    if(spi_write(fpga_dev->interface, 3, command_buffer) != ERROR_OK)
        return FPGA_COMMAND_FAILED;

    command_buffer[0] = 0;
    for(uint16_t i = 0; i<len; i++){
        /*data always zero*/
        if(spi_write(fpga_dev->interface, 1, command_buffer) != ERROR_OK)
            return FPGA_COMMAND_FAILED;
    }
    
    sys_set_pin(io->pin[PIN_T20_CE1], SYSPIN_LOW);

    return ERROR_OK;
}


syscon_error_t fpga_apply_video_setting(struct fpga_dev *fpga_dev, struct SysCon_Pins *io, uint16_t resolution, uint16_t offset, uint8_t *tx_buffer, uint8_t len)
{
    uint16_t address = 0;

    if(resolution == RES_512i)
        address = REG_SETTINGS_512i + offset;
    else if(resolution == RES_480i)
        address = REG_SETTINGS_480i + offset;
    else if(resolution == RES_480p)
        address = REG_SETTINGS_480p + offset;
    else if(resolution == RES_240p)
        address = REG_SETTINGS_240p + offset;
    else if(resolution == RES_256p)
        address = REG_SETTINGS_256p + offset;
    else
        return FPGA_COMMAND_FAILED;

    /*set the config_override flag to update the settings*/
    if(fpga_lock_config_update(fpga_dev, io, ENABLE) != ERROR_OK)
        return FPGA_COMMAND_FAILED;

    /*then we load the settings for one resolution*/
    if(fpga_write_address(fpga_dev, io, address, tx_buffer, len) != ERROR_OK)
        return FPGA_COMMAND_FAILED;
    
    /*at the end unlock config updates to apply the settings*/
    if(fpga_lock_config_update(fpga_dev, io, DISABLE) != ERROR_OK)
        return FPGA_COMMAND_FAILED;

    return ERROR_OK;
}

syscon_error_t fpga_reload_video_cfg(struct fpga_dev *fpga_dev, struct SysCon_Pins *io, uint16_t resolution, uint32_t *video_config)
{
    uint8_t data_buffer[VIDEO_CONFIG_BUFFER_LENGTH] = {0};

    /*fill the tx buffer with the video config*/
    fpga_fill_cfg_buffer(data_buffer, video_config);

    /*then we load the settings for one resolution*/
    if(fpga_apply_video_setting(fpga_dev, io, resolution, OFFSET_H_IMAGE_OFFSET_LSB, data_buffer, 23) != ERROR_OK)
        return FPGA_COMMAND_FAILED;
    
    return ERROR_OK;
}


syscon_error_t fpga_set_cursor(struct fpga_dev *fpga_dev, struct SysCon_Pins *io, uint16_t x, uint16_t y)
{
    uint8_t tx_buffer[4] = {0};
    /*transmit data*/
    tx_buffer[0] = (uint8_t)x;
    tx_buffer[1] = (uint8_t)(x>>8);
    tx_buffer[2] = (uint8_t)y;
    tx_buffer[3] = (uint8_t)(y>>8);

    if(fpga_write_address(fpga_dev, io, REG_POSX_LSB, tx_buffer, 4) != ERROR_OK)
        return FPGA_COMMAND_FAILED;
    
    return ERROR_OK;
}

syscon_error_t fpga_get_current_resolution(struct fpga_dev *fpga_dev, struct SysCon_Pins *io, uint16_t *resolution)
{
    uint8_t returned_resolution = 0;
    /*first we need to read the video resolution to change the right settings*/
    if(fpga_read_address(fpga_dev, io, REG_RESOLUTION_INFO, &returned_resolution) != ERROR_OK)
        return FPGA_COMMAND_FAILED;

    switch(returned_resolution)
    {
        case RETURN_512i:
            *resolution = RES_512i;
            break;
        case RETURN_480i:
            *resolution = RES_480i;
            break;
        case RETURN_480p:
            *resolution = RES_480p;
            break;
        case RETURN_256p:
            *resolution = RES_256p;
            break;
        case RETURN_240p:
            *resolution = RES_240p;
            break;
        default:
            return FPGA_COMMAND_FAILED;
    }

    return ERROR_OK;
}

syscon_error_t fpga_video_endi(struct fpga_dev *fpga_dev, struct SysCon_Pins *io, uint8_t endi)
{
    uint8_t data_buffer = 0;
    uint8_t enable_status = 0;

    /*first we need to read the video control register to keep all bits except for video_enable intact*/
    if(fpga_read_address(fpga_dev, io, REG_VIDEO_CONTROL, &data_buffer) != ERROR_OK)
        return FPGA_COMMAND_FAILED;

    /*then we enable or disable the video output*/
    if(endi == ENABLE) enable_status = data_buffer | 0b00000010; // enable video_enable flag
    else enable_status = data_buffer & 0b11111101; // disable video_enable flag
    if(fpga_write_address(fpga_dev, io, REG_VIDEO_CONTROL, &enable_status, 1) != ERROR_OK)
        return FPGA_COMMAND_FAILED;

    return ERROR_OK;

}


syscon_error_t fpga_set_image_offset(struct fpga_dev *fpga_dev, struct SysCon_Pins *io, uint8_t direction, uint8_t axis)
{
    uint8_t tx_buffer[4] = {0};
    uint16_t resolution = 0;
    uint32_t horizontal_offset = 0;
    
    /*first we need to read the video resolution to change the right settings*/
    if(fpga_get_current_resolution(fpga_dev, io, &resolution) != ERROR_OK)
        return FPGA_COMMAND_FAILED;

    switch(axis){
        case UPDATE_X:
            uint32_t horizontal_inc_dec = fpga_dev->video_config[resolution][NO_SAMPLING_DIVIDER] + 1;
            /*handle incrementing/decrementing based on the direction*/
            inc_dec_setting(&fpga_dev->video_config[resolution][NO_H_IMAGE_OFFSET], direction, horizontal_inc_dec, 512, 0);
            /*transmit new horizontal offset setting*/
            horizontal_offset = fpga_dev->video_config[resolution][NO_H_IMAGE_OFFSET] + fpga_dev->video_config[resolution][NO_PHASE];
            tx_buffer[0] = (uint8_t)horizontal_offset;
            tx_buffer[1] = (uint8_t)(horizontal_offset>>8);
            if(fpga_apply_video_setting(fpga_dev, io, resolution, OFFSET_H_IMAGE_OFFSET_LSB, tx_buffer, 2) != ERROR_OK)
                return FPGA_COMMAND_FAILED;
            break;
        case UPDATE_Y:
            inc_dec_setting(&fpga_dev->video_config[resolution][NO_V_IMAGE_OFFSET], direction, 1, 64, 0);
            /*transmit new vertical offset setting*/
            tx_buffer[0] = (uint8_t)fpga_dev->video_config[resolution][NO_V_IMAGE_OFFSET];
            if(fpga_apply_video_setting(fpga_dev, io, resolution, OFFSET_V_IMAGE_OFFSET, tx_buffer, 1) != ERROR_OK)
                return FPGA_COMMAND_FAILED;
            break;
    }

    return ERROR_OK;   
}


syscon_error_t fpga_set_phase(struct fpga_dev *fpga_dev, struct SysCon_Pins *io, uint8_t direction)
{
    uint32_t horizontal_offset = 0;
    uint8_t tx_buffer[2] = {0};
    uint16_t resolution = 0;

    /*first we need to read the video resolution to change the right settings*/
    if(fpga_get_current_resolution(fpga_dev, io, &resolution) != ERROR_OK)
        return FPGA_COMMAND_FAILED;

    /*handle incrementing/decrementing based on the direction*/
    uint32_t sampling_divider = fpga_dev->video_config[resolution][NO_SAMPLING_DIVIDER];
    inc_dec_setting(&fpga_dev->video_config[resolution][NO_PHASE], direction, 1, sampling_divider, 0);
            
    horizontal_offset = fpga_dev->video_config[resolution][NO_H_IMAGE_OFFSET] + fpga_dev->video_config[resolution][NO_PHASE];
    tx_buffer[0] = (uint8_t)horizontal_offset;
    tx_buffer[1] = (uint8_t)(horizontal_offset>>8);

    if(fpga_apply_video_setting(fpga_dev, io, resolution, OFFSET_H_IMAGE_OFFSET_LSB, tx_buffer, 2) != ERROR_OK)
        return FPGA_COMMAND_FAILED;

    return ERROR_OK;
}

syscon_error_t fpga_toggle_deinterlacer_debug(struct fpga_dev *fpga_dev, struct SysCon_Pins *io)
{
    uint8_t tx_buffer = 0;
    uint16_t resolution = 0;
    /*first we need to read the video resolution to change the right settings*/
    if(fpga_get_current_resolution(fpga_dev, io, &resolution) != ERROR_OK)
        return FPGA_COMMAND_FAILED;

    /*toggle the debug flag*/
    if(fpga_dev->video_config[resolution][NO_DEINTERLACER_DEBUG] == 0)
        fpga_dev->video_config[resolution][NO_DEINTERLACER_DEBUG] = 1;
    else
        fpga_dev->video_config[resolution][NO_DEINTERLACER_DEBUG] = 0;

    /*here we assembe the register value, which consists of the motion threshold and the debug flag*/
    tx_buffer = (uint8_t)(fpga_dev->video_config[resolution][NO_DEINTERLACER_DEBUG] | (fpga_dev->video_config[resolution][NO_MOTION_THRESHOLD]<<1));   //motion_threshold(7-1) & deinterlacer_debug(0)                  

    if(fpga_apply_video_setting(fpga_dev, io, resolution, OFFSET_DEINTERLACER, &tx_buffer, 1) != ERROR_OK)
        return FPGA_COMMAND_FAILED;

    return ERROR_OK;
}

syscon_error_t fpga_set_motion_threshold(struct fpga_dev *fpga_dev, struct SysCon_Pins *io, uint8_t direction)
{
    uint16_t resolution = 0;
    uint8_t tx_buffer = 0;
    
    /*first we need to read the video resolution to change the right settings*/
    if(fpga_get_current_resolution(fpga_dev, io, &resolution) != ERROR_OK)
        return FPGA_COMMAND_FAILED;

    /*handle incrementing/decrementing based on the direction*/
    inc_dec_setting(&fpga_dev->video_config[resolution][NO_MOTION_THRESHOLD], direction, 1, 32, 0);
    
    /*here we assembe the register value, which consists of the motion threshold and the debug flag*/
    tx_buffer = (uint8_t)(fpga_dev->video_config[resolution][NO_DEINTERLACER_DEBUG] | (fpga_dev->video_config[resolution][NO_MOTION_THRESHOLD]<<1));   //motion_threshold(7-1) & deinterlacer_debug(0)                  

    if(fpga_apply_video_setting(fpga_dev, io, resolution, OFFSET_DEINTERLACER, &tx_buffer, 1) != ERROR_OK)
        return FPGA_COMMAND_FAILED;

    return ERROR_OK;
}

syscon_error_t fpga_set_brightness(struct fpga_dev *fpga_dev, struct SysCon_Pins *io, uint8_t direction)
{
    uint8_t tx_buffer[2] = {0};
    uint32_t brightness = (uint32_t)fpga_dev->brightness; 
    /*handle incrementing/decrementing based on the direction*/
    inc_dec_setting(&brightness, direction, 5, BACKLIGHT_MAX_BRIGHTNESS, 0);

    fpga_dev->brightness = (uint8_t)brightness;

    /*payload*/
    tx_buffer[0] = (uint8_t)BACKLIGHT_MAX_COUNT;
    tx_buffer[1] = (uint8_t)(brightness*2);
    if(fpga_write_address(fpga_dev, io, RANGE_BRIGHTNESS, tx_buffer, 2) != ERROR_OK)
        return FPGA_COMMAND_FAILED;
    
    return ERROR_OK;
}

syscon_error_t fpga_set_xy_resolution(struct fpga_dev *fpga_dev, struct SysCon_Pins *io, uint8_t direction, uint8_t axis)
{
    uint8_t data_buffer[VIDEO_CONFIG_BUFFER_LENGTH] = {0};
    uint32_t y_resolution_corrected = 0;
    double scaling_x, scaling_y, packed_pixel = 0.0;
    uint16_t resolution = 0;
    /*first we need to read the video resolution to change the right settings*/
    if(fpga_get_current_resolution(fpga_dev, io, &resolution) != ERROR_OK)
        return FPGA_COMMAND_FAILED;

    switch(axis){
        case RES_IN_X:
            inc_dec_setting(&fpga_dev->video_config[resolution][NO_H_ACTIVE_PXL], direction, 4, 704, 256);
            fpga_dev->video_config[resolution][NO_H_IMAGE_ACTIVE] = fpga_dev->video_config[resolution][NO_H_ACTIVE_PXL] * (fpga_dev->video_config[resolution][NO_SAMPLING_DIVIDER] + 1);
            packed_pixel = (double)fpga_dev->video_config[resolution][NO_H_ACTIVE_PXL] * 0.75;
            fpga_dev->video_config[resolution][NO_H_PACKED_PXL] = (uint32_t)packed_pixel;
            break;
        case RES_IN_Y:
            inc_dec_setting(&fpga_dev->video_config[resolution][NO_V_IMAGE_ACTIVE], direction, 4, 480, 200);
            break;
        case RES_OUT_X:
            inc_dec_setting(&fpga_dev->video_config[resolution][NO_TARGET_RES_X], direction, 2, 800, 600);
            break;
        case RES_OUT_Y:
            inc_dec_setting(&fpga_dev->video_config[resolution][NO_TARGET_RES_Y], direction, 2, 480, 448);
            break;
    }

    fpga_calculate_scale(fpga_dev, resolution, &fpga_dev->video_config[resolution][NO_SCALING_X], &fpga_dev->video_config[resolution][NO_SCALING_Y]);
    
    if(fpga_reload_video_cfg(fpga_dev, io, resolution, fpga_dev->video_config[resolution]) != ERROR_OK)
        return FPGA_COMMAND_FAILED;

    return ERROR_OK;   
}


syscon_error_t fpga_set_sampling_divider(struct fpga_dev *fpga_dev, struct SysCon_Pins *io, uint8_t direction)
{
    uint32_t resolution_select[10] = {0,0,0,640,512,448,384,320,0,256}; /*possible horizontal resolutions based on the sampling divider*/
    uint8_t data_buffer[VIDEO_CONFIG_BUFFER_LENGTH] = {0};
    uint16_t resolution = 0;

    /*first we need to read the video resolution to change the right settings*/
    if(fpga_get_current_resolution(fpga_dev, io, &resolution) != ERROR_OK)
        return FPGA_COMMAND_FAILED;

    /*set the sampling divider and the default horizontal resolution respectively*/
    /*this whole calculation only applies to NTSC/PAL video signals*/
    if(resolution != RES_480p){
        inc_dec_setting(&fpga_dev->video_config[resolution][NO_SAMPLING_DIVIDER], direction, 1, 9, 3);
        fpga_dev->video_config[resolution][NO_H_ACTIVE_PXL] = resolution_select[fpga_dev->video_config[resolution][NO_SAMPLING_DIVIDER]];
        /*update the other horizontal parameters*/
        fpga_calculate_x_params(fpga_dev, resolution, &fpga_dev->video_config[resolution][NO_H_IMAGE_ACTIVE], &fpga_dev->video_config[resolution][NO_H_PACKED_PXL], &fpga_dev->video_config[resolution][NO_H_IMAGE_OFFSET]);
        /*update the scale...*/
        fpga_calculate_scale(fpga_dev, resolution, &fpga_dev->video_config[resolution][NO_SCALING_X], &fpga_dev->video_config[resolution][NO_SCALING_Y]);

        if(fpga_reload_video_cfg(fpga_dev, io, resolution, fpga_dev->video_config[resolution]) != ERROR_OK)
        return FPGA_COMMAND_FAILED;
    }
    else{
        /*480p only supports 640 pixels in x, so we shouldn't update the divider or the resolution*/
    }

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
   return ERROR_OK;
}

/*PRIVATE functions-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

static syscon_error_t fpga_transmit_bitstream(struct fpga_dev *fpga_dev, struct SysCon_Pins *io)
{   
    /*reset FPGA and set initial control pin states*/
    sys_set_pin(io->pin[PIN_T20_CE0], SYSPIN_HIGH);
    sys_set_pin(io->pin[PIN_T20_CRESET], SYSPIN_LOW);
    delay_ms(T_CRESET_MS);
    sys_set_pin(io->pin[PIN_T20_CRESET], SYSPIN_HIZ);
    delay_ms(T_DMIN_MS);
    
    /*write the bitstream in bigger packets until the bitstream size (or more) was transmitted*/
    uint32_t iterations = 0;
    while (iterations <= bitstream_size)
    {
        if(iterations <= (bitstream_size-SPI_PACKET_SIZE_BITSTREAM)){
            /*write SPI_PACKET_SIZE byte packets to speed things up...*/
            if(spi_write(fpga_dev->interface, SPI_PACKET_SIZE_BITSTREAM, &bitstream[iterations]) != ERROR_OK){
                sys_set_pin(io->pin[PIN_T20_CE0], SYSPIN_LOW);
                return FPGA_CONFIG_FAILED;
            }
            iterations = iterations + SPI_PACKET_SIZE_BITSTREAM;
        }
        else{
            /*once we are close to the bitstream size, write single bytes to not go out of bounds here...*/
            if(spi_write(fpga_dev->interface, 1, &bitstream[iterations]) != ERROR_OK){
                sys_set_pin(io->pin[PIN_T20_CE0], SYSPIN_LOW);
                return FPGA_CONFIG_FAILED;
           }
            iterations = iterations + 1; 
        }
        
    }
    /*provide additional clock cycles until CDONE is high -> just send dummy bytes*/
    uint8_t dummy_byte = 0;
    uint16_t config_timeout = 0;
    while(sys_get_pin(io->pin[PIN_T20_CDONE]) == 0)
    {
        if(spi_write(fpga_dev->interface, 1, &dummy_byte) != ERROR_OK)
            return FPGA_CONFIG_FAILED;

        config_timeout++;
        if(config_timeout > 255)
            return FPGA_CONFIG_TIMEOUT;
    }

    #if defined (BLOCK_T20_JTAG)
        /*T20 JTAG will not work if this is low -> any problems with the video processor spi when this stays high?*/
        /*only comment out during debugging!*/
        sys_set_pin(io->pin[PIN_T20_CE0], SYSPIN_LOW);
    #endif

    return ERROR_OK;
} 

static syscon_error_t fpga_transmit_font(struct fpga_dev *fpga_dev, struct SysCon_Pins *io)
{
    uint8_t tx_buffer[3] = {0};
    
    sys_set_pin(io->pin[PIN_T20_CE1], SYSPIN_HIGH);
    /*command to load font*/
    tx_buffer[0] = CMD_WRITE_REG;
    tx_buffer[1] = (uint8_t)(RANGE_SPRITE_RAM);
    tx_buffer[2] = (uint8_t)(RANGE_SPRITE_RAM>>8);
    if(spi_write(fpga_dev->interface, 3, tx_buffer) != ERROR_OK)
        return FPGA_COMMAND_FAILED;

    /*write the font in 8 byte packets until the font size was transmitted*/
    for(uint16_t iterations = 0; iterations < font_size; iterations++)
    {
        /*write SPI_PACKET_SIZE byte packets to speed things up...*/
        if(spi_write(fpga_dev->interface, SPI_PACKET_SIZE_FONT, &font8x8_basic[iterations][0]) != ERROR_OK){
            sys_set_pin(io->pin[PIN_T20_CE1], SYSPIN_LOW);
            return FPGA_COMMAND_FAILED;
        }
    }
    sys_set_pin(io->pin[PIN_T20_CE1], SYSPIN_LOW);

    return ERROR_OK;
}

static syscon_error_t fpga_copy_resolution_cfg(struct fpga_dev *fpga_dev, uint8_t resolution, uint32_t *flash_setting, uint16_t flash_setting_base, uint8_t load_store)
{
    uint16_t flash_setting_no = flash_setting_base;

    if(resolution < NO_RESOLUTIONS)
    {
        /*cycle through resolutions*/
        for(uint8_t set = 0; set < NO_VIDEO_SETTINGS; set++)
        {
            /*cycle through settings for each resolution and load from flash*/
            if(load_store == RES_LOAD)
                fpga_dev->video_config[resolution][set] = flash_setting[flash_setting_no];
            else
                flash_setting[flash_setting_no] = fpga_dev->video_config[resolution][set];

            flash_setting_no++;
        }
        return ERROR_OK;
    }
    else
    {
        return FPGA_COMMAND_FAILED;
    }
}

static syscon_error_t fpga_lock_config_update(struct fpga_dev *fpga_dev, struct SysCon_Pins *io, uint8_t endi)
{
    uint8_t data_buffer = 0;
    uint8_t enable_status = 0;

    /*first we need to read the video control register to keep all bits except for config_override intact*/
    if(fpga_read_address(fpga_dev, io, REG_VIDEO_CONTROL, &data_buffer) != ERROR_OK)
        return FPGA_COMMAND_FAILED;

    /*then we lock or unlock config updates to update the video settings*/
    if(endi != ENABLE) enable_status = data_buffer | 0b00000001; //enabling config_override flag
    else enable_status = data_buffer & 0b11111110; // disabling config_override flag
    if(fpga_write_address(fpga_dev, io, REG_VIDEO_CONTROL, &enable_status, 1) != ERROR_OK)
        return FPGA_COMMAND_FAILED;

    return ERROR_OK;
}

static void fpga_fill_cfg_buffer(uint8_t *data_buffer, uint32_t *video_config)
{
    uint32_t horizontal_offset = 0;

    horizontal_offset = video_config[NO_H_IMAGE_OFFSET] + video_config[NO_PHASE];

    /*TODO this is ugly as hell, improve once working!*/                  
    data_buffer[OFFSET_H_IMAGE_OFFSET_LSB] =    (uint8_t)horizontal_offset;      
    data_buffer[OFFSET_H_IMAGE_OFFSET_MSB] =    (uint8_t)(horizontal_offset>>8); 
    data_buffer[OFFSET_H_IMAGE_ACTIVE_LSB] =    (uint8_t)video_config[NO_H_IMAGE_ACTIVE];      
    data_buffer[OFFSET_H_IMAGE_ACTIVE_MSB] =    (uint8_t)(video_config[NO_H_IMAGE_ACTIVE]>>8); 
    data_buffer[OFFSET_H_ACTIVE_PXL_LSB] =      (uint8_t)video_config[NO_H_ACTIVE_PXL];        
    data_buffer[OFFSET_H_ACTIVE_PXL_MSB] =      (uint8_t)(video_config[NO_H_ACTIVE_PXL]>>8);   
    data_buffer[OFFSET_H_PACKED_PXL_LSB] =      (uint8_t)video_config[NO_H_PACKED_PXL];        
    data_buffer[OFFSET_H_PACKED_PXL_MSB] =      (uint8_t)(video_config[NO_H_PACKED_PXL]>>8);   
    data_buffer[OFFSET_V_IMAGE_OFFSET] =        (uint8_t)video_config[NO_V_IMAGE_OFFSET];          
    data_buffer[OFFSET_V_IMAGE_ACTIVE_LSB] =    (uint8_t)video_config[NO_V_IMAGE_ACTIVE];        
    data_buffer[OFFSET_V_IMAGE_ACTIVE_MSB] =    (uint8_t)(video_config[NO_V_IMAGE_ACTIVE]>>8);  
    data_buffer[OFFSET_INTERLACE_DIVIDE] =      (uint8_t)(video_config[NO_INTERLACE] | (video_config[NO_SAMPLING_DIVIDER]<<4));   //sampling_divider(7-4) & interlaced_flag(0)             
    data_buffer[OFFSET_DEINTERLACER] =          (uint8_t)(video_config[NO_DEINTERLACER_DEBUG] | (video_config[NO_MOTION_THRESHOLD]<<1));   //motion_threshold(7-1) & deinterlacer_debug(0)                  
    data_buffer[OFFSET_SCALE_X_LSB] =           (uint8_t)(video_config[NO_SCALING_X]);
    data_buffer[OFFSET_SCALE_X_MID] =           (uint8_t)(video_config[NO_SCALING_X] >> 8);
    data_buffer[OFFSET_SCALE_X_MSB] =           (uint8_t)(video_config[NO_SCALING_X] >> 16);
    data_buffer[OFFSET_SCALE_Y_LSB] =           (uint8_t)(video_config[NO_SCALING_Y]);
    data_buffer[OFFSET_SCALE_Y_MID] =           (uint8_t)(video_config[NO_SCALING_Y] >> 8);
    data_buffer[OFFSET_SCALE_Y_MSB] =           (uint8_t)(video_config[NO_SCALING_Y] >> 16);
    data_buffer[OFFSET_TARGET_RES_X_LSB] =      (uint8_t)(video_config[NO_TARGET_RES_X]);
    data_buffer[OFFSET_TARGET_RES_X_MSB] =      (uint8_t)(video_config[NO_TARGET_RES_X] >> 8);
    data_buffer[OFFSET_TARGET_RES_Y_LSB] =      (uint8_t)(video_config[NO_TARGET_RES_Y]);
    data_buffer[OFFSET_TARGET_RES_Y_MSB] =      (uint8_t)(video_config[NO_TARGET_RES_Y] >> 8);

    return;
}

static void fpga_calculate_scale(struct fpga_dev *fpga_dev, uint16_t resolution_to_modify, uint32_t *scale_x_out, uint32_t *scale_y_out)
{
    uint32_t input_x = fpga_dev->video_config[resolution_to_modify][NO_H_ACTIVE_PXL];
    uint32_t input_y = fpga_dev->video_config[resolution_to_modify][NO_V_IMAGE_ACTIVE];
    uint32_t target_x = fpga_dev->video_config[resolution_to_modify][NO_TARGET_RES_X];
    uint32_t target_y = fpga_dev->video_config[resolution_to_modify][NO_TARGET_RES_Y];

    uint32_t input_y_corrected = 0;
    double scaling_x, scaling_y = 0.0;

    /*for interlaced resolution we need to double the vertical input resolution, as the scaler will see double that*/
    if(resolution_to_modify != RES_480p) input_y_corrected = (double)input_y * 2;
    else input_y_corrected = input_y;
    /*first we calculate the plain scaling factor (in-1)/(out-1)*/
    scaling_x = ((double)input_x - 1.0) / ((double)target_x - 1.0);
    scaling_y = (input_y_corrected - 1.0) / ((double)target_y - 1.0);
    /*then we convert to fixed point -> 15 bit for the fractional part*/
    scaling_x = scaling_x * pow(2.0, 15.0);
    scaling_y = scaling_y * pow(2.0, 15.0);
    /*convert back to uint and store to array -> the result should be an 18 bit value*/
    *scale_x_out = (uint32_t)scaling_x;
    *scale_y_out = (uint32_t)scaling_y;

}

static void fpga_calculate_x_params(struct fpga_dev *fpga_dev, uint16_t resolution_to_modify, uint32_t *h_image_active_out, uint32_t *h_packed_pxl_out, uint32_t *h_offset_out)
{
    uint32_t x_resolution = fpga_dev->video_config[resolution_to_modify][NO_H_ACTIVE_PXL];
    uint32_t h_offset = fpga_dev->video_config[resolution_to_modify][NO_H_IMAGE_OFFSET];
    uint32_t x_resolution_clocks = 0;
    double packed_pixel = 0.0;

    /*number of pixels multiplied by sampling divider to get the active clock cycles*/
    x_resolution_clocks = x_resolution * (fpga_dev->video_config[resolution_to_modify][NO_SAMPLING_DIVIDER] + 1);
    packed_pixel = (double)x_resolution * 0.75;

    switch(resolution_to_modify){
        case RES_512i:
            if((3202-x_resolution_clocks) < h_offset)
                h_offset = (3202-x_resolution_clocks);
            break;
        case RES_480i:
            if((3178-x_resolution_clocks) < h_offset)
                h_offset = (3178-x_resolution_clocks);
            break;
    }

    *h_image_active_out = x_resolution_clocks;
    *h_packed_pxl_out = (uint32_t)packed_pixel;
    *h_offset_out = h_offset;
}

syscon_error_t fpga_get_version(struct fpga_dev *fpga_dev, struct SysCon_Pins *io, uint8_t *version_major, uint8_t *version_minor)
{
    uint8_t major, minor = 0;
    /*first we need to read the video resolution to change the right settings*/
    if(fpga_read_address(fpga_dev, io, REG_VERSION_MAJOR, &major) != ERROR_OK)
        return FPGA_COMMAND_FAILED;
    if(fpga_read_address(fpga_dev, io, REG_VERSION_MINOR, &minor) != ERROR_OK)
        return FPGA_COMMAND_FAILED;

    *version_major = major;
    *version_minor = minor;

    return ERROR_OK;
}
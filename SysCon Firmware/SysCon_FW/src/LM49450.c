#include "LM49450.h"

static syscon_error_t amp_get_regs(struct speaker_amp *amp_dev, uint8_t start_address, uint8_t *rw_buffer, uint8_t no_of_regs)
{
    int8_t result = ERROR_OK;

    amp_dev->amp_busy = 1;
    if (i2c_read(amp_dev->interface, amp_dev->device_address, start_address, rw_buffer, no_of_regs) != I2C_SUCCESS)
        result = GENERIC_ERROR;
    amp_dev->amp_busy = 0;
    return result;
}

static syscon_error_t amp_set_regs(struct speaker_amp *amp_dev, uint8_t start_address, uint8_t *rw_buffer, uint8_t no_of_regs)
{
    int8_t result = ERROR_OK;

    amp_dev->amp_busy = 1;
    if (i2c_write(amp_dev->interface, amp_dev->device_address, start_address, rw_buffer, no_of_regs) != I2C_SUCCESS)
        result = GENERIC_ERROR;
    amp_dev->amp_busy = 0;
    return result;
}

void amp_init(struct speaker_amp *amp_dev, uint32_t *flash_setting)
{
  amp_dev->interface = GAMEPAD_I2C;
  amp_setup_ps2(amp_dev);
  
  amp_set_speaker_volume(amp_dev, amp_dev->speaker_volume);
  amp_set_headphone_volume(amp_dev, amp_dev->headphone_volume);
}

syscon_error_t amp_setup_ps2(struct speaker_amp *amp_dev)
{
    uint8_t rw_buffer[1] = {0};

    amp_dev->device_address = AMP_SLAVE_ADDRESS;

    rw_buffer[0] = 0b00101001;
    if(amp_set_regs(amp_dev, REG00_MODE_CONTROL, rw_buffer, 1) != ERROR_OK)
        return AMP_FAILED_TO_CONFIGURE;

    rw_buffer[0] = 0b00000010;
    if(amp_set_regs(amp_dev, REG01_CLOCK, rw_buffer, 1) != ERROR_OK)
        return AMP_FAILED_TO_CONFIGURE;

    rw_buffer[0] = 0x4B;
    if(amp_set_regs(amp_dev, REG02_CP_CLK_FREQ, rw_buffer, 1) != ERROR_OK)
        return AMP_FAILED_TO_CONFIGURE;

    rw_buffer[0] = 0b01110010;
    if(amp_set_regs(amp_dev, REG03_I2S_MODE, rw_buffer, 1) != ERROR_OK)
        return AMP_FAILED_TO_CONFIGURE;

    rw_buffer[0] = 0b00000000;
    if(amp_set_regs(amp_dev, REG04_I2S_CLOCK, rw_buffer, 1) != ERROR_OK)
        return AMP_FAILED_TO_CONFIGURE;

    return ERROR_OK;
}

syscon_error_t amp_set_headphone_volume(struct speaker_amp *amp_dev, uint8_t volume)
{
    uint8_t rw_buffer[1] = {0};

    rw_buffer[0] = volume & 0b00011111;
    if(amp_set_regs(amp_dev, REG07_HEADPHONE_VOLUME_CONTROL, rw_buffer, 1) != ERROR_OK)
        return AMP_FAILED_TO_SET_HP_VOL;

    return ERROR_OK;
}
syscon_error_t amp_set_speaker_volume(struct speaker_amp *amp_dev, uint8_t volume)
{
    uint8_t rw_buffer[1] = {0};

    rw_buffer[0] = volume & 0b00011111;
    if(amp_set_regs(amp_dev, REG08_SPEAKER_VOLUME_CONTROL, rw_buffer, 1) != ERROR_OK)
        return AMP_FAILED_TO_SET_LS_VOL;

    return ERROR_OK;
}
syscon_error_t amp_mute(struct speaker_amp *amp_dev, uint8_t mute_en)
{
    uint8_t rw_buffer[1] = {0};

    if(amp_get_regs(amp_dev, REG00_MODE_CONTROL, rw_buffer, 1) != ERROR_OK)
        return AMP_FAILED_TO_MUTE;

    rw_buffer[0] = AMP_SET_BITS(rw_buffer[0], REG00_MUTE, mute_en);    

    if(amp_set_regs(amp_dev, REG00_MODE_CONTROL, rw_buffer, 1) != ERROR_OK)
        return AMP_FAILED_TO_MUTE;

    return ERROR_OK;
}

syscon_error_t amp_inc_dec_hp_volume(struct speaker_amp *amp_dev, uint8_t updown)
{
    uint32_t volume = (uint32_t)amp_dev->headphone_volume;
    /*handle incrementing/decrementing based on the direction*/
    inc_dec_setting(&volume, updown, 1, MAX_VOLUME, 0);
    amp_dev->headphone_volume = volume;
    /*apply the volume*/
    if (amp_set_headphone_volume(amp_dev, amp_dev->headphone_volume) != ERROR_OK)
        return AMP_FAILED_TO_SET_LS_VOL;  
          
    return ERROR_OK;
}

syscon_error_t amp_inc_dec_sp_volume(struct speaker_amp *amp_dev, uint8_t updown)
{
    uint32_t volume = (uint32_t)amp_dev->speaker_volume;
    /*handle incrementing/decrementing based on the direction*/
    inc_dec_setting(&volume, updown, 1, MAX_VOLUME, 0);
    amp_dev->speaker_volume = volume;
    /*apply the volume*/
    if (amp_set_speaker_volume(amp_dev, amp_dev->speaker_volume) != ERROR_OK)
        return AMP_FAILED_TO_SET_LS_VOL;

    return ERROR_OK;
}



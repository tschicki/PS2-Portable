#ifndef LM49450_h
#define LM49450_h

#include "LM49450_Registers.h"
#include "MCU_interface.h"
#include "helper_functions.h"

#define AMP_SLAVE_ADDRESS UINT8_C(0xFA>>1) // 0xFA / 0xFB
#define MAX_VOLUME        20

struct speaker_amp{
    i2c_inst_t *interface;
    uint8_t device_address;
    uint8_t amp_busy;
    uint8_t speaker_volume;
    uint8_t headphone_volume;
    uint8_t hp_detect_state;

};

#define AMP_SET_BITS(reg_data, bitname, data) \
    ((reg_data & ~(bitname##_MASK)) |\
     ((data << bitname##_POS) & bitname##_MASK))



static syscon_error_t amp_get_regs(struct speaker_amp *amp_dev, uint8_t start_address, uint8_t *rw_buffer, uint8_t no_of_regs);
static syscon_error_t amp_set_regs(struct speaker_amp *amp_dev, uint8_t start_address, uint8_t *rw_buffer, uint8_t no_of_regs);

syscon_error_t amp_set_headphone_volume(struct speaker_amp *amp_dev, uint8_t volume);
syscon_error_t amp_set_speaker_volume(struct speaker_amp *amp_dev, uint8_t volume);
syscon_error_t amp_mute(struct speaker_amp *amp_dev, uint8_t mute_en);
syscon_error_t amp_setup_ps2(struct speaker_amp *amp_dev);
void amp_init(struct speaker_amp *amp_dev, uint32_t *flash_setting);
syscon_error_t amp_inc_dec_hp_volume(struct speaker_amp *amp_dev, uint8_t updown);
syscon_error_t amp_inc_dec_sp_volume(struct speaker_amp *amp_dev, uint8_t updown);

#endif
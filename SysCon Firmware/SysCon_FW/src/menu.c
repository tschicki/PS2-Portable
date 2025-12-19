#include "menu.h"

/*text of menu entries*/
static char entry_audio_settings[] = "Audio Settings";
static char entry_hp_volume_display[] = "HP Volume: 99";
static char entry_sp_volume_display[] = "SP Volume: 99";
static char entry_video_settings[] = "Video Settings";
static char entry_video_resolution[] = "Resolution Configs";
static char entry_MAGH[] = "MAGH: 9";
static char entry_MAGH_autodetect[] = "MAGH Autodetect: OFF";
static char entry_video_position_x[] = "X Position: 999";
static char entry_video_position_y[] = "Y Position: 999";
static char entry_video_res_x_in[] = "Input Res X: 999";
static char entry_video_res_y_in[] = "Input Res Y: 999";
static char entry_video_res_x_out[] = "Output Res X: 999";
static char entry_video_res_y_out[] = "Output Res Y: 999";
static char entry_video_others[] = "Other Configs";
static char entry_brightness_display[] = "Brightness: 100%%";
static char entry_video_phase[] = "Horizontal Phase: 3";
static char entry_deinterlacer_debug[] = "Deinterlacer Debug: OFF";
static char entry_motion_thr[] = "Motion Threshold: 999";
static char entry_resolution[] = "Current Resolution: 512i";
static char entry_input_settings[] = "Input Settings";
static char entry_stick_calib_top[] = "Analog Stick Calibration";
static char entry_stick_calib_start[] = "Do not move sticks and press X to start";
static char entry_stick_calib_run[] = "Move sticks in circles, press O to exit";
static char entry_invert_axis_top[] = "Invert Axis";
static char entry_ivert_axis[] = "Move Axis to Invert and press X";
static char entry_stick_deadzone[] = "Deadzone: 999";
static char entry_analog_emulate[] = "PS1 Analog: OFF";
static char entry_rumble_intensity[] = "Rumble Intensity: 10";
static char entry_general[] = "General";
static char entry_credits[] = "About";
static char entry_credits_line1[] = "A Custom PS2 Portable";
static char entry_credits_line2[] = "Designed by Tschicki";
static char entry_credits_line3[] = "2022 - 2025";
static char entry_version[] = "VER: 0.0-0.0        ";
static char entry_monitoring[] = "PM Monitoring";
static char entry_power_display[] = "Battery Power: -99.9W";
static char entry_pack_voltage[] = "Battery Voltage: 9.9V";
static char entry_ttf_display[] = "Time to Full: 9999.9min";
static char entry_tte_display[] = "Time to Empty: 9999.9min";
static char entry_low_temp[] = "Fan Low Temp: 99";
static char entry_high_temp[] = "Fan High Temp: 99";
static char entry_store_inputs[] = "Store Settings   ";
static char menu_main_soc[] = "SOC: 100.0%%";
static char menu_main_charging[] = "DISCHARGE";
static char menu_main_temperature[] = "EE TEMP: -99.9";

void menu_fill_struct(struct menu_dev *menu_dev)
{
    /*needed in order to not break the add_entry function*/
    for (uint8_t i = 0; i < MAX_MENU_ENTRIES; i++)
        menu_dev->menu[i].selected_entry = 0;

    /*main menu*/
    menu_dev->menu[MENU_MAIN].gobackwhere = MENU_MAIN;
    menu_dev->menu[MENU_MAIN].function_to_execute = DO_REFRESH;
    entry_add(&menu_dev->menu[MENU_MAIN], entry_audio_settings, sizeof(entry_audio_settings), MENU_LINE_1_YPOS, MENU_OFFSET_X, NO_SKIP, MENU_AUDIO, ENTER_ENTRY, DO_NOTHING, CURSOR_DOWN, CURSOR_UP, DO_NOTHING, DO_NOTHING);
    entry_add(&menu_dev->menu[MENU_MAIN], entry_video_settings, sizeof(entry_video_settings), MENU_LINE_2_YPOS, MENU_OFFSET_X, NO_SKIP, MENU_VIDEO, ENTER_ENTRY, DO_NOTHING, CURSOR_DOWN, CURSOR_UP, DO_NOTHING, DO_NOTHING);
    entry_add(&menu_dev->menu[MENU_MAIN], entry_input_settings, sizeof(entry_input_settings), MENU_LINE_3_YPOS, MENU_OFFSET_X, NO_SKIP, MENU_INPUT, ENTER_ENTRY, DO_NOTHING, CURSOR_DOWN, CURSOR_UP, DO_NOTHING, DO_NOTHING);
    entry_add(&menu_dev->menu[MENU_MAIN], entry_general, sizeof(entry_general), MENU_LINE_4_YPOS, MENU_OFFSET_X, NO_SKIP, MENU_GENERAL, ENTER_ENTRY, DO_NOTHING, CURSOR_DOWN, CURSOR_UP, DO_NOTHING, DO_NOTHING);
    entry_add(&menu_dev->menu[MENU_MAIN], entry_store_inputs, sizeof(entry_store_inputs), MENU_LINE_6_YPOS, MENU_OFFSET_X, NO_SKIP, MENU_MAIN, MENU_STORE_SETTINGS, GO_BACK, CURSOR_DOWN, CURSOR_UP, DO_NOTHING, DO_NOTHING);

    entry_add(&menu_dev->menu[MENU_MAIN], menu_main_soc, sizeof(menu_main_soc), MENU_LINE_1_YPOS, 400, SKIP, MENU_MAIN, ENTER_ENTRY, DO_NOTHING, DO_NOTHING, DO_NOTHING, DO_NOTHING, DO_NOTHING);
    entry_add(&menu_dev->menu[MENU_MAIN], menu_main_charging, sizeof(menu_main_charging), MENU_LINE_2_YPOS, 400, SKIP, MENU_MAIN, ENTER_ENTRY, DO_NOTHING, DO_NOTHING, DO_NOTHING, DO_NOTHING, DO_NOTHING);
    entry_add(&menu_dev->menu[MENU_MAIN], menu_main_temperature, sizeof(menu_main_temperature), MENU_LINE_3_YPOS, 400, SKIP, MENU_MAIN, ENTER_ENTRY, DO_NOTHING, DO_NOTHING, DO_NOTHING, DO_NOTHING, DO_NOTHING);

    /*audio menu*/
    menu_dev->menu[MENU_AUDIO].gobackwhere = MENU_MAIN;
    menu_dev->menu[MENU_AUDIO].function_to_execute = DO_REFRESH;
    entry_add(&menu_dev->menu[MENU_AUDIO], entry_hp_volume_display, sizeof(entry_hp_volume_display), MENU_LINE_1_YPOS, MENU_OFFSET_X, NO_SKIP, MENU_MAIN, DO_NOTHING, GO_BACK, CURSOR_DOWN, CURSOR_UP, VOLUME_HP, VOLUME_HP);
    entry_add(&menu_dev->menu[MENU_AUDIO], entry_sp_volume_display, sizeof(entry_sp_volume_display), MENU_LINE_2_YPOS, MENU_OFFSET_X, NO_SKIP, MENU_MAIN, DO_NOTHING, GO_BACK, CURSOR_DOWN, CURSOR_UP, VOLUME_SP, VOLUME_SP);

    /*video menu*/
    menu_dev->menu[MENU_VIDEO].gobackwhere = MENU_MAIN;
    menu_dev->menu[MENU_VIDEO].function_to_execute = DO_REFRESH;
    entry_add(&menu_dev->menu[MENU_VIDEO], entry_video_resolution, sizeof(entry_video_resolution), MENU_LINE_1_YPOS, MENU_OFFSET_X, NO_SKIP, MENU_VIDEO_RES, ENTER_ENTRY, GO_BACK, CURSOR_DOWN, CURSOR_UP, DO_NOTHING, DO_NOTHING);
    entry_add(&menu_dev->menu[MENU_VIDEO], entry_video_others, sizeof(entry_video_others), MENU_LINE_2_YPOS, MENU_OFFSET_X, NO_SKIP, MENU_VIDEO_OTHER, ENTER_ENTRY, GO_BACK, CURSOR_DOWN, CURSOR_UP, DO_NOTHING, DO_NOTHING);
    entry_add(&menu_dev->menu[MENU_VIDEO], entry_resolution, sizeof(entry_resolution), MENU_LINE_4_YPOS, MENU_OFFSET_X, SKIP, MENU_MAIN, DO_NOTHING, GO_BACK, CURSOR_DOWN, CURSOR_UP, DO_NOTHING, DO_NOTHING);

    /*video resolution menu*/
    menu_dev->menu[MENU_VIDEO_RES].gobackwhere = MENU_VIDEO;
    menu_dev->menu[MENU_VIDEO_RES].function_to_execute = DO_REFRESH;
    entry_add(&menu_dev->menu[MENU_VIDEO_RES], entry_video_position_x, sizeof(entry_video_position_x), MENU_LINE_1_YPOS, MENU_OFFSET_X, NO_SKIP, MENU_MAIN, DO_NOTHING, GO_BACK, CURSOR_DOWN, CURSOR_UP, PICTURE_X_MOVE, PICTURE_X_MOVE);
    entry_add(&menu_dev->menu[MENU_VIDEO_RES], entry_video_position_y, sizeof(entry_video_position_y), MENU_LINE_2_YPOS, MENU_OFFSET_X, NO_SKIP, MENU_MAIN, DO_NOTHING, GO_BACK, CURSOR_DOWN, CURSOR_UP, PICTURE_Y_MOVE, PICTURE_Y_MOVE);
    entry_add(&menu_dev->menu[MENU_VIDEO_RES], entry_video_res_x_in, sizeof(entry_video_res_x_in), MENU_LINE_3_YPOS, MENU_OFFSET_X, NO_SKIP, MENU_MAIN, DO_NOTHING, GO_BACK, CURSOR_DOWN, CURSOR_UP, VIDEO_IN_RES_X, VIDEO_IN_RES_X);
    entry_add(&menu_dev->menu[MENU_VIDEO_RES], entry_video_res_y_in, sizeof(entry_video_res_y_in), MENU_LINE_4_YPOS, MENU_OFFSET_X, NO_SKIP, MENU_MAIN, DO_NOTHING, GO_BACK, CURSOR_DOWN, CURSOR_UP, VIDEO_IN_RES_Y, VIDEO_IN_RES_Y);
    entry_add(&menu_dev->menu[MENU_VIDEO_RES], entry_video_res_x_out, sizeof(entry_video_res_x_out), MENU_LINE_5_YPOS, MENU_OFFSET_X, NO_SKIP, MENU_MAIN, DO_NOTHING, GO_BACK, CURSOR_DOWN, CURSOR_UP, VIDEO_OUT_RES_X, VIDEO_OUT_RES_X);
    entry_add(&menu_dev->menu[MENU_VIDEO_RES], entry_video_res_y_out, sizeof(entry_video_res_y_out), MENU_LINE_6_YPOS, MENU_OFFSET_X, NO_SKIP, MENU_MAIN, DO_NOTHING, GO_BACK, CURSOR_DOWN, CURSOR_UP, VIDEO_OUT_RES_Y, VIDEO_OUT_RES_Y);
    entry_add(&menu_dev->menu[MENU_VIDEO_RES], entry_MAGH, sizeof(entry_MAGH), MENU_LINE_7_YPOS, MENU_OFFSET_X, NO_SKIP, MENU_MAIN, DO_NOTHING, GO_BACK, CURSOR_DOWN, CURSOR_UP, MAGH, MAGH);
    

    /*video others menu*/
    menu_dev->menu[MENU_VIDEO_OTHER].gobackwhere = MENU_VIDEO;
    menu_dev->menu[MENU_VIDEO_OTHER].function_to_execute = DO_REFRESH;
    entry_add(&menu_dev->menu[MENU_VIDEO_OTHER], entry_brightness_display, sizeof(entry_brightness_display), MENU_LINE_1_YPOS, MENU_OFFSET_X, NO_SKIP, MENU_MAIN, DO_NOTHING, GO_BACK, CURSOR_DOWN, CURSOR_UP, BRIGHTNESS_CONTROL, BRIGHTNESS_CONTROL);
    entry_add(&menu_dev->menu[MENU_VIDEO_OTHER], entry_video_phase, sizeof(entry_video_phase), MENU_LINE_2_YPOS, MENU_OFFSET_X, NO_SKIP, MENU_MAIN, DO_NOTHING, GO_BACK, CURSOR_DOWN, CURSOR_UP, PHASE, PHASE);
    entry_add(&menu_dev->menu[MENU_VIDEO_OTHER], entry_deinterlacer_debug, sizeof(entry_deinterlacer_debug), MENU_LINE_3_YPOS, MENU_OFFSET_X, NO_SKIP, MENU_MAIN, DEINTERLACER_DBG_TOGGLE, GO_BACK, CURSOR_DOWN, CURSOR_UP, DO_NOTHING, DO_NOTHING);
    entry_add(&menu_dev->menu[MENU_VIDEO_OTHER], entry_motion_thr, sizeof(entry_motion_thr), MENU_LINE_4_YPOS, MENU_OFFSET_X, NO_SKIP, MENU_MAIN, DO_NOTHING, GO_BACK, CURSOR_DOWN, CURSOR_UP, MOTION_THR, MOTION_THR);
    entry_add(&menu_dev->menu[MENU_VIDEO_OTHER], entry_MAGH_autodetect, sizeof(entry_MAGH_autodetect), MENU_LINE_5_YPOS, MENU_OFFSET_X, NO_SKIP, MENU_MAIN, MAGH_AUTO, GO_BACK, CURSOR_DOWN, CURSOR_UP, DO_NOTHING, DO_NOTHING);

    /*input menu*/
    menu_dev->menu[MENU_INPUT].gobackwhere = MENU_MAIN;
    menu_dev->menu[MENU_INPUT].function_to_execute = DO_REFRESH;
    entry_add(&menu_dev->menu[MENU_INPUT], entry_stick_calib_top, sizeof(entry_stick_calib_top), MENU_LINE_1_YPOS, MENU_OFFSET_X, NO_SKIP, MENU_JOYSTICK_START, ENTER_ENTRY, GO_BACK, CURSOR_DOWN, CURSOR_UP, DO_NOTHING, DO_NOTHING);
    entry_add(&menu_dev->menu[MENU_INPUT], entry_invert_axis_top, sizeof(entry_invert_axis_top), MENU_LINE_2_YPOS, MENU_OFFSET_X, NO_SKIP, MENU_JOYTSICK_INVERT, ENTER_ENTRY, GO_BACK, CURSOR_DOWN, CURSOR_UP, DO_NOTHING, DO_NOTHING);
    entry_add(&menu_dev->menu[MENU_INPUT], entry_stick_deadzone, sizeof(entry_stick_deadzone), MENU_LINE_3_YPOS, MENU_OFFSET_X, NO_SKIP, MENU_MAIN, DO_NOTHING, GO_BACK, CURSOR_DOWN, CURSOR_UP, DEADZONE_ADJUST, DEADZONE_ADJUST);
    entry_add(&menu_dev->menu[MENU_INPUT], entry_analog_emulate, sizeof(entry_analog_emulate), MENU_LINE_4_YPOS, MENU_OFFSET_X, NO_SKIP, MENU_MAIN, PS1_ANALOG_TOGGLE, GO_BACK, CURSOR_DOWN, CURSOR_UP, DO_NOTHING, DO_NOTHING);
    entry_add(&menu_dev->menu[MENU_INPUT], entry_rumble_intensity, sizeof(entry_rumble_intensity), MENU_LINE_5_YPOS, MENU_OFFSET_X, NO_SKIP, MENU_MAIN, DO_NOTHING, GO_BACK, CURSOR_DOWN, CURSOR_UP, RUMBLE_INTENSITY_CHANGE, RUMBLE_INTENSITY_CHANGE);

    /*analog stick center menu*/
    menu_dev->menu[MENU_JOYSTICK_START].gobackwhere = MENU_INPUT;
    menu_dev->menu[MENU_JOYSTICK_START].function_to_execute = JOYSTICK_SET_CENTER;
    entry_add(&menu_dev->menu[MENU_JOYSTICK_START], entry_stick_calib_start, sizeof(entry_stick_calib_start), MENU_LINE_1_YPOS, MENU_OFFSET_X, NO_SKIP, MENU_JOYSTICK_CALIB, ENTER_ENTRY, GO_BACK, DO_NOTHING, DO_NOTHING, DO_NOTHING, DO_NOTHING);

    /*analog stick calibration menu*/
    menu_dev->menu[MENU_JOYSTICK_CALIB].gobackwhere = MENU_INPUT;
    menu_dev->menu[MENU_JOYSTICK_CALIB].function_to_execute = JOYSTICK_CALIBRATE;
    entry_add(&menu_dev->menu[MENU_JOYSTICK_CALIB], entry_stick_calib_run, sizeof(entry_stick_calib_run), MENU_LINE_1_YPOS, MENU_OFFSET_X, NO_SKIP, MENU_MAIN, DO_NOTHING, GO_BACK, DO_NOTHING, DO_NOTHING, DO_NOTHING, DO_NOTHING);

    /*analog stick invert menu*/
    menu_dev->menu[MENU_JOYTSICK_INVERT].gobackwhere = MENU_INPUT;
    menu_dev->menu[MENU_JOYTSICK_INVERT].function_to_execute = DO_NOTHING;
    entry_add(&menu_dev->menu[MENU_JOYTSICK_INVERT], entry_ivert_axis, sizeof(entry_ivert_axis), MENU_LINE_1_YPOS, MENU_OFFSET_X, NO_SKIP, MENU_MAIN, JOYSTICK_INVERT, GO_BACK, DO_NOTHING, DO_NOTHING, DO_NOTHING, DO_NOTHING);

    /*menu general*/
    menu_dev->menu[MENU_GENERAL].gobackwhere = MENU_MAIN;
    menu_dev->menu[MENU_GENERAL].function_to_execute = DO_REFRESH;
    entry_add(&menu_dev->menu[MENU_GENERAL], entry_credits, sizeof(entry_credits), MENU_LINE_1_YPOS, MENU_OFFSET_X, NO_SKIP, MENU_CREDITS, ENTER_ENTRY, GO_BACK, CURSOR_DOWN, CURSOR_UP, DO_NOTHING, DO_NOTHING);
    entry_add(&menu_dev->menu[MENU_GENERAL], entry_monitoring, sizeof(entry_monitoring), MENU_LINE_2_YPOS, MENU_OFFSET_X, NO_SKIP, MENU_MONITOR, ENTER_ENTRY, GO_BACK, CURSOR_DOWN, CURSOR_UP, DO_NOTHING, DO_NOTHING);
    entry_add(&menu_dev->menu[MENU_GENERAL], entry_low_temp, sizeof(entry_low_temp), MENU_LINE_3_YPOS, MENU_OFFSET_X, NO_SKIP, MENU_MONITOR, DO_NOTHING, GO_BACK, CURSOR_DOWN, CURSOR_UP, FAN_LOW_TEMP, FAN_LOW_TEMP);
    entry_add(&menu_dev->menu[MENU_GENERAL], entry_high_temp, sizeof(entry_high_temp), MENU_LINE_4_YPOS, MENU_OFFSET_X, NO_SKIP, MENU_MONITOR, DO_NOTHING, GO_BACK, CURSOR_DOWN, CURSOR_UP, FAN_HIGH_TEMP, FAN_HIGH_TEMP);

    /*monitoring menu*/
    menu_dev->menu[MENU_MONITOR].gobackwhere = MENU_GENERAL;
    menu_dev->menu[MENU_MONITOR].function_to_execute = DO_REFRESH;
    entry_add(&menu_dev->menu[MENU_MONITOR], entry_power_display, sizeof(entry_power_display), MENU_LINE_1_YPOS, MENU_OFFSET_X, NO_SKIP, MENU_MAIN, DO_NOTHING, GO_BACK, DO_NOTHING, DO_NOTHING, DO_NOTHING, DO_NOTHING);
    entry_add(&menu_dev->menu[MENU_MONITOR], entry_pack_voltage, sizeof(entry_pack_voltage), MENU_LINE_2_YPOS, MENU_OFFSET_X, NO_SKIP, MENU_MAIN, DO_NOTHING, GO_BACK, DO_NOTHING, DO_NOTHING, DO_NOTHING, DO_NOTHING);
    entry_add(&menu_dev->menu[MENU_MONITOR], entry_ttf_display, sizeof(entry_ttf_display), MENU_LINE_3_YPOS, MENU_OFFSET_X, NO_SKIP, MENU_MAIN, DO_NOTHING, GO_BACK, DO_NOTHING, DO_NOTHING, DO_NOTHING, DO_NOTHING);
    entry_add(&menu_dev->menu[MENU_MONITOR], entry_tte_display, sizeof(entry_tte_display), MENU_LINE_4_YPOS, MENU_OFFSET_X, NO_SKIP, MENU_MAIN, DO_NOTHING, GO_BACK, DO_NOTHING, DO_NOTHING, DO_NOTHING, DO_NOTHING);

    /*menu credits*/
    menu_dev->menu[MENU_CREDITS].gobackwhere = MENU_GENERAL;
    menu_dev->menu[MENU_CREDITS].function_to_execute = DO_NOTHING;
    entry_add(&menu_dev->menu[MENU_CREDITS], entry_credits_line1, sizeof(entry_credits_line1), MENU_LINE_1_YPOS, MENU_OFFSET_X, NO_SKIP, MENU_MAIN, DO_NOTHING, GO_BACK, DO_NOTHING, DO_NOTHING, DO_NOTHING, DO_NOTHING);
    entry_add(&menu_dev->menu[MENU_CREDITS], entry_credits_line2, sizeof(entry_credits_line2), MENU_LINE_2_YPOS, MENU_OFFSET_X, NO_SKIP, MENU_MAIN, DO_NOTHING, GO_BACK, DO_NOTHING, DO_NOTHING, DO_NOTHING, DO_NOTHING);
    entry_add(&menu_dev->menu[MENU_CREDITS], entry_credits_line3, sizeof(entry_credits_line3), MENU_LINE_3_YPOS, MENU_OFFSET_X, NO_SKIP, MENU_MAIN, DO_NOTHING, GO_BACK, DO_NOTHING, DO_NOTHING, DO_NOTHING, DO_NOTHING);
    entry_add(&menu_dev->menu[MENU_CREDITS], entry_version, sizeof(entry_version), MENU_LINE_7_YPOS, MENU_OFFSET_X, NO_SKIP, MENU_MAIN, DO_NOTHING, GO_BACK, DO_NOTHING, DO_NOTHING, DO_NOTHING, DO_NOTHING);
}

void entry_add(struct menu *menu, char *content, uint8_t size, uint16_t coords_line, uint16_t coords_column, uint8_t skip, uint8_t launch_menu, uint8_t sel_interaction, uint8_t back_interaction, uint8_t up_interaction, uint8_t down_interaction, uint8_t left_interaction, uint8_t right_interaction)
{

    menu->menu_entry[menu->selected_entry].entry_content = content;
    menu->menu_entry[menu->selected_entry].content_size = size;
    menu->menu_entry[menu->selected_entry].entry_start_coords[0] = coords_column;
    menu->menu_entry[menu->selected_entry].entry_start_coords[1] = coords_line;
    menu->menu_entry[menu->selected_entry].menu_to_lanuch = launch_menu;
    menu->menu_entry[menu->selected_entry].skip_cursor = skip;
    menu->menu_entry[menu->selected_entry].interaction[SEL_INTER] = sel_interaction;
    menu->menu_entry[menu->selected_entry].interaction[BACK_INTER] = back_interaction;
    menu->menu_entry[menu->selected_entry].interaction[UP_INTER] = up_interaction;
    menu->menu_entry[menu->selected_entry].interaction[DOWN_INTER] = down_interaction;
    menu->menu_entry[menu->selected_entry].interaction[LEFT_INTER] = left_interaction;
    menu->menu_entry[menu->selected_entry].interaction[RIGHT_INTER] = right_interaction;
    menu->menu_entry[menu->selected_entry].address_offset = 0;
    menu->selected_entry++;
    menu->used_entries = menu->selected_entry;
}

syscon_error_t menu_init(struct power_dev *power_dev)
{
    power_dev->menu_dev.next_menu = MENU_MAIN;
    power_dev->menu_dev.current_menu = MENU_MAIN;
    power_dev->menu_dev.previous_menu = MENU_MAIN;
    update_interactive_entries(power_dev);

    sprintf(entry_hp_volume_display, "HP Volume: %2u", power_dev->amp_dev.headphone_volume);
    sprintf(entry_sp_volume_display, "SP Volume: %2u", power_dev->amp_dev.speaker_volume);
    sprintf(entry_brightness_display, "Brightness: %3u%%", power_dev->menu_dev.fpga_dev.brightness);
    sprintf(entry_low_temp, "Fan Low Temp: %2u", power_dev->fan_dev.fan_low_temp);
    sprintf(entry_high_temp, "Fan High Temp: %2u", power_dev->fan_dev.fan_high_temp);
    sprintf(entry_stick_deadzone, "Deadzone: %3u", power_dev->gp_dev.analog_stick[GP_RIGHT].axes[0].deadzone);
    sprintf(entry_rumble_intensity, "Rumble Intensity: %2u", power_dev->gp_dev.rumble_intensity);
    /*video entries*/
    if (fpga_get_current_resolution(&power_dev->menu_dev.fpga_dev, &power_dev->io, &power_dev->menu_dev.fpga_dev.current_resolution) != ERROR_OK)
        return ERROR_MENU_REFRESH;
    sprintf(entry_video_position_x, "X Position: %3u", power_dev->menu_dev.fpga_dev.video_config[power_dev->menu_dev.fpga_dev.current_resolution][NO_H_IMAGE_OFFSET]);
    sprintf(entry_video_position_y, "Y Position: %3u", power_dev->menu_dev.fpga_dev.video_config[power_dev->menu_dev.fpga_dev.current_resolution][NO_V_IMAGE_OFFSET]);
    sprintf(entry_video_phase, "Horizontal Phase: %1u", power_dev->menu_dev.fpga_dev.video_config[power_dev->menu_dev.fpga_dev.current_resolution][NO_PHASE]);
    sprintf(entry_motion_thr, "Motion Threshold: %3u", power_dev->menu_dev.fpga_dev.video_config[power_dev->menu_dev.fpga_dev.current_resolution][NO_MOTION_THRESHOLD]);

    sprintf(entry_video_res_x_out, "Output Res X: %3u", power_dev->menu_dev.fpga_dev.video_config[power_dev->menu_dev.fpga_dev.current_resolution][NO_TARGET_RES_X]);
    sprintf(entry_video_res_y_out, "Output Res Y: %3u", power_dev->menu_dev.fpga_dev.video_config[power_dev->menu_dev.fpga_dev.current_resolution][NO_TARGET_RES_Y]);
    sprintf(entry_video_res_x_in, "Input Res X: %3u", power_dev->menu_dev.fpga_dev.video_config[power_dev->menu_dev.fpga_dev.current_resolution][NO_H_ACTIVE_PXL]);
    sprintf(entry_video_res_y_in, "Input Res Y: %3u", power_dev->menu_dev.fpga_dev.video_config[power_dev->menu_dev.fpga_dev.current_resolution][NO_V_IMAGE_ACTIVE]);
    sprintf(entry_MAGH, "MAGH: %1u", power_dev->menu_dev.fpga_dev.video_config[power_dev->menu_dev.fpga_dev.current_resolution][NO_SAMPLING_DIVIDER]);

    if (power_dev->menu_dev.fpga_dev.magh_autodetect_enable == 1){
        sprintf(entry_MAGH_autodetect, "MAGH Autodetect: ON ");
    }
    else{
        sprintf(entry_MAGH_autodetect, "MAGH Autodetect: OFF");
    }

    if (power_dev->menu_dev.fpga_dev.video_config[power_dev->menu_dev.fpga_dev.current_resolution][NO_DEINTERLACER_DEBUG] == 1){
        sprintf(entry_deinterlacer_debug, "Deinterlacer Debug: ON ");
    }
    else{
        sprintf(entry_deinterlacer_debug, "Deinterlacer Debug: OFF");
    }


    /*update the version string*/
    sprintf(entry_version, "VER: %03u.%03u-%03u.%03u", FW_VERSION_MAJOR, FW_VERSION_MINOR, power_dev->menu_dev.fpga_dev.version_major, power_dev->menu_dev.fpga_dev.version_minor);

    sprintf(entry_store_inputs, "Store Settings   ");

    power_dev->menu_dev.menu[power_dev->menu_dev.next_menu].selected_entry = 0;
    menu_draw_page(&power_dev->menu_dev, &power_dev->io);

    return ERROR_OK;
}

syscon_error_t menu_cleanup(struct menu_dev *menu_dev, struct SysCon_Pins *io)
{
    menu_dev->previous_menu = menu_dev->current_menu;
    menu_erase_page(menu_dev, io);
    return ERROR_OK;
}

syscon_error_t menu_draw_entry(struct fpga_dev *fpga_dev, struct SysCon_Pins *io, struct menu_entry *entry, uint8_t selection)
{
    if (entry->address_offset == 0)
    {
        /*if the entry was not drawn yet (address = 0), we need to assign an address offset and increment the current address in the spritebuffer
        if the entry was drawn, we just overwrite the existing addresses*/
        entry->address_offset = fpga_dev->current_spritebuffer_addr + 1;
        fpga_dev->current_spritebuffer_addr = fpga_dev->current_spritebuffer_addr + entry->content_size;
    }

    if (selection == 1)
    {
        /*load color for selection and draw entry*/
        fpga_write_address(fpga_dev, io, REG_COLORA_LSB, &fpga_dev->font_color[COLOR_FONT_SEL][COLOR_LSB], 2);
        fpga_write_address(fpga_dev, io, REG_COLORB_LSB, &fpga_dev->font_color[COLOR_BACK_SEL][COLOR_LSB], 2);
        fpga_set_cursor(fpga_dev, io, entry->entry_start_coords[0], entry->entry_start_coords[1]);
        fpga_write_address(fpga_dev, io, (RANGE_SPRITE_BUF + entry->address_offset), entry->entry_content, entry->content_size);
        /*restore unselected color*/
        fpga_write_address(fpga_dev, io, REG_COLORA_LSB, &fpga_dev->font_color[COLOR_FONT_DESEL][COLOR_LSB], 2);
        fpga_write_address(fpga_dev, io, REG_COLORB_LSB, &fpga_dev->font_color[COLOR_BACK_DESEL][COLOR_LSB], 2);
    }
    else
    {
        fpga_set_cursor(fpga_dev, io, entry->entry_start_coords[0], entry->entry_start_coords[1]);
        fpga_write_address(fpga_dev, io, (RANGE_SPRITE_BUF + entry->address_offset), entry->entry_content, entry->content_size);
    }

    return ERROR_OK;
}

syscon_error_t menu_erase_entry(struct fpga_dev *fpga_dev, struct SysCon_Pins *io, struct menu_entry *entry)
{
    /*write all 0x00 into each entry byte and reset the address offset to indicate that the entry was deleted*/
    fpga_erase_data(fpga_dev, io, (RANGE_SPRITE_BUF + entry->address_offset), entry->content_size);
    entry->address_offset = 0;

    return ERROR_OK;
}

syscon_error_t menu_set_cursor(struct fpga_dev *fpga_dev, struct SysCon_Pins *io, struct menu *menu, uint8_t updown)
{

    uint8_t next_selected_entry = 0;
    /*overwrite the current entry with the default color*/
    menu_draw_entry(fpga_dev, io, &menu->menu_entry[menu->selected_entry], SET_DESELECTED);
    /*handle all selection edge cases to keep the cursor in bounds*/
    /*updown = 0 means decrement the cursor*/
    /*updown = 1 means increment the cursor*/
    /*skip entries with the skip_cursor flag enabled*/
    if ((updown == SET_CURSOR_UP))
    {
        /*up*/
        if (menu->selected_entry < menu->used_entries)
            next_selected_entry = menu->selected_entry + 1;
        for (uint8_t i = next_selected_entry; i < menu->used_entries; i++)
        {
            if (menu->menu_entry[i].skip_cursor == NO_SKIP)
            {
                menu->selected_entry = i;
                break;
            }
        }
    }
    else
    {
        /*down*/
        if (menu->selected_entry > 0)
            next_selected_entry = menu->selected_entry - 1;
        for (uint8_t i = next_selected_entry; i >= 0; i--)
        {
            if (menu->menu_entry[i].skip_cursor == NO_SKIP)
            {
                menu->selected_entry = i;
                break;
            }
        }
    }

    /*draw the next entry in the selecton color*/
    menu_draw_entry(fpga_dev, io, &menu->menu_entry[menu->selected_entry], SET_SELECTED);

    return ERROR_OK;
}

syscon_error_t menu_draw_page(struct menu_dev *menu_dev, struct SysCon_Pins *io)
{
    /*
    the currently displayed menu is the previous menu
    the next menu (which was drawn above) is the current menu
    */
    menu_dev->previous_menu = menu_dev->current_menu;
    menu_dev->current_menu = menu_dev->next_menu;
    /*set the cursor to element 0 of the menu*/
    // fpga_dev->menu[fpga_dev->current_menu].selected_entry = 0;

    menu_erase_page(menu_dev, io);
    for (uint8_t i = 0; i < menu_dev->menu[menu_dev->next_menu].used_entries; i++)
    {
        if (i == menu_dev->menu[menu_dev->next_menu].selected_entry)
            menu_draw_entry(&menu_dev->fpga_dev, io, &menu_dev->menu[menu_dev->next_menu].menu_entry[i], 1); // find the selected entry and display it
        else
            menu_draw_entry(&menu_dev->fpga_dev, io, &menu_dev->menu[menu_dev->next_menu].menu_entry[i], 0); // all other entries are not selected
    }

    return ERROR_OK;
}

syscon_error_t menu_erase_page(struct menu_dev *menu_dev, struct SysCon_Pins *io)
{
    /*how to erase previous menu page?*/
    for (uint8_t i = 0; i < menu_dev->menu[menu_dev->previous_menu].used_entries; i++)
    {
        menu_erase_entry(&menu_dev->fpga_dev, io, &menu_dev->menu[menu_dev->previous_menu].menu_entry[i]); // all other entries are not selected
    }
    /*erase the current address in the spritebuffer RAM to start from address 0 when drawing the next entry*/
    menu_dev->fpga_dev.current_spritebuffer_addr = 0;

    return ERROR_OK;
}

syscon_error_t menu_run(struct power_dev *power_dev, uint16_t systick)
{
    syscon_error_t result = ERROR_OK;

    switch (menu_actions(&power_dev->menu_dev, power_dev->gp_dev.current_digital_states, power_dev->gp_dev.previous_digital_states))
    {
    case DO_NOTHING:
        break;
    case ENTER_ENTRY:
        power_dev->menu_dev.next_menu = power_dev->menu_dev.menu[power_dev->menu_dev.current_menu].menu_entry[power_dev->menu_dev.menu[power_dev->menu_dev.current_menu].selected_entry].menu_to_lanuch;
        power_dev->menu_dev.menu[power_dev->menu_dev.next_menu].selected_entry = 0;
        result = menu_draw_page(&power_dev->menu_dev, &power_dev->io);
        break;
    case GO_BACK:
        power_dev->menu_dev.next_menu = power_dev->menu_dev.menu[power_dev->menu_dev.current_menu].gobackwhere;
        power_dev->menu_dev.menu[power_dev->menu_dev.next_menu].selected_entry = 0;
        result = menu_draw_page(&power_dev->menu_dev, &power_dev->io);
        break;
    case CURSOR_UP:
        result = menu_set_cursor(&power_dev->menu_dev.fpga_dev, &power_dev->io, &power_dev->menu_dev.menu[power_dev->menu_dev.current_menu], SET_CURSOR_UP);
        break;
    case CURSOR_DOWN:
        result = menu_set_cursor(&power_dev->menu_dev.fpga_dev, &power_dev->io, &power_dev->menu_dev.menu[power_dev->menu_dev.current_menu], SET_CURSOR_DOWN);
        break;
    case VOLUME_HP:
        result = amp_inc_dec_hp_volume(&power_dev->amp_dev, power_dev->menu_dev.action_button_state);
        sprintf(entry_hp_volume_display, "HP Volume: %2u", power_dev->amp_dev.headphone_volume);
        break;
    case VOLUME_SP:
        result = amp_inc_dec_sp_volume(&power_dev->amp_dev, power_dev->menu_dev.action_button_state);
        sprintf(entry_sp_volume_display, "SP Volume: %2u", power_dev->amp_dev.speaker_volume);
        break;
    case BRIGHTNESS_CONTROL:
        result = fpga_set_brightness(&power_dev->menu_dev.fpga_dev, &power_dev->io, power_dev->menu_dev.action_button_state);
        sprintf(entry_brightness_display, "Brightness: %3u%%", power_dev->menu_dev.fpga_dev.brightness);
        break;
    case PICTURE_X_MOVE:
        result = fpga_set_image_offset(&power_dev->menu_dev.fpga_dev, &power_dev->io, power_dev->menu_dev.action_button_state, UPDATE_X);
        sprintf(entry_video_position_x, "X Position: %3u", power_dev->menu_dev.fpga_dev.video_config[power_dev->menu_dev.fpga_dev.current_resolution][NO_H_IMAGE_OFFSET]);
        break;
    case PICTURE_Y_MOVE:
        result = fpga_set_image_offset(&power_dev->menu_dev.fpga_dev, &power_dev->io, power_dev->menu_dev.action_button_state, UPDATE_Y);
        sprintf(entry_video_position_y, "Y Position: %3u", power_dev->menu_dev.fpga_dev.video_config[power_dev->menu_dev.fpga_dev.current_resolution][NO_V_IMAGE_OFFSET]);
        break;
    case VIDEO_OUT_RES_X:
        result = fpga_set_xy_resolution(&power_dev->menu_dev.fpga_dev, &power_dev->io, power_dev->menu_dev.action_button_state, RES_OUT_X);
        sprintf(entry_video_res_x_out, "Output Res X: %3u", power_dev->menu_dev.fpga_dev.video_config[power_dev->menu_dev.fpga_dev.current_resolution][NO_TARGET_RES_X]);
        break;
    case VIDEO_OUT_RES_Y:
        result = fpga_set_xy_resolution(&power_dev->menu_dev.fpga_dev, &power_dev->io, power_dev->menu_dev.action_button_state, RES_OUT_Y);
        sprintf(entry_video_res_y_out, "Output Res Y: %3u", power_dev->menu_dev.fpga_dev.video_config[power_dev->menu_dev.fpga_dev.current_resolution][NO_TARGET_RES_Y]);
        break;
    case VIDEO_IN_RES_X:
        result = fpga_set_xy_resolution(&power_dev->menu_dev.fpga_dev, &power_dev->io, power_dev->menu_dev.action_button_state, RES_IN_X);
        sprintf(entry_video_res_x_in, "Input Res X: %3u", power_dev->menu_dev.fpga_dev.video_config[power_dev->menu_dev.fpga_dev.current_resolution][NO_H_ACTIVE_PXL]);
        break;
    case VIDEO_IN_RES_Y:
        result = fpga_set_xy_resolution(&power_dev->menu_dev.fpga_dev, &power_dev->io, power_dev->menu_dev.action_button_state, RES_IN_Y);
        sprintf(entry_video_res_y_in, "Input Res Y: %3u", power_dev->menu_dev.fpga_dev.video_config[power_dev->menu_dev.fpga_dev.current_resolution][NO_V_IMAGE_ACTIVE]);
        break;
    case PHASE:
        result = fpga_set_phase(&power_dev->menu_dev.fpga_dev, &power_dev->io, power_dev->menu_dev.action_button_state);
        sprintf(entry_video_phase, "Horizontal Phase: %1u", power_dev->menu_dev.fpga_dev.video_config[power_dev->menu_dev.fpga_dev.current_resolution][NO_PHASE]);
        break;
    case DEINTERLACER_DBG_TOGGLE:
        result = fpga_toggle_deinterlacer_debug(&power_dev->menu_dev.fpga_dev, &power_dev->io);
        if (power_dev->menu_dev.fpga_dev.video_config[power_dev->menu_dev.fpga_dev.current_resolution][NO_DEINTERLACER_DEBUG] == 1){
            sprintf(entry_deinterlacer_debug, "Deinterlacer Debug: ON ");
        }
        else{
            sprintf(entry_deinterlacer_debug, "Deinterlacer Debug: OFF");
        }
        break;
    case MOTION_THR:
        result = fpga_set_motion_threshold(&power_dev->menu_dev.fpga_dev, &power_dev->io, power_dev->menu_dev.action_button_state);
        sprintf(entry_motion_thr, "Motion Threshold: %3u", power_dev->menu_dev.fpga_dev.video_config[power_dev->menu_dev.fpga_dev.current_resolution][NO_MOTION_THRESHOLD]);
        break;
    case MAGH:
        result = fpga_set_sampling_divider(&power_dev->menu_dev.fpga_dev, &power_dev->io, power_dev->menu_dev.action_button_state);
        sprintf(entry_video_res_x_in, "Input Res X: %3u", power_dev->menu_dev.fpga_dev.video_config[power_dev->menu_dev.fpga_dev.current_resolution][NO_H_ACTIVE_PXL]);
        sprintf(entry_video_position_x, "X Position: %3u", power_dev->menu_dev.fpga_dev.video_config[power_dev->menu_dev.fpga_dev.current_resolution][NO_H_IMAGE_OFFSET]);
        sprintf(entry_MAGH, "MAGH: %1u", power_dev->menu_dev.fpga_dev.video_config[power_dev->menu_dev.fpga_dev.current_resolution][NO_SAMPLING_DIVIDER]);
        break;
    case MAGH_AUTO:
        if(power_dev->menu_dev.fpga_dev.magh_autodetect_enable == 0)
        {
            power_dev->menu_dev.fpga_dev.magh_autodetect_enable = 1;
            sprintf(entry_MAGH_autodetect, "MAGH Autodetect: ON ");
        }
        else
        {
            power_dev->menu_dev.fpga_dev.magh_autodetect_enable = 0;
            sprintf(entry_MAGH_autodetect, "MAGH Autodetect: OFF");
        }
        break;
    case JOYSTICK_SET_CENTER:
        center_analog_sticks(&power_dev->gp_dev);
        break;
    case JOYSTICK_CALIBRATE:
        calibrate_analog_sticks(&power_dev->gp_dev);
        break;
    case MENU_STORE_SETTINGS:
        menu_store_settings(power_dev, power_dev->settings);
        sprintf(entry_store_inputs, "Store Settings OK");
        break;
    case JOYSTICK_INVERT:
        invert_stick(&power_dev->gp_dev);
        break;
    case DEADZONE_ADJUST:
        inc_dec_deadzone(&power_dev->gp_dev, power_dev->menu_dev.action_button_state);
        sprintf(entry_stick_deadzone, "Deadzone: %3u", power_dev->gp_dev.analog_stick[GP_RIGHT].axes[0].deadzone);
        break;
    case FAN_LOW_TEMP:
        update_fan_curve(&power_dev->fan_dev.fan_low_temp, power_dev->menu_dev.action_button_state);
        fan_calculate_constants(&power_dev->fan_dev);
        sprintf(entry_low_temp, "Fan Low Temp: %2u", power_dev->fan_dev.fan_low_temp);
        break;
    case FAN_HIGH_TEMP:
        update_fan_curve(&power_dev->fan_dev.fan_high_temp, power_dev->menu_dev.action_button_state);
        fan_calculate_constants(&power_dev->fan_dev);
        sprintf(entry_high_temp, "Fan High Temp: %2u", power_dev->fan_dev.fan_high_temp);
        break;
    case PS1_ANALOG_TOGGLE:
        analog_emulation_toggle(&power_dev->gp_dev);
        if (power_dev->gp_dev.ps1_analog_en == 1)
            sprintf(entry_analog_emulate, "PS1 Analog: ON ");
        else
            sprintf(entry_analog_emulate, "PS1 Analog: OFF");
        break;
    case RUMBLE_INTENSITY_CHANGE:
        inc_dec_setting(&power_dev->gp_dev.rumble_intensity, power_dev->menu_dev.action_button_state, 1, 3, 0);
        sprintf(entry_rumble_intensity, "Rumble Intensity: %2u", power_dev->gp_dev.rumble_intensity);
        break;
    case DO_REFRESH:
        result = menu_refresh(power_dev, systick);
        break;

    default:
        break;
    }

    return result;
}

syscon_error_t menu_store_settings(struct power_dev *power_dev, uint32_t *flash_setting)
{
    flash_setting[NO_SPEAKER_VOLUME] = power_dev->amp_dev.speaker_volume;
    flash_setting[NO_HEADPHONE_VOLUME] = power_dev->amp_dev.headphone_volume;
    flash_setting[NO_DISP_BRIGHTNESS] = power_dev->menu_dev.fpga_dev.brightness;
    flash_setting[NO_FAN_TEMP_LOW] = power_dev->fan_dev.fan_low_temp;
    flash_setting[NO_FAN_TEMP_HIGH] = power_dev->fan_dev.fan_high_temp;
    flash_setting[NO_AX0_DEADZONE] = power_dev->gp_dev.analog_stick[GP_RIGHT].axes[0].deadzone;
    flash_setting[NO_AX1_DEADZONE] = power_dev->gp_dev.analog_stick[GP_RIGHT].axes[1].deadzone;
    flash_setting[NO_AX2_DEADZONE] = power_dev->gp_dev.analog_stick[GP_LEFT].axes[0].deadzone;
    flash_setting[NO_AX3_DEADZONE] = power_dev->gp_dev.analog_stick[GP_LEFT].axes[1].deadzone;
    flash_setting[NO_RUMBLE_INTENSITY] = power_dev->gp_dev.rumble_intensity;
    flash_setting[NO_MAGH_AUTODETECT] = power_dev->menu_dev.fpga_dev.magh_autodetect_enable;
    
    store_stick_calibration(&power_dev->gp_dev, flash_setting);
    if (fpga_copy_video_cfg(&power_dev->menu_dev.fpga_dev, flash_setting, RES_STORE) != ERROR_OK)
        return FPGA_COMMAND_FAILED;

    power_dev->store_settings_flag = 1;

    return ERROR_OK;
}

syscon_error_t menu_refresh(struct power_dev *power_dev, uint16_t systick)
{
    uint16_t elapsed_ticks = 0;

    if (power_dev->menu_dev.refresh_flag == 0)
    {
        power_dev->menu_dev.refresh_flag = 1;
        power_dev->menu_dev.start_systick = systick;
    }
    else
    {
        /*handle overflow in tick counter and set the elapsed ticks accordingly*/
        if ((systick < power_dev->menu_dev.start_systick))
            elapsed_ticks = (65536 - power_dev->menu_dev.start_systick) + systick;
        else
            elapsed_ticks = systick - power_dev->menu_dev.start_systick;

        if (elapsed_ticks > REFRESH_INTERVAL)
        {
            /*reading the resolution regularly is important, the entries will be displayed based on that*/
            if (fpga_get_current_resolution(&power_dev->menu_dev.fpga_dev, &power_dev->io, &power_dev->menu_dev.fpga_dev.current_resolution) != ERROR_OK)
                return ERROR_MENU_REFRESH;
            /*refreshing all entries should be done after getting the resolution*/
            update_interactive_entries(power_dev);
            if (menu_refresh_entries(&power_dev->menu_dev, &power_dev->io) != ERROR_OK)
                return ERROR_MENU_REFRESH;
            /*lastly, reset the refresh flag to start new cycle*/
            power_dev->menu_dev.refresh_flag = 0;
        }
    }

    return ERROR_OK;
}

syscon_error_t menu_refresh_entries(struct menu_dev *menu_dev, struct SysCon_Pins *io)
{
    for (uint8_t i = 0; i < menu_dev->menu[menu_dev->current_menu].used_entries; i++)
    {
        /*this is essentially just overwriting the entry with the new strings, instead of erasing the entry and redrawing it, saves half the comms*/
        if (i == menu_dev->menu[menu_dev->current_menu].selected_entry)
            menu_draw_entry(&menu_dev->fpga_dev, io, &menu_dev->menu[menu_dev->current_menu].menu_entry[i], 1); // find the selected entry and display it
        else
            menu_draw_entry(&menu_dev->fpga_dev, io, &menu_dev->menu[menu_dev->current_menu].menu_entry[i], 0); // all other entries are not selected
    }
    return ERROR_OK;
}

void update_interactive_entries(struct power_dev *power_dev)
{

    /*clipping inputs to avoid mismatch in array elements*/
    float soc = (power_dev->MAX_Dev.MAX_batt_properties.state_of_charge > 100.0) ? 100.0 : power_dev->MAX_Dev.MAX_batt_properties.state_of_charge;
    soc = (power_dev->MAX_Dev.MAX_batt_properties.state_of_charge < 0.0) ? 0.0 : power_dev->MAX_Dev.MAX_batt_properties.state_of_charge;

    float temperature = (power_dev->fan_dev.ee_temperature > 99.9) ? 99.9 : power_dev->fan_dev.ee_temperature;
    temperature = (power_dev->fan_dev.ee_temperature < -99.9) ? -99.9 : power_dev->fan_dev.ee_temperature;

    float power = (power_dev->MAX_Dev.MAX_batt_properties.battery_power > 99.9) ? 99.9 : power_dev->MAX_Dev.MAX_batt_properties.battery_power;
    power = (power_dev->MAX_Dev.MAX_batt_properties.battery_power < -99.9) ? -99.9 : power_dev->MAX_Dev.MAX_batt_properties.battery_power;

    float ttf = (power_dev->MAX_Dev.MAX_batt_properties.time_to_full > 9999.9) ? 9999.9 : power_dev->MAX_Dev.MAX_batt_properties.time_to_full;
    ttf = (power_dev->MAX_Dev.MAX_batt_properties.time_to_full < 0.0) ? 0.0 : power_dev->MAX_Dev.MAX_batt_properties.time_to_full;

    float tte = (power_dev->MAX_Dev.MAX_batt_properties.time_to_empty > 9999.9) ? 9999.9 : power_dev->MAX_Dev.MAX_batt_properties.time_to_empty;
    tte = (power_dev->MAX_Dev.MAX_batt_properties.time_to_empty < 0.0) ? 0.0 : power_dev->MAX_Dev.MAX_batt_properties.time_to_empty;

    float voltage = (power_dev->MAX_Dev.MAX_batt_properties.pack_voltage > 9.9) ? 9.9 : power_dev->MAX_Dev.MAX_batt_properties.pack_voltage;
    voltage = (power_dev->MAX_Dev.MAX_batt_properties.pack_voltage < 0.0) ? 0.0 : power_dev->MAX_Dev.MAX_batt_properties.pack_voltage;

    /*writing the clipped emelents into the display buffers*/
    /*main menu entries*/
    sprintf(menu_main_soc, "SOC: %5.1f%%", soc);
    sprintf(menu_main_temperature, "EE TEMP: %5.1f", temperature);
    // static char entry_ttf_display[] = "Time to Full: 9999.9min";
    // static char entry_tte_display[] = "Time to Empty: 9999.9min";
    if (power_dev->BQ_Dev.BQ_CHGStatus.vbus_present_stat == 1)
    {
        sprintf(menu_main_charging, "CHARGE   ");
        sprintf(entry_ttf_display, "Time to Full: %6.1fmin", ttf);
        sprintf(entry_tte_display, "Time to Empty: n/a      ");
    }
    else
    {
        sprintf(menu_main_charging, "DISCHARGE");
        sprintf(entry_tte_display, "Time to Empty: %6.1fmin", tte);
        sprintf(entry_ttf_display, "Time to Full: n/a      ");
    }
    /*monitoring entries*/
    sprintf(entry_power_display, "Battery Power: %5.1fW", power);
    sprintf(entry_pack_voltage, "Battery Voltage: %3.1fV", voltage);

    /*resolution stuff*/
    switch (power_dev->menu_dev.fpga_dev.current_resolution)
    {
    case RES_512i:
        sprintf(entry_resolution, "Current Resolution: 512i");
        break;
    case RES_480i:
        sprintf(entry_resolution, "Current Resolution: 480i");
        break;
    case RES_480p:
        sprintf(entry_resolution, "Current Resolution: 480p");
        break;
    case RES_240p:
        sprintf(entry_resolution, "Current Resolution: 240p");
        break;
    case RES_256p:
        sprintf(entry_resolution, "Current Resolution: 256p");
        break;
    default:
        break;
    }

    sprintf(entry_video_res_x_in, "Input Res X: %3u", power_dev->menu_dev.fpga_dev.video_config[power_dev->menu_dev.fpga_dev.current_resolution][NO_H_ACTIVE_PXL]);
    sprintf(entry_video_position_x, "X Position: %3u", power_dev->menu_dev.fpga_dev.video_config[power_dev->menu_dev.fpga_dev.current_resolution][NO_H_IMAGE_OFFSET]);
    sprintf(entry_MAGH, "MAGH: %1u", power_dev->menu_dev.fpga_dev.video_config[power_dev->menu_dev.fpga_dev.current_resolution][NO_SAMPLING_DIVIDER]);
}

uint8_t menu_actions(struct menu_dev *menu_dev, uint16_t buttonstates, uint16_t prev_buttonstates)
{
    uint8_t result = DO_NOTHING;

    /*set the result to the default function to execute in the menu*/
    result = menu_dev->menu[menu_dev->current_menu].function_to_execute;

    // X Button
    if (((buttonstates & SEL_MASK) == 0) && ((prev_buttonstates & SEL_MASK) != 0))
    {
        if (menu_dev->lock_sel == 0)
            result = menu_dev->menu[menu_dev->current_menu].menu_entry[menu_dev->menu[menu_dev->current_menu].selected_entry].interaction[0];
        menu_dev->lock_sel = 1;
        menu_dev->action_button_state = ACTION_SEL;
    }
    else
        menu_dev->lock_sel = 0;

    // O Button
    if (((buttonstates & BACK_MASK) == 0) && ((prev_buttonstates & BACK_MASK) != 0))
    {
        if (menu_dev->lock_back == 0)
            result = menu_dev->menu[menu_dev->current_menu].menu_entry[menu_dev->menu[menu_dev->current_menu].selected_entry].interaction[1];
        menu_dev->lock_back = 1;
        menu_dev->action_button_state = ACTION_BACK;
    }
    else
        menu_dev->lock_back = 0;

    // up
    if (((buttonstates & UP_MASK) == 0) && ((prev_buttonstates & UP_MASK) != 0))
    {
        if (menu_dev->lock_up == 0)
            result = menu_dev->menu[menu_dev->current_menu].menu_entry[menu_dev->menu[menu_dev->current_menu].selected_entry].interaction[2];
        menu_dev->lock_up = 1;
        menu_dev->action_button_state = ACTION_UP;
    }
    else
        menu_dev->lock_up = 0;

    // down
    if (((buttonstates & DOWN_MASK) == 0) && ((prev_buttonstates & DOWN_MASK) != 0))
    {
        if (menu_dev->lock_down == 0)
            result = menu_dev->menu[menu_dev->current_menu].menu_entry[menu_dev->menu[menu_dev->current_menu].selected_entry].interaction[3];
        menu_dev->lock_down = 1;
        menu_dev->action_button_state = ACTION_DOWN;
    }
    else
        menu_dev->lock_down = 0;

    // left
    if (((buttonstates & LEFT_MASK) == 0) && ((prev_buttonstates & LEFT_MASK) != 0))
    {
        if (menu_dev->lock_left == 0)
            result = menu_dev->menu[menu_dev->current_menu].menu_entry[menu_dev->menu[menu_dev->current_menu].selected_entry].interaction[4];
        menu_dev->lock_left = 1;
        menu_dev->action_button_state = ACTION_LEFT;
    }
    else
        menu_dev->lock_left = 0;

    // right
    if (((buttonstates & RIGHT_MASK) == 0) && ((prev_buttonstates & RIGHT_MASK) != 0))
    {
        if (menu_dev->lock_right == 0)
            result = menu_dev->menu[menu_dev->current_menu].menu_entry[menu_dev->menu[menu_dev->current_menu].selected_entry].interaction[5];
        menu_dev->lock_right = 1;
        menu_dev->action_button_state = ACTION_RIGHT;
    }
    else
        menu_dev->lock_right = 0;

    return result;
}

#ifndef MENU_H
#define MENU_H

#include "MCU_interface.h"

#include "helper_functions.h"
#include "STUSB4500.h"
#include "BQ25792.h"
#include "MAX17320.h"
#include "LM49450.h"
#include "gamepad_user.h"
#include "gamepad_core.h"
#include "flash.h"
#include "fan.h"
#include "power_led.h"
#include "fpga.h"


#define COLOR_A                     0
#define COLOR_B                     1

#define SKIP                        1
#define NO_SKIP                     0

/*button interaction masks*/
#define SEL_MASK                    (0x01 << BUTTON_CROSS)
#define BACK_MASK                   (0x01 << BUTTON_CIRCLE)
#define UP_MASK                     (0x01 << BUTTON_UP)
#define DOWN_MASK                   (0x01 << BUTTON_DOWN)
#define LEFT_MASK                   (0x01 << BUTTON_LEFT)
#define RIGHT_MASK                  (0x01 << BUTTON_RIGHT)

/*menu stuff*/
#define MAX_MENU_ENTRIES            8
#define MENU_ENTRY_LENGTH           100
#define MAX_INTERACTION_BUTTONS     6
#define MENU_OFFSET_X               10
#define SET_CURSOR_UP               1
#define SET_CURSOR_DOWN             0
#define SET_SELECTED              1
#define SET_DESELECTED              0
#define SEL_INTER                   0
#define BACK_INTER                  1
#define UP_INTER                    2
#define DOWN_INTER                  3
#define LEFT_INTER                  4
#define RIGHT_INTER                 5

#define REFRESH_INTERVAL            3   //5 -> 5*100ms


#define MENU_LINE_1_YPOS            10
#define MENU_LINE_2_YPOS            MENU_LINE_1_YPOS + 18
#define MENU_LINE_3_YPOS            MENU_LINE_2_YPOS + 18
#define MENU_LINE_4_YPOS            MENU_LINE_3_YPOS + 18
#define MENU_LINE_5_YPOS            MENU_LINE_4_YPOS + 18
#define MENU_LINE_6_YPOS            MENU_LINE_5_YPOS + 18
#define MENU_LINE_7_YPOS            MENU_LINE_6_YPOS + 18
#define MENU_LINE_8_YPOS            MENU_LINE_7_YPOS + 18

#define DEG_SIGN                    UINT8_C(176)

/*contains all information for ONE entry*/
struct menu_entry
{
    char *entry_content;
    uint16_t address_offset;
    uint8_t content_size;
    uint16_t entry_start_coords[2];
    uint8_t interaction[MAX_INTERACTION_BUTTONS];
    uint8_t menu_to_lanuch;
    uint8_t skip_cursor;
};

/*this struct contains the whole menu and logic for ONE menu page*/
struct menu
{
    struct menu_entry menu_entry[MAX_MENU_ENTRIES];
    uint8_t used_entries;
    uint8_t function_to_execute;
    uint8_t selected_entry;
    uint8_t gobackwhere;
};

/*enum containing all currently implemented menu pages, needed for menu logic*/
typedef enum fpga_menu_e
{
    MENU_MAIN = 0,
    MENU_AUDIO,
    MENU_VIDEO,
    MENU_VIDEO_RES,
    MENU_VIDEO_OTHER,
    MENU_INPUT,
    MENU_JOYSTICK_START,
    MENU_JOYSTICK_CALIB,
    MENU_JOYTSICK_INVERT,
    MENU_GENERAL,
    MENU_CREDITS,
    MENU_MONITOR,

    NO_MENUS,

}fpga_menu_t;

/*main struct for the whole menu, contains everything to run it*/
struct menu_dev
{
    /*menu stuff*/
    struct menu menu[NO_MENUS];
    uint8_t next_menu;
    uint8_t current_menu;
    uint8_t previous_menu;
    uint8_t refresh_flag;
    uint16_t start_systick;
    uint8_t menu_active;
    uint16_t action_button_state;

    /*user input flags*/
    uint8_t lock_sel;
    uint8_t lock_back;
    uint8_t lock_up;
    uint8_t lock_down;
    uint8_t lock_left;
    uint8_t lock_right;

    /*fpga struct is here, because it is only related to the menu*/
    struct fpga_dev fpga_dev;

};

/*functions accessed by core0*/
struct power_dev
{
    struct STUSB4500_Dev STUSB_Dev;
    struct BQ25792_Dev BQ_Dev;
    struct MAX17320_Dev MAX_Dev;
    struct SysCon_Pins io;
    struct gamepad_user gp_dev;
    struct speaker_amp amp_dev;
    struct powerLED powerLED;
    struct menu_dev menu_dev;
    struct fan_dev fan_dev;
    syscon_state_t cur_run_state;
    syscon_state_t next_run_state;
    syscon_state_t last_run_state;
    syscon_state_t cur_chg_state;
    syscon_state_t next_chg_state;
    syscon_state_t last_chg_state;
    /*flag to indicate that the power status was polled at least once already*/
    /*never reset explictly, only when resetting the mcu*/
    uint8_t power_status_known;
    uint8_t power_error_flag;
    uint32_t *settings;
    uint8_t store_settings_flag;
};

/*these are the actions the different menu entries can execute upon interaction*/
typedef enum fpga_interaction_e
{
    DO_NOTHING = 0,
    DO_REFRESH,
    ENTER_ENTRY,
    GO_BACK,
    CURSOR_UP,
    CURSOR_DOWN,
    /*audio*/
    VOLUME_HP,
    VOLUME_SP,
    /*video*/
    BRIGHTNESS_CONTROL,
    PICTURE_X_MOVE,
    PICTURE_Y_MOVE,
    PHASE,
    DEINTERLACER_DBG_TOGGLE,
    MOTION_THR,
    VIDEO_IN_RES_X,
    VIDEO_IN_RES_Y,
    VIDEO_OUT_RES_X,
    VIDEO_OUT_RES_Y,
    MAGH,
    MAGH_AUTO,
    /*analog stick*/
    JOYSTICK_INVERT,
    MENU_STORE_SETTINGS,
    DEADZONE_ADJUST,
    PS1_ANALOG_TOGGLE,
    RUMBLE_INTENSITY_CHANGE,
    /*functions to execute in menu*/
    JOYSTICK_SET_CENTER,
    JOYSTICK_CALIBRATE,
    /*fan stuff*/
    FAN_LOW_TEMP,
    FAN_HIGH_TEMP,
    
}fpga_interaction_t;

void menu_fill_struct(struct menu_dev *menu_dev);
void entry_add(struct menu *menu, char *content, uint8_t size, uint16_t coords_line, uint16_t coords_column, uint8_t skip, uint8_t launch_menu, uint8_t sel_interaction, uint8_t back_interaction, uint8_t up_interaction, uint8_t down_interaction, uint8_t left_interaction, uint8_t right_interaction);
syscon_error_t menu_init(struct power_dev *power_dev);
syscon_error_t menu_cleanup(struct menu_dev *menu_dev, struct SysCon_Pins *io);
syscon_error_t menu_draw_entry(struct fpga_dev *fpga_dev, struct SysCon_Pins *io, struct menu_entry *entry, uint8_t selection);
syscon_error_t menu_erase_entry(struct fpga_dev *fpga_dev, struct SysCon_Pins *io, struct menu_entry *entry);
syscon_error_t menu_set_cursor(struct fpga_dev *fpga_dev, struct SysCon_Pins *io, struct menu *menu, uint8_t updown);
syscon_error_t menu_draw_page(struct menu_dev *menu_dev, struct SysCon_Pins *io);
syscon_error_t menu_erase_page(struct menu_dev *menu_dev, struct SysCon_Pins *io);
syscon_error_t menu_run(struct power_dev *power_dev, uint16_t systick);
syscon_error_t menu_refresh(struct power_dev *power_dev, uint16_t systick);
uint8_t menu_actions(struct menu_dev *menu_dev, uint16_t buttonstates, uint16_t prev_buttonstates);
void update_interactive_entries(struct power_dev *power_dev);
syscon_error_t menu_store_settings(struct power_dev *power_dev, uint32_t *flash_setting);
syscon_error_t menu_refresh_entries(struct menu_dev *menu_dev, struct SysCon_Pins *io);





#endif
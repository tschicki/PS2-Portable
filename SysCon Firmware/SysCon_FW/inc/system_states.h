
#ifndef system_states_h
#define system_states_h

#include <stdint.h>
#include "MCU_interface.h"


typedef enum syscone_state_e
{
    STATE_SYS_DEFAULT = 0,
    STATE_INIT,
    STATE_WAIT_USER,
    STATE_BOOT,
    STATE_SHUTDOWN,
    STATE_RUN,
    STATE_CONFIG,
    STATE_STORE_SETTINGS,
    STATE_LOAD_SETTINGS,
    STATE_OFF,
    STATE_WRITE_CONFIG,
    STATE_RUN_ERROR,  
    STATE_RUN_HALT,
    //charging states*/
    STATE_CHARGE_DEFAULT,
    STATE_WAIT_FOR_STATUS,
    STATE_GET_POWER_STATUS,
    STATE_CHARGE_INIT,
    STATE_CHARGE,
    STATE_CHARGE_ERROR,

    SYSCON_STATE_COUNT,
}syscon_state_t;


// Array of strings to map enum error types to printable strings
// - see important NOTE above!
//const char* const SYSCON_STATE_STRS[] = 
static char* SYSCON_STATE_STRS[] = 
{
    "STATE_SYS_DEFAULT",
    "STATE_INIT",
    "STATE_WAIT_USER",
    "STATE_BOOT",
    "STATE_SHUTDOWN",
    "STATE_RUN",
    "STATE_CONFIG",
    "STATE_STORE_SETTINGS",
    "STATE_LOAD_SETTINGS",
    "STATE_OFF",
    "STATE_WRITE_CONFIG",
    "STATE_RUN_ERROR",
    "STATE_RUN_HALT",
    //charging states*/
    "STATE_CHARGE_DEFAULT",
    "STATE_WAIT_FOR_STATUS",
    "STATE_GET_POWER_STATUS",
    "STATE_CHARGE_INIT",
    "STATE_CHARGE",
    "STATE_CHARGE_ERROR",
};

_Static_assert(ARRAY_LEN(SYSCON_STATE_STRS) == SYSCON_STATE_COUNT,
    "You must keep your `syscon_state_t` enum and your "
    "`SYSCON_STATE_STRS` array in-sync!");

syscon_state_t syscon_state_str(syscon_state_t new_state, syscon_state_t cur_state);

#endif /* INC_STATES_H_ */

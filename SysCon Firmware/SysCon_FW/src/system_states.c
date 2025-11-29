#include "system_states.h"

syscon_state_t syscon_state_str(syscon_state_t new_state, syscon_state_t cur_state)
{
    // Ensure error codes are within the valid array index range
    if (new_state <= SYSCON_STATE_COUNT){
        #if defined(STATE_DEBUG)
        printf("STATE_CHANGE: %s to %s\r\n", SYSCON_STATE_STRS[cur_state], SYSCON_STATE_STRS[new_state]);
        #endif
        return new_state;
    }
    else{
        #if defined(STATE_DEBUG)
        printf("INVALID STATE CHANGE, CHECK THE CODE\r\n");
        #endif
        while(1);
    }
}


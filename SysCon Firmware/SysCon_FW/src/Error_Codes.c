#include "Error_Codes.h"

/// @brief      Function to get a printable string from an enum error type
/// @param[in]  err     a valid error code for this module
/// @return     A printable C string corresponding to the error code input above, or NULL if an invalid error code
///             was passed in
const char* syscon_error_str(syscon_error_t err)
{

    // Ensure error codes are within the valid array index range
    if (err <= SYSCON_ERROR_COUNT){

        printf("ERROR: %c\n",SYSCON_ERROR_STRS[err]);
        return SYSCON_ERROR_STRS[err];
    }
    else{
        printf("INVALID ERROR CODE DEFINITION, CHECK THE CODE\r\n");
        while(1);
    }
}

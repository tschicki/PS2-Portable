#include "main.h"



void run_charge_state_machine(struct power_dev *power_dev);
syscon_state_t run_state_get_power_status(struct BQ25792_Dev *BQ_Dev);
syscon_state_t run_state_charge_init(struct STUSB4500_Dev *STUSB_Dev, struct BQ25792_Dev *BQ_Dev, struct SysCon_Pins *io);
syscon_state_t get_charger_status(struct BQ25792_Dev *BQ_Dev);
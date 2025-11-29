#include "main.h"

void run_power_state_machine(struct power_dev *power_dev);

syscon_state_t run_state_init(struct power_dev *power_dev);
syscon_state_t run_state_wait_user(struct SysCon_Pins *io);
syscon_state_t run_state_boot(struct power_dev *power_dev);
syscon_state_t run_state_run(struct SysCon_Pins *io);
syscon_state_t run_state_config(struct power_dev *power_dev);
syscon_state_t run_state_shutdown(struct SysCon_Pins *io, struct speaker_amp *amp_dev, struct gamepad_user *gp_dev);
syscon_state_t run_state_load_settings(struct power_dev *power_dev);
syscon_state_t run_state_store_settings(struct power_dev *power_dev);
syscon_state_t run_state_off(struct power_dev *power_dev);
syscon_error_t manufacturing_config(struct power_dev *power_dev);

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef main_h
#define main_h

/* Includes ------------------------------------------------------------------*/
#include "MCU_interface.h"
#include "core0_irq.h"
#include "menu.h"
#include "charge_state_machine.h"
#include "power_state_machine.h"

void run_power_polling(struct power_dev *power_dev);
void run_button_polling(struct power_dev *power_dev);




#endif


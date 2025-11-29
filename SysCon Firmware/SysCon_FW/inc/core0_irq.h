#ifndef CORE0_IRQ_H
#define CORE0_IRQ_H

#include "MCU_interface.h"

/*Timer or power management polling IRQ*/
#define ALARM_POLL_POWER            2
#define ALARM_IRQ_POLL_POWER        TIMER_IRQ_2
#define POWER_POLL_INTERVAL_US      100000

#define ALARM_POLL_BUTTON           1
#define ALARM_IRQ_POLL_BUTTON       TIMER_IRQ_1
#define BUTTON_POLL_INTERVAL_US     4000


extern volatile uint8_t poll_power_flag;
extern volatile uint8_t poll_button_flag;
extern volatile uint8_t power_good_flag;
extern volatile uint16_t systick;

extern struct SysCon_Pins io;

void endi_power_good_irq(struct SysCon_Pins *io, bool enable);
static void power_good_alarm_irq(uint gpio, uint32_t events);

void start_power_poll_timer(uint32_t delay_us);
void stop_power_poll_timer(void);
static void power_poll_alarm_irq(void);

void button_poll_alarm_irq(void);
void start_button_poll_timer(uint32_t delay_us);
void stop_button_poll_timer(void);



#endif
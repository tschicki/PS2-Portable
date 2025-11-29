#include "core0_irq.h"


static void power_poll_alarm_irq(void)
{
    // Clear the alarm irq
    hw_clear_bits(&timer_hw->intr, 1u << ALARM_POLL_POWER);
    /*start the polling timer for the next poll event
    and poll inputs*/
    start_power_poll_timer(POWER_POLL_INTERVAL_US);
    poll_power_flag = 1;
    /*increase the tick everytime the function is executed*/
    /*systick is used for LED patterns and menu refreshing*/
    systick++;  
}

void start_power_poll_timer(uint32_t delay_us)
{

    hw_set_bits(&timer_hw->inte, 1u << ALARM_POLL_POWER);
    irq_set_exclusive_handler(ALARM_IRQ_POLL_POWER, power_poll_alarm_irq);

    // Enable the alarm irq
    irq_set_enabled(ALARM_IRQ_POLL_POWER, true);
    // Enable interrupt in block and at processor
    // Alarm is only 32 bits so if trying to delay more
    // than that need to be careful and keep track of the upper
    // bits
    uint64_t target = timer_hw->timerawl + delay_us;
    // Write the lower 32 bits of the target time to the alarm which
    // will arm it
    timer_hw->alarm[ALARM_POLL_POWER] = (uint32_t)target;

    poll_power_flag = 1;
}

void stop_power_poll_timer(void)
{
  irq_set_enabled(ALARM_IRQ_POLL_POWER, false);
}

void endi_power_good_irq(struct SysCon_Pins *io, bool enable)
{
    /*setup interrupt for ATT rising and falling edge detection*/
    gpio_set_irq_enabled_with_callback(io->pin[PIN_DCDC_PG].io_pin, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, enable, &power_good_alarm_irq);
}

static void power_good_alarm_irq(uint gpio, uint32_t events)
{
    // Assume alarm has fired
    if ((gpio == POWER_GOOD_PIN) && (events == GPIO_IRQ_EDGE_RISE))
    {
        power_good_flag = 1;
    }
}

void button_poll_alarm_irq(void)
{
    // Clear the alarm irq
    hw_clear_bits(&timer_hw->intr, 1u << ALARM_POLL_BUTTON);
    /*start the polling timer for the next poll event
    and poll inputs*/
    start_button_poll_timer(BUTTON_POLL_INTERVAL_US);
    poll_button_flag = 1;  
}

void start_button_poll_timer(uint32_t delay_us)
{

    hw_set_bits(&timer_hw->inte, 1u << ALARM_POLL_BUTTON);
    irq_set_exclusive_handler(ALARM_IRQ_POLL_BUTTON, button_poll_alarm_irq);

    // Enable the alarm irq
    irq_set_enabled(ALARM_IRQ_POLL_BUTTON, true);
    // Enable interrupt in block and at processor
    // Alarm is only 32 bits so if trying to delay more
    // than that need to be careful and keep track of the upper
    // bits
    uint64_t target = timer_hw->timerawl + delay_us;
    // Write the lower 32 bits of the target time to the alarm which
    // will arm it
    timer_hw->alarm[ALARM_POLL_BUTTON] = (uint32_t)target;
}

void stop_button_poll_timer(void)
{
    irq_set_enabled(ALARM_IRQ_POLL_BUTTON, false);
}
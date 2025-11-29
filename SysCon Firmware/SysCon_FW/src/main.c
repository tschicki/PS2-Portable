#include "main.h"

/*variables to provide for core1*/
uint16_t digital_states = 0;
uint16_t previous_digital_states = 0;
uint8_t current_analog_states[16] = {0};
uint8_t motor_left_pwm = 0;
uint8_t motor_right_pwm = 0;

/*stuff used by main functions and IRQ*/
/*flag to indicate whether the power management needs to be polled*/
volatile uint8_t poll_power_flag = 0;
volatile uint8_t poll_button_flag = 0;
volatile uint8_t power_good_flag = 0;

/*incremented at every power poll timer irq, currently 100ms*/
volatile uint16_t systick = 0;

int main(void)
{
  #ifdef WATCHDOG_EN
    watchdog_enable(WATCHDOG_TIMEOUT, true);
  #endif

  static struct power_dev power_dev;
  /*initialize the both the run state and charge states*/
  power_dev.cur_run_state = STATE_SYS_DEFAULT;
  power_dev.next_run_state = STATE_SYS_DEFAULT;
  power_dev.last_run_state = STATE_SYS_DEFAULT;

  power_dev.cur_chg_state = STATE_CHARGE_DEFAULT;
  power_dev.next_chg_state = STATE_CHARGE_DEFAULT;
  power_dev.last_chg_state = STATE_CHARGE_DEFAULT;

  power_dev.power_error_flag = 0;

  /*init GPIO*/
  MX_GPIO_Init(&power_dev.io);
  /*initialize interfaces for debugging*/
  MX_UART_Init(UART);
  /*initial state for system*/
  power_dev.cur_run_state = syscon_state_str(STATE_INIT, power_dev.cur_run_state);

  while (1)
  {
    //run_state_machine(&power_dev);
    run_power_state_machine(&power_dev);
    run_charge_state_machine(&power_dev);
    
    if (poll_power_flag == 1)
      run_power_polling(&power_dev);

    if (poll_button_flag == 1)
      run_button_polling(&power_dev);

    #ifdef WATCHDOG_EN
      watchdog_update();
    #endif
  }
}


void run_power_polling(struct power_dev *power_dev)
{
  /*should be executed very POWER_POLL_INTERVAL_US when enabled*/
  if(BQ_read_ADC(&power_dev->BQ_Dev) != ERROR_OK)
    power_dev->power_error_flag = 1;
  if(BQ_getFault_Flag(&power_dev->BQ_Dev) != ERROR_OK)
    power_dev->power_error_flag = 1;
  if(BQ_getCHG_Status(&power_dev->BQ_Dev) != ERROR_OK)
    power_dev->power_error_flag = 1;

  #ifndef DEBUG_IGNORE_MAX
  if(MAX_get_batt_properties(&power_dev->MAX_Dev) != ERROR_OK)
    power_dev->power_error_flag = 1;
  if(MAX_get_status(&power_dev->MAX_Dev) != ERROR_OK)
    power_dev->power_error_flag = 1;
  #endif

  if((power_dev->cur_run_state == STATE_RUN) || (power_dev->cur_run_state == STATE_CONFIG))
  {
    /*sample temperature, apply fan curve and update PWM counts - ONLY when the PS2 is on*/
    run_fan_control(&power_dev->fan_dev);
    /*shut down system if EE gets too hot*/
    if(power_dev->fan_dev.ee_temperature > 50.0)
      power_dev->power_error_flag = 1;
  }
  /*update power LED with state of charge*/
  set_power_led(power_dev->MAX_Dev.MAX_batt_properties.state_of_charge, &power_dev->powerLED);
  handle_led_patterns(&power_dev->powerLED, &power_dev->gp_dev.extio_dev[GP_RIGHT], systick);
  /*check whether headphones were plugged*/
  power_dev->amp_dev.hp_detect_state = sys_get_pin(power_dev->io.pin[PIN_HP_DETECT]);

  poll_power_flag = 0;
  power_dev->power_status_known = 1;
}

void run_button_polling(struct power_dev *power_dev)
{

  gamepad_user_read_buttons(&power_dev->gp_dev);
  gamepad_user_set_rumble(&power_dev->gp_dev, motor_right_pwm, motor_left_pwm);

  if(power_dev->cur_run_state != STATE_CONFIG){
    digital_states = power_dev->gp_dev.current_digital_states;
    previous_digital_states = power_dev->gp_dev.previous_digital_states;

    for(uint8_t i = 0; i < 16; i++) 
      current_analog_states[i] = power_dev->gp_dev.current_analog_states[i];

  }

  poll_button_flag = 0;
}


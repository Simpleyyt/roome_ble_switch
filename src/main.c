#include <stdbool.h>
#include <stdint.h>
#include "nrf_gpio.h"
#include "nrf_drv_gpiote.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "app_pwm.h"
#include "app_simple_timer.h"
#include "nrf_delay.h"

#define GPIO_SERVO_ENABLE 14
#define GPIO_SERVO_1 13
#define GPIO_SERVO_2 12
#define GPIO_SERVO_3 10

#define GPIO_KEY_AN 22
#define GPIO_KEY1 8
#define GPIO_KEY2 9
#define GPIO_KEY3 10
#define GPIO_SW2 20

#define GPIO_LED_RED 28
#define GPIO_LED_BLUE 25

#define GPIO_BAT_LED 6
#define GPIO_BAT_ADC_EN 5
#define GPIO_BAT_CHRG 18

#define SERVO_TIME (500 * 125) // 500ms
#define SERVO_MIDDLE_DUTY 8

#define GPIO_MI_SWITCH GPIO_KEY1

APP_PWM_INSTANCE(PWM0, 0);

void timeout_handler(void *p_context);

void set_red_led(int on)
{
    nrf_gpio_pin_write(GPIO_LED_RED, on ? 0 : 1);
}

void set_blue_led(int on)
{
    nrf_gpio_pin_write(GPIO_LED_BLUE, on ? 0 : 1);
}

void set_servo(int level)
{
    ret_code_t err_code;
    
    app_pwm_enable(&PWM0);
    
    nrf_gpio_pin_write(GPIO_SERVO_ENABLE, 1);
    app_pwm_channel_duty_set(&PWM0, 0, SERVO_MIDDLE_DUTY + level); // 6 ~ 10
    
    err_code = app_simple_timer_start(APP_SIMPLE_TIMER_MODE_SINGLE_SHOT, timeout_handler, SERVO_TIME, NULL);
    APP_ERROR_CHECK(err_code);
}

void update_servo() {
    if (nrf_gpio_pin_read(GPIO_MI_SWITCH)) {
        // Turn on
        set_blue_led(false);
        set_servo(2);
        NRF_LOG_INFO("Light on\n");
    }
    else {
        // Turn off
        set_blue_led(true);
        set_servo(-2);
        NRF_LOG_INFO("Light off\n");
    }
}

void gpio_event_handler(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
    switch (pin) {
        case GPIO_MI_SWITCH:
            update_servo();
        break;
        case GPIO_BAT_CHRG:
            nrf_gpio_pin_write(GPIO_BAT_LED, !nrf_gpio_pin_read(GPIO_BAT_CHRG));
            break;
        default:
            NRF_LOG_ERROR("Unknow pin\n");
    }
}

void timeout_handler(void *p_context)
{
    nrf_gpio_pin_write(GPIO_SERVO_ENABLE, 0);
    app_pwm_disable(&PWM0);
}

void servo_init()
{
    ret_code_t err_code;
    err_code = app_simple_timer_init();
    APP_ERROR_CHECK(err_code);
    
    app_pwm_config_t pwm0_cfg = APP_PWM_DEFAULT_CONFIG_1CH(20000L, GPIO_SERVO_2);
    pwm0_cfg.pin_polarity[0] = APP_PWM_POLARITY_ACTIVE_HIGH;
    pwm0_cfg.pin_polarity[1] = APP_PWM_POLARITY_ACTIVE_HIGH;
    err_code = app_pwm_init(&PWM0, &pwm0_cfg, NULL);
    APP_ERROR_CHECK(err_code);
    
    app_pwm_channel_duty_set(&PWM0, 0, 0);
    app_pwm_enable(&PWM0);
    
    nrf_gpio_pin_write(GPIO_SERVO_ENABLE, 0);
}

void gpio_init()
{
    ret_code_t err_code;

    err_code = nrf_drv_gpiote_init();
    APP_ERROR_CHECK(err_code);
    
    nrf_drv_gpiote_in_config_t mi_switch_config = GPIOTE_CONFIG_IN_SENSE_TOGGLE(false);
    mi_switch_config.pull = NRF_GPIO_PIN_PULLUP;

    err_code = nrf_drv_gpiote_in_init(GPIO_MI_SWITCH, &mi_switch_config, gpio_event_handler);
    APP_ERROR_CHECK(err_code);
    
    
    nrf_drv_gpiote_in_config_t bat_chrg_config = GPIOTE_CONFIG_IN_SENSE_TOGGLE(false);
    bat_chrg_config.pull = NRF_GPIO_PIN_PULLUP;

    err_code = nrf_drv_gpiote_in_init(GPIO_BAT_CHRG, &bat_chrg_config, gpio_event_handler);
    APP_ERROR_CHECK(err_code);
    
    nrf_drv_gpiote_in_event_enable(GPIO_MI_SWITCH, true);
    nrf_drv_gpiote_in_event_enable(GPIO_BAT_CHRG, true);

    nrf_gpio_cfg_output(GPIO_LED_RED);
    nrf_gpio_cfg_output(GPIO_LED_BLUE);
    nrf_gpio_cfg_output(GPIO_SERVO_ENABLE);
    nrf_gpio_cfg_output(GPIO_BAT_LED);
    
    nrf_gpio_pin_write(GPIO_LED_RED, 1);
    nrf_gpio_pin_write(GPIO_LED_BLUE, 1);
    nrf_gpio_pin_write(GPIO_BAT_LED, 0);
    nrf_gpio_pin_write(GPIO_SERVO_ENABLE, 0);
    
}

int main(void)
{
    uint32_t err_code = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(err_code);
    
    gpio_init();
    servo_init();
    
    NRF_LOG_INFO("System inited\n");
    
    update_servo();
    
    while (true) {
        __WFI();
    }
}

#include <stdbool.h>
#include <stdint.h>
#include "nrf_gpio.h"
#include "SEGGER_RTT.h"

#define log_info(format, args...) SEGGER_RTT_printf(0, "[INFO]"format"\n", ##args)
#define log_error(format, args...) SEGGER_RTT_printf(0, "[ERROR]"format"\n", ##args)


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

void gpio_init()
{
    nrf_gpio_cfg_output(GPIO_SERVO_1);
    nrf_gpio_cfg_output(GPIO_SERVO_2);
    nrf_gpio_cfg_output(GPIO_SERVO_3);

    nrf_gpio_cfg_output(GPIO_LED_RED);
    nrf_gpio_cfg_output(GPIO_LED_BLUE);
    
    nrf_gpio_pin_write(GPIO_LED_RED, 1);
    nrf_gpio_pin_write(GPIO_LED_BLUE, 1);

    nrf_gpio_cfg_input(GPIO_KEY_AN, NRF_GPIO_PIN_PULLUP);
    nrf_gpio_cfg_input(GPIO_KEY1, NRF_GPIO_PIN_PULLUP);
    nrf_gpio_cfg_input(GPIO_KEY2, NRF_GPIO_PIN_PULLUP);
    nrf_gpio_cfg_input(GPIO_KEY3, NRF_GPIO_PIN_PULLUP);
    nrf_gpio_cfg_input(GPIO_SW2, NRF_GPIO_PIN_PULLUP);
}

void set_red_led(int on) {
    nrf_gpio_pin_write(GPIO_LED_RED, on ? 0 : 1);
}

void set_blue_led(int on) {
    nrf_gpio_pin_write(GPIO_LED_BLUE, on ? 0 : 1);
}

int main(void)
{
    gpio_init();
    log_info("System inited");
    while (true) {
    }
}

#include <stdio.h>
#include "pico_led.h"
#include "pico/stdlib.h"


#define LED_STATUS_PIN PICO_DEFAULT_LED_PIN

void setup()
{
    gpio_init(LED_STATUS_PIN);
    gpio_set_dir(LED_STATUS_PIN, GPIO_OUT);
    gpio_put(LED_STATUS_PIN, 1);
}

void blink_error(const char *error_msg)
{
    gpio_put(LED_STATUS_PIN, 1);
    sleep_ms(250);
    printf(error_msg);
    gpio_put(LED_STATUS_PIN, 0);
    sleep_ms(250);
}
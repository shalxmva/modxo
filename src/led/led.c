#include "led.h"

#ifdef WS2812_LED
#include "ws2812/ws2812_led.h"
#else
#include "pico/pico_led.h"
#endif

void init_led_struct(LED *led) {
    led->setup = setup;
    led->blink_error = blink_error;
}
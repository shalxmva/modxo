#include <stdio.h>
#include "ws2812_led.h"
#include "pico/stdlib.h"
#include "ws2812.pio.h"

#define LEDPIO pio1

void put_rgb(uint8_t red, uint8_t green, uint8_t blue)
{
    uint32_t pixel_grb_mask = (green << 16) | (red << 8) | (blue << 0);
    pio_sm_put_blocking(LEDPIO, 0, pixel_grb_mask << 8u);
}

void setup()
{
    uint offset = pio_add_program(LEDPIO, &ws2812_program);
    ws2812_program_init(LEDPIO, 0, offset, 16, 800000, true);
    put_rgb(0x00, 0xff, 0x00);
    sleep_ms(250);
    put_rgb(0, 0, 0);
}

void blink_error(const char *error_msg)
{
    put_rgb(0xff, 0, 0);
    sleep_ms(250);
    printf(error_msg);
    put_rgb(0, 0, 0);
    sleep_ms(250);
}
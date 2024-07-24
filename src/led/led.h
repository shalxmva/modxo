#ifndef _LED_H_
#define _LED_H_

typedef struct {
    void (*setup)(void);
    void (*blink_error)(const char *error_msg);
} LED;

void init_led_struct(LED *led);

#endif
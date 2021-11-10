#include <sys/param.h>
#include "common.h"
#include "led_strip.h"

static led_strip_t *s_led;


void status_init(void)
{
    s_led = led_strip_init(0, LED_GPIO, 1);
    s_led->clear(s_led, 50);
}

void status_red(void)
{
    s_led->set_pixel(s_led, 0, 16, 0, 0);
    s_led->refresh(s_led, 100);
}

void status_green(void)
{
    s_led->set_pixel(s_led, 0, 0, 16, 0);
    s_led->refresh(s_led, 100);
}

void status_blue(void)
{
    s_led->set_pixel(s_led, 0, 0, 0, 16);
    s_led->refresh(s_led, 100);
}

void status_rgb(int r, int g, int b)
{
#define CLAMP(x) MIN(255, MAX(0, x))
    s_led->set_pixel(s_led, CLAMP(r), CLAMP(g), CLAMP(b), 16);
    s_led->refresh(s_led, 100);
}

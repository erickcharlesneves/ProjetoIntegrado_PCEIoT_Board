#ifndef IO_EXPANDER_H
#define IO_EXPANDER_H

#include <stdbool.h>
#include <stdint.h>

// Identificadores dos LEDs RGB
typedef enum {
    RGB_LED_1 = 5,
    RGB_LED_2 = 8,
    RGB_LED_3 = 13
} rgb_led_t;

// Protótipos das funções
void io_expander_init_buttons();
void io_expander_init_leds();
void set_rgb_led(rgb_led_t led, bool red, bool green, bool blue);

uint8_t read_button_status();

#endif
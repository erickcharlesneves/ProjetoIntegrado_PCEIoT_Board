#include "io_expander.h"
#include "hardware/i2c.h"
#include "pico/stdlib.h"

// Definições de hardware
#define EXPANDER_I2C i2c0
#define EXPANDER_ADDR 0x3E

// Registradores do expansor
typedef enum {
    REG_DIR_B = 0x0E,
    REG_DIR_A = 0x0F,
    REG_DATA_B = 0x10,
    REG_DATA_A = 0x11
} expander_reg_t;

// Comunicação I2C
static void write_expander_reg(uint8_t reg, uint8_t value) {
    uint8_t buffer[2] = {reg, value};
    i2c_write_blocking(EXPANDER_I2C, EXPANDER_ADDR, buffer, 2, false);
}

static uint8_t read_expander_reg(uint8_t reg) {
    uint8_t value;
    i2c_write_blocking(EXPANDER_I2C, EXPANDER_ADDR, &reg, 1, true);
    i2c_read_blocking(EXPANDER_I2C, EXPANDER_ADDR, &value, 1, false);
    return value;
}

// Controle de pinos
static void set_pin_state(uint8_t pin, bool active) {
    expander_reg_t reg = (pin < 8) ? REG_DATA_A : REG_DATA_B;
    uint8_t bit_mask = 1 << (pin % 8);
    uint8_t current = read_expander_reg(reg);

    if (active) {
        current &= ~bit_mask;  // Ativa com nível baixo
    } else {
        current |= bit_mask;   // Desativa com nível alto
    }
    write_expander_reg(reg, current);
}

// Implementação das funções públicas
void io_expander_init_buttons() {
    uint8_t dir_a = read_expander_reg(REG_DIR_A);
    dir_a |= 0x07;  // Configura bits 0-2 como entradas
    write_expander_reg(REG_DIR_A, dir_a);
}

void io_expander_init_leds() {
    // LED1 (pinos 5-7)
    uint8_t dir_a = read_expander_reg(REG_DIR_A);
    dir_a &= ~0xE0;  // Configura como saída
    
    // LED2 (pinos 8-10) e LED3 (pinos 13-15)
    uint8_t dir_b = read_expander_reg(REG_DIR_B);
    dir_b &= ~0x07;  // LED2
    dir_b &= ~0xE0;  // LED3
    
    write_expander_reg(REG_DIR_A, dir_a);
    write_expander_reg(REG_DIR_B, dir_b);
}

void set_rgb_led(rgb_led_t led, bool r, bool g, bool b) {
    switch(led) {
        case RGB_LED_1:
            set_pin_state(led, r);
            set_pin_state(led + 1, g);
            set_pin_state(led + 2, b);
            break;
        case RGB_LED_2:
            set_pin_state(led, r);
            set_pin_state(led + 1, g);
            set_pin_state(led + 2, b);
            break;
        case RGB_LED_3:
            set_pin_state(led, r);
            set_pin_state(led + 1, g);
            set_pin_state(led + 2, b);
            break;
    }
}

uint8_t read_button_status() {
    uint8_t data = read_expander_reg(REG_DATA_A);
    return data & 0x07;  // Retorna apenas os bits dos botões
}
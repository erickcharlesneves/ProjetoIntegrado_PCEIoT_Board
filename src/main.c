#include <stdio.h>
#include <math.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "ms5637.h"
#include "SHT4xl-PCEIoT-Board.h"
#include "ssd1306.h"
#include "io_expander.h"

// Configurações I2C
#define I2C_PORT i2c0
#define I2C_SDA 4
#define I2C_SCL 5
#define SSD1306_ADDR 0x3C

ssd1306_t display;

// painel atual 
static uint8_t current_panel = 1;

/**
 * @brief Calcula a altitude relativa, usando a fórmula barométrica 
 * @param pressure_hpa Pressão atual
 * @param baseline_hpa Pressão de referência
 * @return Altitude com base na posição inicial do sensor em metros
 */
static float calculate_altitude(float pressure_hpa, float baseline_hpa) {
    return 44330.0f * (1.0f - powf(pressure_hpa / baseline_hpa, 1.0f / 5.255f));  
}

/****************************************************************************
 * @brief Painel demo de dados no display para o sensor MS5637_02BA03
 * @param temp Temperatura em °C
 * @param press Pressão em hPA / mbar
 * @param alt Altitude em metros
 */
static void draw_ms5637_panel(ssd1306_t *disp, float temp, float press, float alt) {
    char buf[20];
    ssd1306_clear(disp);
    ssd1306_draw_string(disp, 30, 0, "MS5637 02BA03");
    ssd1306_fill_rect(disp, 0, 10, 128, 1, true);

    ssd1306_draw_string(disp, 10, 15, "Temperatura:");
    snprintf(buf, sizeof(buf), "%.2f C", temp);
    ssd1306_draw_string(disp, 80, 15, buf);

    ssd1306_draw_string(disp, 10, 27, "Pressao:");
    snprintf(buf, sizeof(buf), "%.2f hPa", press);
    ssd1306_draw_string(disp, 80, 27, buf);

    ssd1306_draw_string(disp, 10, 39, "Altitude:");
    snprintf(buf, sizeof(buf), "%.2f m", alt);
    ssd1306_draw_string(disp, 80, 39, buf);

    ssd1306_fill_rect(disp, 0, 50, 128, 1, true);
    ssd1306_draw_string(disp, 15, 55, "Monitor Climatico 1");
    ssd1306_display(disp);
}

/*********************************************************
 * @brief Painel demo de dados no display para o sensor SHT4xl
 * @param temp Temperatura em °C
 * @param hum  Umidade relativa em %
 */
static void draw_sht4x_panel(ssd1306_t *disp, float temp, float hum) {
    char buf[20];
    ssd1306_clear(disp);
    ssd1306_draw_string(disp, 30, 0, "SHT4xl SENSOR");
    ssd1306_fill_rect(disp, 0, 10, 128, 1, true);

    ssd1306_draw_string(disp, 10, 15, "Temperatura:");
    snprintf(buf, sizeof(buf), "%.2f C", temp);
    ssd1306_draw_string(disp, 80, 15, buf);

    ssd1306_draw_string(disp, 10, 27, "Umidade:");
    snprintf(buf, sizeof(buf), "%.2f %%", hum);
    ssd1306_draw_string(disp, 80, 27, buf);

    ssd1306_fill_rect(disp, 0, 50, 128, 1, true);
    ssd1306_draw_string(disp, 15, 55, "Monitor Climatico 2");
    ssd1306_display(disp);
}

int main() {
    stdio_init_all();

    // Inicializa I2C
    i2c_init(I2C_PORT, 400 * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    // Inicializa display
    ssd1306_init(&display, I2C_PORT, SSD1306_ADDR);
    ssd1306_clear(&display);
    ssd1306_draw_string(&display, 10, 20, "Inicializando...");
    ssd1306_display(&display);

    // Expansão I/O SX1509B (botões e leds) ---
    io_expander_init_buttons();
    io_expander_init_leds();
    
    // Inicia com o LED1 (RGB) indicando o painel MS5637
    set_rgb_led(RGB_LED_1, false, true, false); // r,g,b -> 0,1,0 => verde

    // --- Sensores ---

    // Inicializa sensor ms5637
    ms5637_init();        
    // Inicializa sensor SHT4x:
    // Reset para validar comunicação (I2C já inicializado acima). melhoria futura: modificar a biblioteca para que sht4x_init() já teste a comunicação e retorne true se o sensor estiver respondendo, sem precisar chamar reset() no main.c, como foi feito com o ms5637_init().
    if (!sht4x_reset()) {
        ssd1306_clear(&display);
        ssd1306_draw_string(&display, 6, 20, "Erro init SHT4x!");
        ssd1306_display(&display);
        while (1) sleep_ms(1000);
    }

    // Captura pressão de referência (baseline para altitude atual do sensor)
    float baseline = 0.0f;
    float temp_ms = 0.0f, press_ms = 0.0f;
    uint32_t attempts = 0;
    while (baseline == 0.0f && attempts < 20) { // tenta por ~10s
        if (ms5637_read_temperature_pressure(&temp_ms, &press_ms) == MS5637_STATUS_OK) {
            baseline = press_ms;
        }
        attempts++;
        sleep_ms(500);
    }
    if (baseline == 0.0f) {
        // fallback para valor padrão (opcional)
        baseline = 1013.25f; // pressão ao nível do mar em hPa universal
    }

    // Variáveis SHT4x
    float temp_sht = 0.0f, hum_sht = 0.0f;

    // Para debouncing / edge detection do botão
    uint8_t prev_buttons = 0;
    const uint32_t debounce_ms = 200;

    // Loop principal
    while (true) {
        // lê botões do expander
        uint8_t buttons = read_button_status();

        // Detecta borda de subida do botão 0 (bit0)
        bool btn0_now = buttons & 0x01;  // Bit 0
        bool btn0_prev = prev_buttons & 0x01;
        if (btn0_now && !btn0_prev) {
            // alterna painel
            current_panel = (current_panel + 1) % 2;
            // atualiza LED indicando painel atual:
            if (current_panel == 0) {
                // MS5637 -> verde
                set_rgb_led(RGB_LED_1, 0, 1, 0); 
            } else {
                // SHT4x -> azul
                set_rgb_led(RGB_LED_1, 0, 0, 1); 
            }
            // debounce simples
            sleep_ms(debounce_ms);
        }
        prev_buttons = buttons;

        // Atualiza e mostra na serial conforme current_panel
        if (current_panel == 0) {
            // MS5637
            if (ms5637_read_temperature_pressure(&temp_ms, &press_ms) == MS5637_STATUS_OK) {
                float alt = calculate_altitude(press_ms, baseline);
                draw_ms5637_panel(&display, temp_ms, press_ms, alt);
                printf("[MS5637] T: %.2f C | P: %.2f hPa | Alt: %.2f m\n",
                       temp_ms, press_ms, alt);
            } else {
                ssd1306_clear(&display);
                ssd1306_draw_string(&display, 10, 20, "Erro MS5637!");
                ssd1306_display(&display);
            }
        } else {
            // SHT4x
            if (sht4x_read_temp_hum(PRECISION_HIGH, &temp_sht, &hum_sht)) {
                draw_sht4x_panel(&display, temp_sht, hum_sht);
                printf("[SHT4x] T: %.2f C | U: %.2f %%\n", temp_sht, hum_sht);
            } else {
                ssd1306_clear(&display);
                ssd1306_draw_string(&display, 10, 20, "Erro SHT4x!");
                ssd1306_display(&display);
            }
        }

        // pequeno atraso para responsividade + economia de CPU
        sleep_ms(150);
    }

    return 0;
}
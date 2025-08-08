/**
 * @file ssd1306.h
 * @brief Driver para displays OLED com controlador SSD1306 para Rasbberry Pi PICO W (128x64 pixels)
 * @author Carlos Elias
 * @version 1.0
 * @date 2023-07-25
 * 
 * Este driver fornece uma interface simples para controlar displays OLED com o controlador
 * SSD1306 via interface I2C. Inclui funções básicas para desenho de pixels, texto e formas.
 */

#ifndef SSD1306_H
#define SSD1306_H

#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include <stdlib.h>
#include <string.h>

// Display dimensions
#define SSD1306_WIDTH  128  ///< Largura do display em pixels
#define SSD1306_HEIGHT 64   ///< Altura do display em pixels
#define SSD1306_PAGES  (SSD1306_HEIGHT / 8) ///< Número de páginas (cada página tem 8 linhas)

/// Endereço I2C padrão do display SSD1306
#define SSD1306_I2C_ADDR 0x3C

// Command definitions
#define SSD1306_SET_CONTRAST 0x81          ///< Define o contraste do display
#define SSD1306_DISPLAY_ALL_ON_RESUME 0xA4 ///< Liga todos os pixels (resume)
#define SSD1306_DISPLAY_ALL_ON 0xA5        ///< Liga todos os pixels
#define SSD1306_NORMAL_DISPLAY 0xA6        ///< Display normal (não invertido)
#define SSD1306_INVERT_DISPLAY 0xA7        ///< Inverte as cores do display
#define SSD1306_DISPLAY_OFF 0xAE           ///< Desliga o display
#define SSD1306_DISPLAY_ON 0xAF            ///< Liga o display
#define SSD1306_SET_DISPLAY_OFFSET 0xD3    ///< Define offset vertical
#define SSD1306_SET_COM_PINS 0xDA          ///< Configura pinos COM
#define SSD1306_SET_VCOM_DETECT 0xDB       ///< Configura detecção VCOM
#define SSD1306_SET_DISPLAY_CLOCK_DIV 0xD5 ///< Define divisor de clock
#define SSD1306_SET_PRECHARGE 0xD9         ///< Define tempo de pré-carga
#define SSD1306_SET_MULTIPLEX 0xA8         ///< Configura multiplexação
#define SSD1306_SET_LOW_COLUMN 0x00        ///< Define coluna baixa
#define SSD1306_SET_HIGH_COLUMN 0x10       ///< Define coluna alta
#define SSD1306_SET_START_LINE 0x40        ///< Define linha inicial
#define SSD1306_MEMORY_ADDR_MODE 0x20      ///< Define modo de endereçamento
#define SSD1306_SET_COLUMN_ADDR 0x21       ///< Define endereço de coluna
#define SSD1306_SET_PAGE_ADDR 0x22         ///< Define endereço de página
#define SSD1306_COM_SCAN_INC 0xC0          ///< Varredura COM crescente
#define SSD1306_COM_SCAN_DEC 0xC8          ///< Varredura COM decrescente
#define SSD1306_SEG_REMAP 0xA0             ///< Remapeamento de segmentos
#define SSD1306_CHARGE_PUMP 0x8D           ///< Configura charge pump

/**
 * @brief Estrutura principal do display SSD1306
 * 
 * Esta estrutura contém todo o estado necessário para controlar o display,
 * incluindo a configuração de hardware e o buffer de frame.
 */
typedef struct {
    i2c_inst_t *i2c_port;  ///< Ponteiro para a instância I2C (i2c0 ou i2c1)
    uint8_t address;       ///< Endereço I2C do dispositivo
    uint8_t width;         ///< Largura do display em pixels
    uint8_t height;        ///< Altura do display em pixels
    uint8_t buffer[SSD1306_WIDTH * SSD1306_PAGES]; ///< Buffer de frame (128x64 pixels)
} ssd1306_t;

/**
 * @brief Inicializa o display SSD1306
 * 
 * @param display Ponteiro para a estrutura do display
 * @param i2c_port Ponteiro para a instância I2C (i2c0 ou i2c1)
 * @param address Endereço I2C do display (geralmente 0x3C ou 0x3D)
 * @return true se a inicialização foi bem-sucedida, false caso contrário
 */
bool ssd1306_init(ssd1306_t *display, i2c_inst_t *i2c_port, uint8_t address);

/**
 * @brief Limpa o buffer do display (não atualiza o display físico)
 * 
 * @param display Ponteiro para a estrutura do display inicializada
 */
void ssd1306_clear(ssd1306_t *display);

/**
 * @brief Atualiza o display físico com o conteúdo do buffer
 * 
 * @param display Ponteiro para a estrutura do display inicializada
 */
void ssd1306_display(ssd1306_t *display);

/**
 * @brief Define o estado de um pixel no buffer
 * 
 * @param display Ponteiro para a estrutura do display inicializada
 * @param x Posição horizontal (0-127)
 * @param y Posição vertical (0-63)
 * @param on true para ligar o pixel, false para desligar
 */
void ssd1306_set_pixel(ssd1306_t *display, uint8_t x, uint8_t y, bool on);

/**
 * @brief Desenha um caractere no buffer
 * 
 * @param display Ponteiro para a estrutura do display inicializada
 * @param x Posição horizontal inicial (0-127)
 * @param y Posição vertical inicial (0-63)
 * @param c Caractere a ser desenhado (ASCII 32-126)
 */
void ssd1306_draw_char(ssd1306_t *display, uint8_t x, uint8_t y, char c);

/**
 * @brief Desenha uma string no buffer
 * 
 * @param display Ponteiro para a estrutura do display inicializada
 * @param x Posição horizontal inicial (0-127)
 * @param y Posição vertical inicial (0-63)
 * @param str String terminada em null a ser desenhada
 */
void ssd1306_draw_string(ssd1306_t *display, uint8_t x, uint8_t y, const char *str);

/**
 * @brief Desenha um retângulo preenchido no buffer
 * 
 * @param display Ponteiro para a estrutura do display inicializada
 * @param x Posição horizontal do canto superior esquerdo (0-127)
 * @param y Posição vertical do canto superior esquerdo (0-63)
 * @param w Largura do retângulo em pixels
 * @param h Altura do retângulo em pixels
 * @param on true para preencher com pixels ligados, false para desligados
 */
void ssd1306_fill_rect(ssd1306_t *display, uint8_t x, uint8_t y, uint8_t w, uint8_t h, bool on);

#endif // SSD1306_H
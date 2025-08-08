#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "SHT4xl-PCEIoT-Board.h"

// Definições para a comunicação I2C
#define I2C_PORT i2c0
#define I2C_SDA 4  //8
#define I2C_SCL 5  //9

// Comandos de Medição por Precisao
#define CMD_MEASURE_HIGH_PREC     0xFD
#define CMD_MEASURE_MEDIUM_PREC   0xF6
#define CMD_MEASURE_LOW_PREC      0xE0
// Comandos de Aquecimento
#define CMD_HEATER_HIGH_1S        0x39
#define CMD_HEATER_HIGH_0_1S      0x32
#define CMD_HEATER_MEDIUM_1S      0x2F
#define CMD_HEATER_MEDIUM_0_1S    0x24
#define CMD_HEATER_LOW_1S         0x1E
#define CMD_HEATER_LOW_0_1S       0x15
// Comando de Reset
#define CMD_RESET                 0x94
//Delays em Milissegundos 
#define DELAY_HIGH_PREC_MS      10
#define DELAY_MEDIUM_PREC_MS    5
#define DELAY_LOW_PREC_MS       2
#define DELAY_HEATER_1S         1100
#define DELAY_HEATER_0_1S       110


//Calcula o CRC-8 para um buffer de dados
static uint8_t sht4x_crc8(const uint8_t *data, size_t len) {
    uint8_t crc = 0xFF;
    for (size_t i = 0; i < len; i++) {
        crc ^= data[i];
        for (uint8_t j = 0; j < 8; j++) {
            crc = (crc & 0x80) ? (crc << 1) ^ 0x31 : (crc << 1);
        }
    }
    return crc;
}

//Funcao interna que executa o ciclo de medicao 

static bool sht4x_perform_measurement(uint8_t cmd, uint16_t delay_ms, float *temperature, float *humidity) {
    uint8_t rx_buffer[6];
    //Envia o comando de medição para o sensor
    if (i2c_write_blocking(I2C_PORT, SHT4X_I2C_ADDRESS, &cmd, 1, false) != 1) {
        return false;
    }

    //Aguarda o tempo necessário para a medição
    sleep_ms(delay_ms);
    //Le os 6 bytes de resposta do sensor
    if (i2c_read_blocking(I2C_PORT, SHT4X_I2C_ADDRESS, rx_buffer, 6, false) != 6) {
        return false;
    }

    // Verifica se os dados estao iguais com o CRC 
    if (sht4x_crc8(rx_buffer, 2) != rx_buffer[2] || sht4x_crc8(rx_buffer + 3, 2) != rx_buffer[5]) {
        return false;
    }

    // Converte os valores brutos para as unidades corretas
    uint16_t raw_temp = (rx_buffer[0] << 8) | rx_buffer[1];
    uint16_t raw_humi = (rx_buffer[3] << 8) | rx_buffer[4];
    //Calculo presente no datasheet do sensor :)
    *temperature = -45.0f + 175.0f * (raw_temp / 65535.0f);
    *humidity    = -6.0f + 125.0f * (raw_humi / 65535.0f);

    // Garante que o valor da umidade permaneça no intervalo de 0 a 100%
    if (*humidity > 100.0f) *humidity = 100.0f;
    if (*humidity < 0.0f) *humidity = 0.0f;

    return true;
}
//incia o sensor
bool sht4x_init(void) {
    i2c_init(I2C_PORT, 100 * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
    return sht4x_reset();
}
//Reset
bool sht4x_reset(void) {
    uint8_t cmd = CMD_RESET;
    int result = i2c_write_blocking(I2C_PORT, SHT4X_I2C_ADDRESS, &cmd, 1, false);
    sleep_ms(2);
    return result == 1;
}
//Le a temp e a umidade usando um switch case
bool sht4x_read_temp_hum(SHT4x_Precision precision, float *temperature, float *humidity) {
    uint8_t cmd;
    uint16_t delay_ms;

    switch (precision) {
        case PRECISION_HIGH:
            cmd = CMD_MEASURE_HIGH_PREC;
            delay_ms = DELAY_HIGH_PREC_MS;
            break;
        case PRECISION_MEDIUM:
            cmd = CMD_MEASURE_MEDIUM_PREC;
            delay_ms = DELAY_MEDIUM_PREC_MS;
            break;
        case PRECISION_LOW:
            cmd = CMD_MEASURE_LOW_PREC;
            delay_ms = DELAY_LOW_PREC_MS;
            break;
        default:
            return false;
    }
    // Chama a função interna para fazer o trabalho pesado
    return sht4x_perform_measurement(cmd, delay_ms, temperature, humidity);
}
    //Leitura na base do aquecedor interno
bool sht4x_read_with_heater(SHT4x_HeaterMode mode, float *temperature, float *humidity) {
    uint8_t cmd;
    uint16_t delay_ms;

    switch (mode) {
        case HEATER_HIGH_1S:
            cmd = CMD_HEATER_HIGH_1S;
            delay_ms = DELAY_HEATER_1S;
            break;
        case HEATER_HIGH_0_1S:
            cmd = CMD_HEATER_HIGH_0_1S;
            delay_ms = DELAY_HEATER_0_1S;
            break;
        case HEATER_MEDIUM_1S:
            cmd = CMD_HEATER_MEDIUM_1S;
            delay_ms = DELAY_HEATER_1S;
            break;
        case HEATER_MEDIUM_0_1S:
            cmd = CMD_HEATER_MEDIUM_0_1S;
            delay_ms = DELAY_HEATER_0_1S;
            break;
        case HEATER_LOW_1S:
            cmd = CMD_HEATER_LOW_1S;
            delay_ms = DELAY_HEATER_1S;
            break;
        case HEATER_LOW_0_1S:
            cmd = CMD_HEATER_LOW_0_1S;
            delay_ms = DELAY_HEATER_0_1S;
            break;
        default:
            return false;
    }
    // Também chama a função interna para fazer o trabalho pesado
    return sht4x_perform_measurement(cmd, delay_ms, temperature, humidity);
}

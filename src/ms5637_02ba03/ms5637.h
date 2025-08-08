#ifndef MS5637_H
#define MS5637_H

#include <stdint.h>
#include <stdbool.h>
#include "hardware/i2c.h"

// --- CONFIGURAÇÕES DE HARDWARE ---
#define MS5637_I2C        i2c0
#define MS5637_I2C_SDA    4
#define MS5637_I2C_SCL    5
#define MS5637_I2C_FREQ   400000 // 400kHz

// --- ENDEREÇO I2C DO SENSOR ---
#define MS5637_ADDR       0x76

// --- COMANDOS ---
#define MS5637_RESET_COMMAND                     0x1E
#define MS5637_CONVERT_D1_BASE                   0x40
#define MS5637_CONVERT_D2_BASE                   0x50
#define MS5637_READ_ADC_COMMAND                  0x00
#define MS5637_PROM_READ_BASE                    0xA0

// --- ÍNDICES DOS COEFICIENTES ---
#define MS5637_CRC_INDEX                         0
#define MS5637_COEFFICIENT_COUNT                 7

// --- RESOLUÇÕES SUPORTADAS ---
typedef enum {
    MS5637_OSR_256 = 0, // 0x00 resolução mais baixa
    MS5637_OSR_512,
    MS5637_OSR_1024,
    MS5637_OSR_2048,
    MS5637_OSR_4096,
    MS5637_OSR_8192 // 0x05 resolução mais alta
} ms5637_osr_t;

// --- STATUS ---
typedef enum {
    MS5637_STATUS_OK = 0,
    MS5637_STATUS_ERROR,
    MS5637_STATUS_CRC_ERROR
} ms5637_status_t;

// --- INTERFACE ---
void ms5637_init(void);
ms5637_status_t ms5637_reset(void);
ms5637_status_t ms5637_read_temperature_pressure(float *temperature, float *pressure);

#endif // MS5637_H
#ifndef SHT4X_PCEIOT_BOARD_H
#define SHT4X_PCEIOT_BOARD_H

#include <stdbool.h>

// Endereco I2C padrao do SHT4x presente no datasheet
#define SHT4X_I2C_ADDRESS 0x44

// Enumeracao para os niveis de precisao da medicao
typedef enum {
    PRECISION_HIGH,
    PRECISION_MEDIUM,
    PRECISION_LOW
} SHT4x_Precision;

// Enumeracao para os modos de aquecimento
typedef enum {
    HEATER_HIGH_1S,
    HEATER_HIGH_0_1S,
    HEATER_MEDIUM_1S,
    HEATER_MEDIUM_0_1S,
    HEATER_LOW_1S,
    HEATER_LOW_0_1S
} SHT4x_HeaterMode;


//Inicializa a comunicacao I2C e reseta o sensor
bool sht4x_init(void);
 //Envia um comando de reset para o sensor
bool sht4x_reset(void);
 //Le temperatura e umidade com um nivel de precisao 
bool sht4x_read_temp_hum(SHT4x_Precision precision, float *temperature, float *humidity);
//Le temperatura e umidade utilizando um modo de aquecedor
bool sht4x_read_with_heater(SHT4x_HeaterMode mode, float *temperature, float *humidity);

#endif

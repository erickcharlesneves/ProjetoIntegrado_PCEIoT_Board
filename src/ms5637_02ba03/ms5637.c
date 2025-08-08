/**
 * @file ms5637.c
 * @brief Implementação da biblioteca para o sensor MS5637-02BA03 utilizando Raspberry Pi Pico.
 * @author Erick Charles
 * @version 1.0
 * @date 2023-07-29
 *
 * Este arquivo contém a implementação das funções declaradas em ms5637.h
 */

#include "ms5637.h"
#include "pico/stdlib.h"
#include <math.h>
#include <stdio.h>

// Buffer para armazenar os coeficientes da PROM do sensor
static uint16_t prom[8]; // Inclui espaço para CRC, PROM de 0 a 7
// O índice 0 é o CRC, os índices 1 a 6 são os coeficientes
// O índice 7 é reservado para uso futuro ou pode ser usado para armazenar o CRC
// Resolução atual do sensor, inicializada para a resolução mais alta
// MS5637_OSR_8192 é a resolução mais alta, que oferece a melhor precisão
// A resolução afeta o tempo de conversão e a precisão dos dados lidos

static ms5637_osr_t current_osr = MS5637_OSR_8192;

// Tabela de tempos de conversão em milissegundos para cada resolução
// Esses valores são baseados na especificação do sensor MS5637
static const uint8_t conversion_time_ms[] = {1, 2, 3, 5, 9, 17};

// Função para calcular o CRC4 dos coeficientes da PROM
// O CRC é usado para verificar a integridade dos dados lidos do sensor
// A função percorre os coeficientes, aplica o algoritmo CRC4 e retorna o valor CRC
// O CRC é calculado com base nos 16 bits de cada coeficiente, exceto o CRC em si
// O CRC é armazenado no índice 0 do array prom, e os coeficientes são armazenados nos índices 1 a 6
static uint8_t crc4(uint16_t *n_prom) {
    uint16_t n_rem = 0x00;
    n_prom[0] &= 0x0FFF;
    n_prom[7] = 0;
    for (int cnt = 0; cnt < 16; cnt++) {
        if (cnt % 2 == 1)
            n_rem ^= n_prom[cnt >> 1] & 0x00FF;
        else
            n_rem ^= n_prom[cnt >> 1] >> 8;
        for (int n_bit = 8; n_bit > 0; n_bit--) {
            if (n_rem & 0x8000)
                n_rem = (n_rem << 1) ^ 0x3000;
            else
                n_rem <<= 1;
        }
    }
    return (n_rem >> 12) & 0xF;
}

// Função para ler os coeficientes da PROM do sensor
// Lê os coeficientes de 0 a 7 do sensor MS5637 via I2C
// Cada coeficiente é lido como um par de bytes, combinados em um único valor de 16 bits
// O CRC é verificado após a leitura de todos os coeficientes
// Se o CRC calculado não corresponder ao CRC armazenado no sensor, retorna um erro de CRC
// Se a leitura for bem-sucedida, armazena os coeficientes no array prom
static ms5637_status_t read_prom(void) {
    for (int i = 0; i < 8; i++) {
        uint8_t cmd = MS5637_PROM_READ_BASE + (i * 2);
        uint8_t data[2];
        if (i2c_write_blocking(MS5637_I2C, MS5637_ADDR, &cmd, 1, true) != 1)
            return MS5637_STATUS_ERROR;
        if (i2c_read_blocking(MS5637_I2C, MS5637_ADDR, data, 2, false) != 2)
            return MS5637_STATUS_ERROR;
        prom[i] = (data[0] << 8) | data[1];
    }
    if (crc4(prom) != (prom[0] >> 12))
        return MS5637_STATUS_CRC_ERROR;
    return MS5637_STATUS_OK;
}

// Função para iniciar a conversão de temperatura ou pressão
// Envia o comando de conversão apropriado para o sensor MS5637 
// O comando é baseado na resolução atual (current_osr)
static ms5637_status_t start_conversion(uint8_t cmd) {
    return i2c_write_blocking(MS5637_I2C, MS5637_ADDR, &cmd, 1, false) == 1
           ? MS5637_STATUS_OK
           : MS5637_STATUS_ERROR;
}

// Função para ler o valor ADC do sensor após a conversão
// Envia o comando de leitura do ADC e lê os 3 bytes de dados retornados
// Combina os 3 bytes em um único valor de 24 bits
// O valor lido é o resultado da conversão de temperatura ou pressão
// Retorna o valor lido via ponteiro, ou um erro se a leitura falhar
static ms5637_status_t read_adc(uint32_t *value) {
    uint8_t cmd = MS5637_READ_ADC_COMMAND;
    uint8_t data[3];
    if (i2c_write_blocking(MS5637_I2C, MS5637_ADDR, &cmd, 1, true) != 1)
        return MS5637_STATUS_ERROR;
    if (i2c_read_blocking(MS5637_I2C, MS5637_ADDR, data, 3, false) != 3)
        return MS5637_STATUS_ERROR;
    *value = (data[0] << 16) | (data[1] << 8) | data[2];
    return MS5637_STATUS_OK;
}

// Função para reiniciar o sensor MS5637
// Envia o comando de reset para o sensor, que reinicializa o estado interno 
// e limpa os dados temporários
// Após o reset, é recomendado esperar um curto período antes de realizar novas leituras
ms5637_status_t ms5637_reset(void) {
    uint8_t cmd = MS5637_RESET_COMMAND;
    return i2c_write_blocking(MS5637_I2C, MS5637_ADDR, &cmd, 1, false) == 1
           ? MS5637_STATUS_OK
           : MS5637_STATUS_ERROR;
}

// Função de inicialização do sensor MS5637
// Configura o I2C, define os pinos SDA e SCL, e inicia a comunicação com o sensor
// Lê os coeficientes da PROM e verifica o CRC 
// Se a leitura for bem-sucedida, o sensor está pronto para uso
void ms5637_init(void) {
    i2c_init(MS5637_I2C, MS5637_I2C_FREQ);
    gpio_set_function(MS5637_I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(MS5637_I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(MS5637_I2C_SDA);
    gpio_pull_up(MS5637_I2C_SCL);

    ms5637_reset();
    sleep_ms(20);
    read_prom();
}

// Função para ler a temperatura e pressão do sensor MS5637
// Realiza a conversão de temperatura e pressão, calcula os valores finais
// A função lê os valores ADC para temperatura (D2) e pressão (D1), calcula a temperatura em °C
// e a pressão em mbar usando os coeficientes lidos da PROM
// Retorna o status da operação, que pode ser OK, ERROR ou CRC_ERROR
ms5637_status_t ms5637_read_temperature_pressure(float *temperature, float *pressure) {
    uint32_t D1 = 0, D2 = 0;
    int32_t dT, TEMP;
    int64_t OFF, SENS, P;
    int64_t T2 = 0, OFF2 = 0, SENS2 = 0;

    // Conversão de temperatura
    uint8_t cmd = MS5637_CONVERT_D2_BASE + (current_osr * 2);
    if (start_conversion(cmd) != MS5637_STATUS_OK) return MS5637_STATUS_ERROR;
    sleep_ms(conversion_time_ms[current_osr]);
    if (read_adc(&D2) != MS5637_STATUS_OK) return MS5637_STATUS_ERROR;

    // Conversão de pressão
    // A pressão é lida após a temperatura para garantir que os dados estejam prontos
    // A resolução atual (current_osr) é usada para determinar o comando de conversão 
    cmd = MS5637_CONVERT_D1_BASE + (current_osr * 2);
    if (start_conversion(cmd) != MS5637_STATUS_OK) return MS5637_STATUS_ERROR;
    sleep_ms(conversion_time_ms[current_osr]);
    if (read_adc(&D1) != MS5637_STATUS_OK) return MS5637_STATUS_ERROR;

    // Cálculo da temperatura e pressão
    // A temperatura é calculada com base no valor D2 e nos coeficientes lidos da PROM
    // A pressão é calculada com base no valor D1, na temperatura e nos coeficientes
    // O cálculo leva em conta correções adicionais se a temperatura estiver abaixo de 2000         
    // A fórmula é baseada na especificação do sensor MS5637
    // A temperatura é retornada em °C e a pressão em mbar


    // A temperatura é calculada a partir do valor D2 e dos coeficientes da PROM
    // A fórmula é TEMP = 2000 + (dT * prom[6])
    // onde dT é a diferença entre D2 e o coeficiente prom[5] ajustado
    // A temperatura é retornada em centésimos de grau Celsius (2000 corresponde a 20.00 °C)
    // A pressão é calculada a partir do valor D1, da temperatura e dos coeficientes da PROM
    // A fórmula é P = ((D1 * SENS) >> 21 - OFF) >> 15
    // onde SENS e OFF são calculados a partir dos coeficientes e da temperatura
    // A pressão é retornada em mbar (hectopascals)
    // Cálculo de dT e TEMP
    // dT é a diferença entre D2 e o coeficiente prom[5] ajusted
    // TEMP é a temperatura calculada em centésimos de grau Celsius
    dT = D2 - ((int32_t)prom[5] << 8);
    TEMP = 2000 + ((int64_t)dT * prom[6]) / 8388608;

    // OFF é o offset da pressão, SENS é a sensibilidade
    OFF = ((int64_t)prom[2] << 17) + ((int64_t)prom[4] * dT) / 64;
    SENS = ((int64_t)prom[1] << 16) + ((int64_t)prom[3] * dT) / 128;


    // Se a temperatura for inferior a 2000 °C, aplica correções adicionais
    if (TEMP < 2000) {
        T2 = ((int64_t)dT * dT) >> 31;
        OFF2 = 5 * ((TEMP - 2000) * (TEMP - 2000)) / 2;
        SENS2 = 5 * ((TEMP - 2000) * (TEMP - 2000)) / 4;
        if (TEMP < -1500) {
            OFF2 += 7 * ((TEMP + 1500) * (TEMP + 1500));
            SENS2 += (11 * ((TEMP + 1500) * (TEMP + 1500))) / 2;
        }
    }

    TEMP -= T2;
    OFF -= OFF2;
    SENS -= SENS2;

    // Cálculo final da pressão em centésimos de mbar
    P = (((D1 * SENS) >> 21) - OFF) >> 15;

    // A temperatura é convertida de centésimos de grau Celsius para graus Celsius
    *temperature = TEMP / 100.0f;
    *pressure = P / 100.0f;

    return MS5637_STATUS_OK;
}

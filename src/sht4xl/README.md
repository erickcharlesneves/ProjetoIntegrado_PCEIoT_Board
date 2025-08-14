# Biblioteca SHT4xl-PCEIoT-Board para Raspberry Pi Pico

Esta é uma biblioteca em C para a placa PCEIoT_Board - Raspberry pi pico W que permite a comunicação e leitura de dados do sensor de temperatura e umidade SHT4x através do protocolo I2C.

## Funcionalidades

* Inicialização e reset do sensor.
* Medição de temperatura e umidade com três níveis de precisão (alta, média e baixa).
* Medição de temperatura e umidade com o aquecedor interno ativado em diferentes modos e durações.
* Verificação de integridade dos dados utilizando CRC-8, conforme especificado no datasheet do sensor.

## Tipos de Dados

A biblioteca define enums para facilitar o uso das diferentes configurações de medição.

### `SHT4x_Precision`

Enumeração para selecionar o nível de precisão da medição. Os diferentes modos resultam em diferentes tempos de medição e consumo de energia.

| Nível de Precisão | Comando I2C | Tempo de Medição (típico) |
| :---------------- | :---------- | :------------------------------------- |
| `PRECISION_HIGH`  | `0xFD`      | 8.3 ms (máx)                           |
| `PRECISION_MEDIUM`| `0xF6`      | 4.5 ms (máx)                           |
| `PRECISION_LOW`   | `0xE0`      | 1.6 ms (máx)                           |

### `SHT4x_HeaterMode`

Enumeração para ativar o aquecedor interno do sensor para remover condensação ou para outras aplicações, usado em ambiente umidos. Cada modo ativa o aquecedor por uma duração específica e com uma potência determinada.

| Modo de Aquecedor         | Comando I2C | Potência (típica) | Duração do Aquecedor |
| :------------------------ | :---------- | :----------------------------- | :--------------------------------- |
| `HEATER_HIGH_1S`          | `0x39`      | 200 mW                         | 1 segundo                          |
| `HEATER_HIGH_0_1S`        | `0x32`      | 200 mW                         | 0.1 segundo                        |
| `HEATER_MEDIUM_1S`        | `0x2F`      | 110 mW                         | 1 segundo                          |
| `HEATER_MEDIUM_0_1S`      | `0x24`      | 110 mW                         | 0.1 segundo                        |
| `HEATER_LOW_1S`           | `0x1E`      | 20 mW                          | 1 segundo                          |
| `HEATER_LOW_0_1S`         | `0x15`      | 20 mW                          | 0.1 segundo                        |

## Funções Públicas

### `bool sht4x_init(void)`

Inicializa a interface I2C na Raspberry Pi Pico, configurando os pinos SDA (GPIO 8) e SCL (GPIO 9) e a velocidade de 100 kHz. Em seguida, chama `sht4x_reset` para garantir que o sensor esteja em um estado conhecido antes da primeira medição.

* **Retorno:** `true` se a inicialização e o reset forem bem-sucedidos; `false` caso contrário.

### `bool sht4x_reset(void)`

Envia um comando de software reset (`0x94`) para o sensor SHT4x através da interface I2C. Após o comando, a função aguarda 2 ms para que o sensor retorne ao estado de repouso.

* **Retorno:** `true` se o comando for enviado com sucesso; `false` caso contrário.

### `bool sht4x_read_temp_hum(SHT4x_Precision precision, float *temperature, float *humidity)`

Inicia uma medição de temperatura e umidade com base no nível de precisão fornecido. A função envia o comando de medição correspondente, aguarda o tempo necessário para a medição, lê os 6 bytes de dados e verifica a integridade com o checksum CRC-8. Por fim, converte os valores brutos para as unidades corretas.

* **`precision`:** O nível de precisão desejado (`PRECISION_HIGH`, `PRECISION_MEDIUM` ou `PRECISION_LOW`).
* **`*temperature`:** Ponteiro para a variável onde a temperatura em °C será armazenada.
* **`*humidity`:** Ponteiro para a variável onde a umidade relativa em %RH será armazenada.
* **Retorno:** `true` se a medição for bem-sucedida, os dados forem validados e as conversões realizadas; `false` caso contrário.

### `bool sht4x_read_with_heater(SHT4x_HeaterMode mode, float *temperature, float *humidity)`

Ativa o aquecedor interno do sensor no modo especificado. O sensor executa o aquecimento e, ao término, realiza uma medição de alta precisão. Os dados são lidos, validados com CRC-8 e convertidos. O aquecedor é desativado automaticamente pelo sensor após a medição.

* **`mode`:** O modo de aquecimento desejado (ex: `HEATER_HIGH_1S`).
* **`*temperature`:** Ponteiro para a variável onde a temperatura em °C será armazenada.
* **`*humidity`:** Ponteiro para a variável onde a umidade relativa em %RH será armazenada.
* **Retorno:** `true` se a medição e a conversão forem bem-sucedidas; `false` caso contrário.

## Conversão de Dados e CRC

A biblioteca implementa o algoritmo CRC-8 e as fórmulas de conversão de dados do datasheet do SHT4x.

### Conversão

Os valores brutos (`raw_temp` e `raw_humi`) são convertidos para temperatura e umidade usando as seguintes fórmulas, conforme o datasheet:

* **Temperatura (°C):** `T = -45 + 175 * (raw_temp / 65535)`
* **Umidade Relativa (%RH):** `RH = -6 + 125 * (raw_humi / 65535)`

A umidade relativa é ajustada para permanecer no intervalo de 0% a 100%.

### Checksum CRC-8

A comunicação I2C do SHT4x inclui um checksum de 8 bits para cada palavra de 16 bits de dados. A biblioteca usa o mesmo polinômio (`0x31`) e parâmetros (`Initialization = 0xFF`). A função `sht4x_crc8` calcula e verifica o CRC para garantir a confiabilidade dos dados lidos.

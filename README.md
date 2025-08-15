# LabTempo

Sistema de monitoramento ambiental para controle de condições climatológicas em ambientes de trabalho sensíveis.

## Descrição

O **LabTempo** é um firmware desenvolvido para o Raspberry Pi Pico W que monitora condições ambientais essenciais para pequenas oficinas e ambientes fechados onde são manipulados materiais sensíveis a variações de temperatura, umidade e pressão atmosférica.

A ideia surgiu da necessidade de acompanhar condições de materiais em pequenas oficinas ou ambientes fechados onde é feita a manipulação de materiais sensíveis a variações de calor, umidade e pressão. O projeto busca oferecer uma solução acessível e simples para um problema comum e pouco explorado.

## Características Principais

- **Monitoramento Duplo**: Utiliza dois sensores de alta precisão (MS5637 e SHT4x) para medições redundantes
- **Interface Visual**: Display OLED SSD1306 128x64 com navegação entre painéis
- **Controle Interativo**: Botões e LEDs RGB para navegação e status visual
- **Cálculo de Altitude**: Implementa fórmula barométrica para medição de altitude relativa
- **Comunicação I2C**: Todos os componentes conectados via protocolo I2C para simplicidade de conexão

## Hardware Compatível

### Microcontrolador
- **Raspberry Pi Pico W** (ARM Cortex-M0+)

### Sensores
- **MS5637-02BA03**: Sensor de pressão e temperatura de alta precisão
  - Faixa de pressão: 10 a 2000 mbar
  - Precisão: ±1.5 mbar, ±0.8°C
  - Interface: I2C (0x76)

- **SHT4x**: Sensor de temperatura e umidade
  - Faixa de temperatura: -40 a +125°C
  - Faixa de umidade: 0 a 100% RH
  - Interface: I2C (0x44)

### Periféricos
- **SSD1306**: Display OLED 128x64 pixels (I2C: 0x3C)
- **SX1509B**: Expansor de I/O para controle de botões e LEDs RGB (I2C: 0x3E)

### Pinagem I2C
```
SDA: GPIO 4
SCL: GPIO 5
Frequência: 400kHz
```

## Funcionalidades

### Painel MS5637 (Monitor Climatológico 1)
- Temperatura em °C
- Pressão atmosférica em hPa/mbar
- Altitude relativa em metros (baseada em pressão de referência)

### Painel SHT4x (Monitor Climatológico 2)
- Temperatura em °C
- Umidade relativa em %

### Controles
- **Botão 0**: Alterna entre painéis de sensores
- **LED RGB 1**: 
  - Verde: Painel MS5637 ativo
  - Azul: Painel SHT4x ativo

## Instalação e Compilação

### Pré-requisitos
- **Pico SDK 2.2.0** ou superior
- **CMake 3.13** ou superior
- **GCC ARM** toolchain (versão 14_2_Rel1 recomendada)
- **Git** para clonagem do repositório

### Passos de Instalação

1. **Clone o repositório**:
```bash
git clone https://github.com/erickcharlesneves/ProjetoIntegrado_PCEIoT_Board.git
cd ProjetoIntegrado_PCEIOT_Board
```

2. **Configure o ambiente Pico SDK**:
```bash
export PICO_SDK_PATH=/path/to/pico-sdk
```

3. **Crie o diretório de build**:
```bash
mkdir build
```

4. **Configure e compile**:
```bash
cmake -B build -G Ninja
ninja -C build
```

5. **Gravação no Pico W**:
   - Mantenha o botão BOOTSEL pressionado e conecte a Pico W ao seu PC
   - Copie o arquivo `ProjetoIntegrado_PCEIoT_Board.uf2` para o dispositivo

## Estrutura do Projeto

```
LabTempo/
├── src/
│   ├── main.c                          # Aplicação principal
│   ├── ms5637_02ba03/
│   │   ├── ms5637.c                    # Driver sensor MS5637
│   │   └── ms5637.h
│   ├── sht4xl/
│   │   ├── SHT4xl-PCEIoT-Board.c      # Driver sensor SHT4x
│   │   └── SHT4xl-PCEIoT-Board.h
│   ├── ssd1306/
│   │   ├── ssd1306.c                   # Driver display OLED
│   │   └── ssd1306.h
│   └── io_sx1509b/
│       ├── io_expander.c               # Driver expansor I/O
│       └── io_expander.h
├── CMakeLists.txt                      # Configuração de build
├── pico_sdk_import.cmake               # Import do Pico SDK
└── README.md                           # Este arquivo
```

## Uso

### Inicialização
1. Conecte o hardware conforme a pinagem especificada
2. Ligue o sistema - o display mostrará "Inicializando..."
3. O sistema captura automaticamente a pressão de referência para cálculo de altitude
4. O LED verde indica que o painel MS5637 está ativo

### Navegação
- **Pressione o Botão 0** para alternar entre os painéis de sensores
- **LED Verde**: Painel MS5637 (pressão, temperatura, altitude)
- **LED Azul**: Painel SHT4x (temperatura, umidade)

### Saída Serial
O sistema também envia dados via USB Serial (115200 baud) para monitoramento:
```
[MS5637] T: 23.45 C | P: 1013.25 hPa | Alt: 0.12 m
[SHT4x] T: 23.67 C | U: 45.32 %
```

## Algoritmos Implementados

### Cálculo de Altitude
Utiliza a fórmula barométrica internacional:
```c
altitude = 44330.0 * (1.0 - pow(pressao_atual / pressao_referencia, 1.0/5.255))
```

### Verificação CRC
- **MS5637**: Implementa CRC-4 para validação dos coeficientes PROM
- **SHT4x**: Implementa CRC-8 para validação dos dados de medição

## Configurações Avançadas

### Precisão do SHT4x
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


### Resolução do MS5637
O sensor MS5637 suporta diferentes resoluções OSR:
- `MS5637_OSR_8192`: Máxima resolução (17ms)
- `MS5637_OSR_4096`: Alta resolução (9ms)
- `MS5637_OSR_2048`: Resolução média (5ms)
- `MS5637_OSR_1024`: Resolução básica (3ms)

## Troubleshooting

### Problemas Comuns

**Display não inicializa**:
- Verifique as conexões I2C (SDA/SCL)
- Confirme o endereço I2C do display (0x3C)
- Verifique a alimentação (3.3V)

**Sensor não responde**:
- Confirme os endereços I2C dos sensores
- Verifique as conexões de alimentação
- Execute um scan I2C para detectar dispositivos

**Erro de compilação**:
- Verifique se o PICO_SDK_PATH está configurado
- Confirme a versão do CMake (≥ 3.13)
- Verifique se todos os arquivos estão presentes

### Códigos de Erro

- `MS5637_STATUS_ERROR`: Erro de comunicação I2C
- `MS5637_STATUS_CRC_ERROR`: Falha na verificação CRC dos coeficientes
- `false` em funções SHT4x: Erro de comunicação ou CRC inválido

## Contribuições

Contribuições são bem-vindas! Para contribuir:

1. Fork o projeto
2. Crie uma branch para sua feature (`git checkout -b feature/AmazingFeature`)
3. Commit suas mudanças (`git commit -m 'Add some AmazingFeature'`)
4. Push para a branch (`git push origin feature/AmazingFeature`)
5. Abra um Pull Request

## Licença

Este projeto está licenciado sob a licença MIT - veja o arquivo `LICENSE` para detalhes.

## Autores

- **Carlos Elias** - Desenvolvimento do driver SSD1306, DEMO inicial e documentação do projeto
- **Erick Charles** - Desenvolvimento do driver MS5637-02BA03, SX1509B e DEMO final.
- **Pablo Daniel** - Desenvolvimento do driver sht4xl, documentação...

## Referências

- [Raspberry Pi Pico Datasheet](https://datasheets.raspberrypi.org/pico/pico-datasheet.pdf)
- [MS5637 Datasheet](https://www.te.com/usa-en/product-CAT-BLPS0036.html)
- [SHT4x Datasheet](https://sensirion.com/products/catalog/SHT40/)
- [SSD1306 Datasheet](https://cdn-shop.adafruit.com/datasheets/SSD1306.pdf)
- [SX1509 Datasheet](https://cdn.sparkfun.com/datasheets/BreakoutBoards/sx1509.pdf)

---

**LabTempo** - Monitoramento ambiental preciso e acessível para ambientes de trabalho sensíveis.

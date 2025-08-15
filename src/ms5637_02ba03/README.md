
# Driver p/ sensor de pressão MS5637 - 02BA03 | Plataforma de SE: PCEIoT-Board V1.1.0 / Raspberry Pi Pico W

## 📖 Introdução
Este é o repositório de desenvolvimento da biblioteca/driver em C para configurar e acessar de forma fácil e modularizada o sensor de pressão barométrica e temperatura [**MS5637 - 02BA03**](https://github.com/anderson-pereira/PCEIoT-Board/blob/main/Datasheets/MS563702BA03.pdf)  via comunicação **I²C** com a plataforma de desenvolvimento [**PCEIoT-Board V1.1.0**](https://github.com/anderson-pereira/PCEIoT-Board/tree/main) baseada no microcontrolador RP2040 - [**Raspberry Pi Pico W**](https://github.com/anderson-pereira/PCEIoT-Board/blob/main/Datasheets/SC0918-RPI%20pico%20W.pdf)  com SDK do Pico.

O Sensor MS5637 - 02BA03 da TE Connectivity é barômetro digital de alta resolução, com ADC ΔΣ de 24 bits, de ultra baixo consumo e coeficientes de calibração gravados de fábrica, muito sensível, capaz de detectar uma mudança de  altitude ao nível do mar de 13 cm de ar. 

OBS: Neste projeto final para a placa terá apenas o driver da biblioteca com todas as suas funcionalidades, caso queira acessar o repositório completo com exemplos de uso e rotina de testes da presente biblioteca acesse [aqui](https://github.com/erickcharlesneves/MS5637_02BA03_BIBLIO). 

---

## 🔌 Conexões com o Raspberry Pi Pico W

| Pico W Pin  | MS5637 Pin | Função   |
|-------------|-----------|----------|
| GP4         | SDA       | I²C SDA  |
| GP5         | SCL       | I²C SCL  |
| 3V3         | VDD       | Alimentação |
| GND         | GND       | Terra    |

**OBS: Outras características e padrões mais detalhados de fabrica do sensor podem ser consultados no [datasheet](https://github.com/anderson-pereira/PCEIoT-Board/blob/main/Datasheets/MS563702BA03.pdf).


**I²C** configurado para **400 kHz (Fast Mode)**:

```c
#define MS5637_I2C i2c0
#define MS5637_I2C_SDA 4
#define MS5637_I2C_SCL 5
#define MS5637_I2C_FREQ 400000
```

## 📚Recursos da biblioteca

 - Inicialização do MS5637 e leitura robusta de PROM (com verificação
   CRC-4).

 - Leitura de temperatura (°C) e pressão (mbar **(milibar)** / hPa **(hectopascal)**) 1 hPa = 1 mbar.
 
 - Suporte a várias resoluções (OSR) — padrão: OSR_8192 (maior precisão).

 - Exemplos de uso com cálculo de altitude relativa e outros conforme pasta main_exemplos.

## 🔬Recursos Avançados:
### Resolução do MS5637

O sensor MS5637 suporta diferentes resoluções OSR:

-   `MS5637_OSR_8192`: Máxima resolução (17ms)
-   `MS5637_OSR_4096`: Alta resolução (9ms)
-   `MS5637_OSR_2048`: Resolução média (5ms)
-   `MS5637_OSR_1024`: Resolução básica (3ms)

## 📦 Estrutura da biblioteca

```
  📁 src/
		📁 ms5637_02ba03/		# (obrigatório) pasta da biblioteca em si. 
		├── ms5637.c        	# implementação da biblioteca.
		└── ms5637.h			# interface pública da biblioteca.
        └── README.md           # README da biblioteca.
```

## 🧭Instalação e Compilação


### ✅Pré-requisitos

-   **Pico SDK 2.2.0**  ou superior
-   **CMake 3.13**  ou superior
-   **GCC ARM**  toolchain (versão 14_2_Rel1 recomendada)
-   **Git**  para clonagem do repositório (opcionalmente recomendado) 

### Passos de Instalação (Caso queira reaproveitar esta biblioteca)

(Alternativamente - baixe a pasta que contém apenas o sensor e adicione a pasta do mesmo, ao seu projeto caso 

OBS: Inclua ao cabeçalho da sua main: #include "src/ms5637_02ba03/ms5637.h")
   
## :shipit: Requisitos
**Necessários:** 
 - [ ] Raspberry Pi Pico / Pico W (ou compatível com Pico SDK)
 - [ ] Pico SDK (`PICO_SDK_PATH` definida) configurado e funcionando (CMake)
 - [ ] Cabo USB / Fonte 3.3V
 - [ ] MS5637  (definido na lib com SDA	no GPIO4 (MS5637_I2C_SDA) SCL	no GPIO5 (MS5637_I2C_SCL) além da alimentação VCC	3.3V, GND.
 
**Outros utilizados:**
- [ ]   Placa:  **PCEIoT-Board-V1.1.0**
- [ ]   VSCode com extensão Raspberry Pi Pico

## 🔍 **Funções Públicas**

```c
void ms5637_init(void)
```

Inicializa a interface I2C na Raspberry Pi Pico, configurando os pinos SDA (GPIO 4) e SCL (GPIO 5) com frequência de 400 kHz. Envia um comando de reset ao sensor para garantir um estado conhecido e carrega os coeficientes de calibração da PROM. E realiza automaticamente a verificação de CRC dos dados lidos.

----------

```c
ms5637_status_t ms5637_reset(void)
```

Envia um comando de reset (0x1E) ao sensor via I2C, reinicializando seu estado interno e preparando-o para novas operações.  

_Retorno:_

-   `MS5637_STATUS_OK`  se o reset for bem-sucedido
    
-   `MS5637_STATUS_ERROR`  em falha de comunicação I2C
    

----------

```c
ms5637_status_t ms5637_read_temperature_pressure(float  _temperature, float  _pressure)
```
Realiza medições simultâneas de temperatura e pressão com a resolução configurada (padrão: OSR 8192).  

_Parâmetros:_

-   `temperature`: Ponteiro para armazenar temperatura em °C
    
-   `pressure`: Ponteiro para armazenar pressão em mbar
    

_Processo:_

1.  Inicia conversão de temperatura (D2)
    
2.  Aguarda tempo de conversão conforme OSR
    
3.  Lê valor ADC de temperatura
    
4.  Repete processo para pressão (D1)
    
5.  Calcula valores usando coeficientes da PROM
    

_Retorno:_

-   `MS5637_STATUS_OK`  em sucesso
    
-   `MS5637_STATUS_ERROR`  em falha de comunicação
    
-   `MS5637_STATUS_CRC_ERROR`  se coeficientes inválidos
    

----------

## 🔩Características Principais

-   Suporte a múltiplas resoluções (OSR 256 a 8192)
    
-   Cálculo automático de compensação térmica
    
-   Verificação de integridade dos dados (CRC4)
    
-   Interface I2C otimizada para Raspberry Pi Pico


## 🧮 Cálculo de altitude relativa
A biblioteca fornece pressão em mbar (hPa). Para calcular a variação de altitude relativa a partir de uma pressão de referência (baseline) ao ligar a placa, use a fórmula padrão do barômetro:

```c
float calc_altitude(float pressure_hpa, float baseline_hpa) {
    return 44330.0f * (1.0f - powf(pressure_hpa / baseline_hpa, 1.0f / 5.255f));
}
```

____________
## 🔧 Principais comandos de registrador utilizados na biblioteca 

| Função  | Comando base | Descrição   |
|-------------|-----------|----------|
| Reset         | 0x1E      | Reinicia o sensor  |
| Conversão pressão (D1)         | 0x40–0x4A      | Depende do OSR  |
| Conversão temperatura (D2)         | 0x50–0x5A      | Depende do OSR |
| Leitura ADC        | 0x00       | Retorna resultado da última conversão    |
| Leitura PROM        | 0xA0–0xAE      | 7 endereços para coeficientes   |

## 📊 **Tabela de características resumidas do sensor em si**


| **Parâmetro** | **Valor**                            |**Unidade** | **Observações** |
|---------------------------------------|----------------------------------------------|------------------|-----------------|
| **Faixa de pressão operacional**      | 300 a 1200                                   | mbar             | Faixa típica de operação |
| **Faixa de pressão estendida**        | 10 a 2000                                    | mbar             | Sem garantia total de especificação |
| **Faixa de temperatura**              | -40 a +85                                    | °C               | Totalmente funcional |
| **Resolução máxima (pressão)**        | 0,016                                        | mbar             | OSR = 8192 |
| **Resolução máxima (temperatura)**    | 0,002                                        | °C               | OSR = 8192 |
| **Consumo em standby**                | ≤ 0,1                                        | µA               | Modo idle |
| **Consumo em medição**                 | 0,63 a 20                                    | µA               | Dependente do OSR |
| **Tensão de alimentação (VDD)**       | 1,5 a 3,6                                    | V                | Recom. 3,3 V |
| **Tempo conversão mínima**            | 0,54                                         | ms               | OSR = 256 |
| **Tempo conversão máxima**            | 16,44                                        | ms               | OSR = 8192 |
| **Interface de comunicação**          | I²C                                          | —                | Endereço 0x76 |
| **Coeficientes de calibração**        | 6                                            | —                | Armazenados na PROM interna |
| **Precisão absoluta**                  | ±2                                           | mbar             | De 700 a 1100 mbar, 0–+60 °C |
| **Dimensões do encapsulamento**       | 3 × 3 × 0,9                                  | mm³              | QFN |
| **Resistência ESD**                   | 2                                            | kV               | HBM |

---

## 🎯 **Conclusão**  


Esta biblioteca representa uma implementação robusta e otimizada para o sensor barométrico  **MS5637-02BA03**  na plataforma  **Raspberry Pi Pico W**, oferecendo:

1.  **Precisão e Performance:**
    
    -   Suporte completo às 6 resoluções OSR (256 a 8192)
        
    -   Implementação fiel dos algoritmos de compensação térmica do fabricante TE Connectivity.
        
    -   Leituras estáveis com resolução de até 0.016 mbar (pressão) e 0.002°C (temperatura)
        
2.  **Confiabilidade Operacional:**
    
    -   Verificação CRC-4 dos coeficientes de calibração da PROM.
        
    -   Tratamento robusto de erros de comunicação I²C.
        
    -   Tempos de conversão rigorosamente respeitados conforme datasheet.
        
3.  **Modularidade e Extensibilidade:**
    
    -   Arquitetura desacoplada permitindo fácil portabilidade.
        
    -   Interface clara com funções bem documentadas para rápida integração.
        
        
4.  **Otimizações Específicas:**
    
    -   Comunicação I²C a 400 kHz (Fast Mode).
        
    -   Cálculos inteiros de 64 bits para máxima precisão.
        
    -   Algoritmos de compensação térmica implementados conforme AN520.
        
    

## 📚 Referência

-   **Datasheet MS5637-02BA03 – TE Connectivity**  
    https://www.alldatasheet.com/datasheet-pdf/pdf/880804/TEC/MS563702BA03-50.html



# SSD1306 Library for Raspberry Pi Pico W

A lightweight and efficient library for controlling **SSD1306 OLED displays** via **I2C** using the **Raspberry Pi Pico W** and **Pico SDK**.

Developed by **Carlos Elias**.

---

## Features

- Written in **C** using the **Pico SDK**.
- Supports **128x64 SSD1306 OLED displays** over I2C.
- Simple API for easy text rendering and screen manipulation.
- Built-in **default font** embedded directly in the library.

---

## Project Structure

```
ssd1306-picow/
├── CMakeLists.txt        # Build configuration
├── README.md             # Documentation (this file...)
└── src/
    ├── main.c              # SSD1306 demo example
    └── display/
        ├── ssd1306.c              # SSD1306 driver implementation
        ├── ssd1306.h              # SSD1306 driver header
```

---

## Getting Started

### Requirements

- **Raspberry Pi Pico W**
- **Pico SDK** installed
- **CMake** and **Ninja** build tools
- **SSD1306 OLED Display** with I2C interface

### Clone the Repository

```bash
git clone https://github.com/PlayzyJar/ssd1306-picow.git
```

### Build the Project

```bash
mkdir build
cmake -B build -G Ninja
ninja -C build
```

### Flash to Pico W

```bash
cp ssd1306-picow.uf2 /media/<YOUR_PICO>/
```

---

## Usage Example

Below is a minimal example from the **demo** folder:

```c
#include "pico/stdlib.h"
#include "ssd1306.h"

int main() {
    stdio_init_all();
    ssd1306_t disp;
    ssd1306_init(&disp, i2c0, 0x3C, 128, 64);
    ssd1306_clear(&disp);
    ssd1306_draw_string(&disp, 0, 0, "Hello, SSD1306!");
    ssd1306_show(&disp);
    while (true) tight_loop_contents();
}
```

---

## API Overview

### Initialization

```c
void ssd1306_init(ssd1306_t *dev, i2c_inst_t *i2c, uint8_t address, uint8_t width, uint8_t height);
```

### Drawing Functions

```c
void ssd1306_clear(ssd1306_t *display);
void ssd1306_display(ssd1306_t *display);
void ssd1306_set_pixel(ssd1306_t *display, uint8_t x, uint8_t y, bool on);
void ssd1306_draw_char(ssd1306_t *display, uint8_t x, uint8_t y, char c);
void ssd1306_draw_string(ssd1306_t *display, uint8_t x, uint8_t y, const char *str);
void ssd1306_fill_rect(ssd1306_t *display, uint8_t x, uint8_t y, uint8_t w, uint8_t h, bool on);
```

---

## License

This project is licensed under the **MIT License** - see the [LICENSE](LICENSE) file for details.

---

## Author

**Carlos Elias**  
GitHub: [PlayzyJar](https://github.com/PlayzyJar)

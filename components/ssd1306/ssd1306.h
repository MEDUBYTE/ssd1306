#ifndef SSD1306_H
#define SSD1306_H

#include <stdint.h>
#include "driver/i2c.h"  // für i2c_port_t

// Konfigurierbare Initialisierungsstruktur
typedef struct {
    i2c_port_t i2c_port;   // z. B. I2C_NUM_0
    int sda_io;            // SDA-GPIO
    int scl_io;            // SCL-GPIO
    uint32_t freq_hz;      // Taktfrequenz I²C
    uint8_t i2c_addr;      // Display-Adresse (meist 0x3C)
    uint8_t width;         // Breite in Pixeln (z. B. 128)
    uint8_t height;        // Höhe in Pixeln (z. B. 32, 64)
} ssd1306_config_t;

// Gibt eine Standardkonfiguration zurück
ssd1306_config_t ssd1306_get_default_config(void);

// Initialisiert das Display mit eigener Konfiguration
void ssd1306_init(const ssd1306_config_t *config);

// Bildschirmfunktionen
void ssd1306_clear(void);
void ssd1306_update(void);
void ssd1306_draw_pixel(uint8_t x, uint8_t y, uint8_t color);

// Zeichenfunktionen
void ssd1306_draw_char(uint8_t x, uint8_t y, char c);
void ssd1306_draw_string(uint8_t x, uint8_t y, const char *str);

#endif // SSD1306_H

#include "ssd1306.h"
#include "driver/i2c.h"
#include <string.h>
#include <stdlib.h>
#include "font8x8_basic.h"

static uint8_t *buffer = NULL;
static uint8_t display_pages = 4;
static uint8_t ssd1306_height = 32;
static uint8_t ssd1306_width = 128;
static i2c_port_t i2c_port = I2C_NUM_0;
static uint8_t i2c_addr = 0x3C;

ssd1306_config_t ssd1306_get_default_config(void) {
    ssd1306_config_t cfg = {
        .i2c_port = I2C_NUM_0,
        .sda_io = 8,
        .scl_io = 9,
        .freq_hz = 400000,
        .i2c_addr = 0x3C,
        .width = 128,
        .height = 32
    };
    return cfg;
}

static void ssd1306_cmd(uint8_t cmd) {
    i2c_cmd_handle_t handle = i2c_cmd_link_create();
    i2c_master_start(handle);
    i2c_master_write_byte(handle, (i2c_addr << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(handle, 0x00, true);  // Command mode
    i2c_master_write_byte(handle, cmd, true);
    i2c_master_stop(handle);
    i2c_master_cmd_begin(i2c_port, handle, 1000 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(handle);
}

void ssd1306_init(const ssd1306_config_t *cfg) {
    ssd1306_height = cfg->height;
    ssd1306_width = cfg->width;
    i2c_port = cfg->i2c_port;
    i2c_addr = cfg->i2c_addr;
    display_pages = ssd1306_height / 8;

    if (buffer) free(buffer);
    buffer = calloc(ssd1306_width * display_pages, sizeof(uint8_t));

    i2c_config_t i2c_cfg = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = cfg->sda_io,
        .scl_io_num = cfg->scl_io,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = cfg->freq_hz
    };
    i2c_param_config(i2c_port, &i2c_cfg);
    i2c_driver_install(i2c_port, i2c_cfg.mode, 0, 0, 0);

    // SSD1306 Init-Sequenz
    ssd1306_cmd(0xAE);
    ssd1306_cmd(0x20); ssd1306_cmd(0x00);
    ssd1306_cmd(0xB0);
    ssd1306_cmd(0xC8);
    ssd1306_cmd(0x00);
    ssd1306_cmd(0x10);
    ssd1306_cmd(0x40);
    ssd1306_cmd(0x81); ssd1306_cmd(0x7F);
    ssd1306_cmd(0xA1);
    ssd1306_cmd(0xA6);
    ssd1306_cmd(0xA8); ssd1306_cmd(ssd1306_height - 1);
    ssd1306_cmd(0xA4);
    ssd1306_cmd(0xD3); ssd1306_cmd(0x00);
    ssd1306_cmd(0xD5); ssd1306_cmd(0x80);
    ssd1306_cmd(0xD9); ssd1306_cmd(0xF1);
    ssd1306_cmd(0xDA); ssd1306_cmd(ssd1306_height == 64 ? 0x12 : 0x02);
    ssd1306_cmd(0xDB); ssd1306_cmd(0x20);
    ssd1306_cmd(0x8D); ssd1306_cmd(0x14);
    ssd1306_cmd(0xAF);
}

void ssd1306_clear(void) {
    if (buffer) memset(buffer, 0, ssd1306_width * display_pages);
}

void ssd1306_update(void) {
    for (uint8_t page = 0; page < display_pages; page++) {
        ssd1306_cmd(0xB0 + page);
        ssd1306_cmd(0x00);
        ssd1306_cmd(0x10);

        i2c_cmd_handle_t handle = i2c_cmd_link_create();
        i2c_master_start(handle);
        i2c_master_write_byte(handle, (i2c_addr << 1) | I2C_MASTER_WRITE, true);
        i2c_master_write_byte(handle, 0x40, true);  // Data mode
        i2c_master_write(handle, &buffer[page * ssd1306_width], ssd1306_width, true);
        i2c_master_stop(handle);
        i2c_master_cmd_begin(i2c_port, handle, 1000 / portTICK_PERIOD_MS);
        i2c_cmd_link_delete(handle);
    }
}

void ssd1306_draw_pixel(uint8_t x, uint8_t y, uint8_t color) {
    if (!buffer || x >= ssd1306_width || y >= ssd1306_height) return;

    uint16_t index = x + (y / 8) * ssd1306_width;
    if (color)
        buffer[index] |= (1 << (y % 8));
    else
        buffer[index] &= ~(1 << (y % 8));
}

void ssd1306_draw_char(uint8_t x, uint8_t y, char c) {
    if ((uint8_t)c > 127) return;

    for (uint8_t row = 0; row < 8; row++) {
        uint8_t bits = font8x8_basic[(uint8_t)c][row];
        for (uint8_t col = 0; col < 8; col++) {
            uint8_t pixel = (bits >> col) & 0x01;
            ssd1306_draw_pixel(x + col, y + row, pixel);
        }
    }
}

void ssd1306_draw_string(uint8_t x, uint8_t y, const char *str) {
    while (*str) {
        ssd1306_draw_char(x, y, *str);
        x += 8;
        str++;
    }
}

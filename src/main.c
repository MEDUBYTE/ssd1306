#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "ssd1306.h"  // ← Wichtig!

void app_main(void) {
    ssd1306_config_t cfg = ssd1306_get_default_config();
    cfg.freq_hz = 100000;  // falls du etwas ändern willst
    ssd1306_init(&cfg);

    ssd1306_clear();
    ssd1306_draw_string(0, 0, "Hello ESP32");
    ssd1306_draw_string(0, 10, "Hello World");
    ssd1306_update();
}

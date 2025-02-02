#include "driver/i2c_types.h"
#include "esp_err.h"

esp_err_t ssd1306_setup();
esp_err_t ssd1306_write_text(char *text, bool append);
esp_err_t ssd1306_clear_screen();
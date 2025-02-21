#include "driver/i2c_types.h"
#include "esp_err.h"

typedef enum _SSD1306AdressingMode {
  SSD1306_HORIZONTAL_ADRESSING,
  SSD1306_VERTICAL_ADRESSING,
  SSD1306_PAGE_ADRESSING,
} SSD1306AdressingMode;

typedef struct _SSD1306Config {
  uint8_t i2c_address;
  SSD1306AdressingMode addressing_mode;
  bool invert_orientation;
  uint8_t sdaPin;
  uint8_t sclPin;
} SSD1306Config;

typedef struct _SSD1306Handle {
  SSD1306Config conf;
  i2c_master_bus_handle_t master_bus_handle;
  i2c_master_dev_handle_t dev_handle;
} SSD1306Handle;

SSD1306Config ssd1306_default_config();

esp_err_t ssd1306_setup(SSD1306Handle *handle, SSD1306Config *conf);
esp_err_t ssd1306_write_text(SSD1306Handle *handle, char *text, bool append);
esp_err_t ssd1306_clear_screen(SSD1306Handle *handle);

esp_err_t ssd1306_vertical_scroll_start(SSD1306Handle *handle);
#include "ssd1306_driver.h"
#include "driver/i2c_master.h"
#include "driver/i2c_types.h"
#include "esp_err.h"
#include "esp_log.h"
#include <stdio.h>
#include <string.h>

#include "font8x8.h"
#include "ssd1306.h"

static const char *TAG = "SSD1306_DRIVER";

SSD1306Config ssd1306_default_config() {
  SSD1306Config conf = {
      .i2c_address = SSD1306_I2C_ADDRESS,
      .addressing_mode = SSD1306_HORIZONTAL_ADRESSING,
      .invert_orientation = false,
      .sdaPin = CONFIG_SSD1306_SDA_PIN,
      .sclPin = CONFIG_SSD1306_SCL_PIN,
  };
  return conf;
}

esp_err_t i2c_master_init(SSD1306Config *conf,
                          i2c_master_bus_handle_t *bus_handle) {
  i2c_master_bus_config_t i2c_mst_config = {.clk_source = I2C_CLK_SRC_DEFAULT,
                                            .sda_io_num = conf->sdaPin,
                                            .scl_io_num = conf->sclPin,
                                            .flags.enable_internal_pullup =
                                                true};
  return i2c_new_master_bus(&i2c_mst_config, bus_handle);
}

esp_err_t i2c_master_create_device(SSD1306Config *conf,
                                   i2c_master_bus_handle_t *bus_handle,
                                   i2c_master_dev_handle_t *dev_handle) {
  esp_err_t res;

  i2c_device_config_t dev_cfg = {
      .dev_addr_length = I2C_ADDR_BIT_LEN_7,
      .device_address = conf->i2c_address,
      .scl_speed_hz = 100000, // TODO dynamic?
  };

  res = i2c_master_probe(*bus_handle, conf->i2c_address, -1);

  if (res != ESP_OK) {
    printf("Device at address 0x%02x not found\n", conf->i2c_address);
    return res;
  }
  return i2c_master_bus_add_device(*bus_handle, &dev_cfg, dev_handle);
}

esp_err_t ssd1306_setup(SSD1306Handle *handle, SSD1306Config *conf) {
  esp_err_t res;

  if (handle == NULL) {
    ESP_LOGE(TAG, "handle is NULL\n");
    return ESP_ERR_INVALID_ARG;
  }
  handle->conf = *conf;

  res = i2c_master_init(conf, &handle->master_bus_handle);
  if (res != ESP_OK) {
    printf("I2C master init failed\n");
    return res;
  }
  res = i2c_master_create_device(conf, &handle->master_bus_handle,
                                 &handle->dev_handle);
  if (res != ESP_OK) {
    printf("I2C device creation failed\n");
    return res;
  }

  // not invert display
  uint8_t data[] = {
      SSD1306_CB_CMD_STREAM, SSD1306_CMDSL_CHARGE_PUMP,
      SSD1306_CMDVL_CHARGE_PUMP_ON, SSD1306_CMD_SET_SEGMENT_REMAP_FALSE,
      SSD1306_CMD_SET_COM_SCAN_MODE_NORMAL,
      SSD1306_CMDSL_SET_MEMORY_ADDR_MODE, // 0x20, // set memory addressing mode
      SSD1306_CMDVL_SET_MEMORY_ADDR_MODE_HORZ, // 0xA0, // horizontal addressing
                                               // mode
      SSD1306_CMDSL_SET_PAGE_ADDR,             // 0x22, // set page address
      0xA0,                                    // start page address
      0xB7,                                    // end page address
      SSD1306_CMD_DISPLAY_ON};                 // 0xAF};

  if (conf->invert_orientation) {
    data[3] = SSD1306_CMD_SET_SEGMENT_REMAP_TRUE;
    data[4] = SSD1306_CMD_SET_COM_SCAN_MODE_REMAP;
  }
  res = i2c_master_transmit(handle->dev_handle, data, sizeof(data), -1);
  if (res != ESP_OK) {
    printf("I2C setup ssd1306 failed\n");
    return res;
  }

  res = ssd1306_clear_screen(handle);
  if (res != ESP_OK) {
    printf("I2C clear screen failed\n");
    return res;
  }
  return res;
}

esp_err_t ssd1306_clear_screen(SSD1306Handle *handle) {
  uint8_t clear_data[1025] = {SSD1306_CB_DATA_STREAM};
  memset(clear_data + 1, 0b11111111, 1024);
  return i2c_master_transmit(handle->dev_handle, clear_data, sizeof(clear_data),
                             -1);
  return ESP_OK;
}

esp_err_t ssd1306_reset_row_col_ptr(SSD1306Handle *handle) {
  esp_err_t res;

  if (handle->conf.addressing_mode == SSD1306_PAGE_ADRESSING) {
    return ESP_ERR_NOT_SUPPORTED;
  }

  uint8_t cmds[] = {SSD1306_CB_CMD_STREAM,
                    0x21, // set column address
                    0x00, // start column
                    127,  // end column
                    0x22, // set page address
                    0x00, // start page
                    7};   // end page

  res = i2c_master_transmit(handle->dev_handle, cmds, sizeof(cmds), -1);
  if (res != ESP_OK) {
    printf("I2C transmit failed\n");
    return res;
  }
  return res;
}

esp_err_t ssd1306_write_text(SSD1306Handle *handle, char *text) {
  esp_err_t res = ESP_OK;
  uint8_t text_len = strlen(text);
  if (text_len > 128) {
    printf("WARNING: Text too long\n");
  }

  uint8_t page = handle->conf.page_cursor++;
  uint8_t cmds[] = {SSD1306_CB_CMD_STREAM,
                    0xB0 | page, // set page address
                    0x00,        // set lower column address
                    0x10};       // set higher column address

  res = i2c_master_transmit(handle->dev_handle, cmds, sizeof(cmds), -1);
  if (res != ESP_OK) {
    printf("I2C write text failed\n");
    return res;
  }

  for (uint8_t i = 0; i < text_len; i++) {

    uint8_t *glyph = font8x8_basic_tr[(uint8_t)text[i]];
    uint8_t data[9] = {SSD1306_CB_DATA_STREAM};
    memcpy(data + 1, glyph, 8);
    res = i2c_master_transmit(handle->dev_handle, data, sizeof(data), -1);
    if (res != ESP_OK) {
      printf("I2C write text failed\n");
      return res;
    }
  }

  return res;
}

esp_err_t ssd1306_vertical_scroll_start(SSD1306Handle *handle) {
  uint8_t data[] = {SSD1306_CB_CMD_STREAM,
                    SSD1306_CMDSL_CONT_HORZ_VERT_SCROLL_SETUP,
                    0x00, // dummy byte
                    0x00, // page start address
                    0x00, // frame interval
                    0x07, // page end address
                    0x3F, // TODO translate magical numbers
                    0xA3,
                    0,
                    8,
                    0x2F};
  return i2c_master_transmit(handle->dev_handle, data, sizeof(data), -1);
}

#include "ssd1306_driver.h"
#include "driver/i2c_master.h"
#include "driver/i2c_types.h"
#include "esp_err.h"
#include <stdio.h>
#include <string.h>

#include "font8x8.h"
#include "ssd1306.h"

i2c_master_bus_handle_t master_bus_handle;
i2c_master_dev_handle_t dev_handle;

esp_err_t i2c_master_init(i2c_master_bus_handle_t *bus_handle) {
  i2c_master_bus_config_t i2c_mst_config = {
      .clk_source = I2C_CLK_SRC_DEFAULT,
      .sda_io_num = CONFIG_SSD1306_SDA_PIN,
      .scl_io_num = CONFIG_SSD1306_SCL_PIN,
      .flags.enable_internal_pullup = true};

  return i2c_new_master_bus(&i2c_mst_config, bus_handle);
}

esp_err_t i2c_master_create_device(i2c_master_bus_handle_t *bus_handle,
                                   uint8_t dev_addr,
                                   i2c_master_dev_handle_t *dev_handle) {

  esp_err_t res;

  i2c_device_config_t dev_cfg = {
      .dev_addr_length = I2C_ADDR_BIT_LEN_7,
      .device_address = dev_addr,
      .scl_speed_hz = 100000, // TODO dynamic?
  };

  res = i2c_master_probe(*bus_handle, dev_addr, -1);

  if (res != ESP_OK) {
    printf("Device at address 0x%02x not found\n", dev_addr);
    return res;
  }
  return i2c_master_bus_add_device(*bus_handle, &dev_cfg, dev_handle);
}

esp_err_t ssd1306_setup() {
  esp_err_t res;

  res = i2c_master_init(&master_bus_handle);
  if (res != ESP_OK) {
    printf("I2C master init failed\n");
    return res;
  }
  res = i2c_master_create_device(&master_bus_handle, SSD1306_I2C_ADDRESS,
                                 &dev_handle);
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
  res = i2c_master_transmit(dev_handle, data, sizeof(data), -1);
  if (res != ESP_OK) {
    printf("I2C setup ssd1306 failed\n");
    return res;
  }

  res = ssd1306_clear_screen();
  if (res != ESP_OK) {
    printf("I2C clear screen failed\n");
    return res;
  }
  return res;
}

esp_err_t ssd1306_clear_screen() {
  uint8_t clear_data[1025] = {SSD1306_CB_DATA_STREAM};
  memset(clear_data + 1, 0x00, 1024);
  return i2c_master_transmit(dev_handle, clear_data, sizeof(clear_data), -1);
}

esp_err_t ssd1306_reset_row_col_ptr() {
  esp_err_t res;
  uint8_t cmds[] = {SSD1306_CB_CMD_STREAM,
                    0x21, // set column address
                    0x00, // start column
                    127,  // end column
                    0x22, // set page address
                    0x00, // start page
                    7};   // end page

  res = i2c_master_transmit(dev_handle, cmds, sizeof(cmds), -1);
  if (res != ESP_OK) {
    printf("I2C transmit failed\n");
    return res;
  }
  return res;
}

esp_err_t ssd1306_write_text(char *text, bool append) {
  esp_err_t res;
  uint8_t text_len = strlen(text);
  if (text_len > 128) {
    printf("WARNING: Text too long\n");
  }

  if (!append) {
    res = ssd1306_clear_screen();
    if (res != ESP_OK) {
      printf("I2C clear screen failed\n");
      return res;
    }
    res = ssd1306_reset_row_col_ptr();
    if (res != ESP_OK) {
      printf("I2C reset row col ptr failed\n");
      return res;
    }
  }

  for (uint8_t i = 0; i < text_len; i++) {

    uint8_t *glyph = font8x8_basic_tr[(uint8_t)text[i]];
    uint8_t data[9] = {SSD1306_CB_DATA_STREAM};
    memcpy(data + 1, glyph, 8);
    res = i2c_master_transmit(dev_handle, data, sizeof(data), -1);
    if (res != ESP_OK) {
      printf("I2C write text failed\n");
      return res;
    }
  }

  return res;
}

esp_err_t ssd1306_vertical_scroll_start() {
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
  return i2c_master_transmit(dev_handle, data, sizeof(data), -1);
}

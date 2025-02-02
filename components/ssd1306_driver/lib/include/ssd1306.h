#pragma once

#define SSD1306_I2C_ADDRESS 0x3C
#define SSD1306_WIDTH 128
#define SSD1306_HEIGHT 64

// TODO config
#define SSD1306_SDA_PIN 21
#define SSD1306_SCL_PIN 22

// Control byte
#define SSD1306_CB_CMD_SINGLE 0x80
#define SSD1306_CB_CMD_STREAM 0x00
#define SSD1306_CB_DATA_STREAM 0x40

// Fundamental commands (pg.28)
#define SSD1306_CMDSL_SET_CONTRAST                                             \
  0x81 // follow with SSD1306_CMDVL_RESET_CONSTRAST or 0-255
#define SSD1306_CMDVL_RESET_CONSTRAST 0x7F
#define SSD1306_CMD_DISPLAY_RAM 0xA4
#define SSD1306_CMD_DISPLAY_ALLON 0xA5
#define SSD1306_CMD_DISPLAY_NORMAL 0xA6
#define SSD1306_CMD_DISPLAY_INVERTED 0xA7
#define SSD1306_CMD_DISPLAY_OFF 0xAE
#define SSD1306_CMD_DISPLAY_ON 0xAF

// Hardware Config (pg.31)
#define SSD1306_CMD_SET_DISPLAY_START_LINE 0x40
#define SSD1306_CMD_SET_SEGMENT_REMAP_FALSE 0xA0
#define SSD1306_CMD_SET_SEGMENT_REMAP_TRUE 0xA1
#define SSD1306_CMD_SET_COM_SCAN_MODE_NORMAL 0xC0
#define SSD1306_CMD_SET_COM_SCAN_MODE_REMAP 0xC8

// Addressing Command Table (pg.30)
#define SSD1306_CMDSL_SET_MEMORY_ADDR_MODE 0x20 // follow with 0x00 = HORZ mode
#define SSD1306_CMDVL_SET_MEMORY_ADDR_MODE_HORZ 0xA0
#define SSD1306_CMDVL_SET_MEMORY_ADDR_MODE_VERT 0x01
#define SSD1306_CMDVL_SET_MEMORY_ADDR_MODE_PAGE 0x02
#define SSD1306_CMD_SET_LOW_COL_START_ADDR 0x00  // bitwise OR with 0-7
#define SSD1306_CMD_SET_HIGH_COL_START_ADDR 0x10 // bitwise OR with 0-15

#define SSD1306_CMDSL_SET_PAGE_ADDR                                            \
  0x22 // follows with 0-7 start addr, 0-7 end addr

// Charge Pump (pg.62)
#define SSD1306_CMDSL_CHARGE_PUMP 0x8D // follow with 0x14
#define SSD1306_CMDVL_CHARGE_PUMP_ON 0x14

// Scrolling Commands
#define SSD1306_CMDSL_CONT_HORZ_VERT_SCROLL_SETUP 0x29

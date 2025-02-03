#include "motion_sensor.h"

#include <stdio.h>

void setup_motion_sensor_led() {
  // Configure the built-in LED pin as output
  gpio_config_t led_config = {
      .pin_bit_mask = (1ULL << LED_BUILTIN), // Pin bit mask for the LED
      .mode = GPIO_MODE_OUTPUT,              // Set as output
      .pull_up_en = GPIO_PULLUP_DISABLE,     // No pull-up resistor
      .pull_down_en = GPIO_PULLDOWN_DISABLE, // No pull-down resistor
      .intr_type = GPIO_INTR_DISABLE         // No interrupt
  };
  gpio_config(&led_config);
}

void setup_pir_motion_sensor() {
  gpio_config_t io_conf = {
      .pin_bit_mask = (1ULL << CONFIG_PIR_SENSOR_PIN), // Bit mask for the pin
      .mode = GPIO_MODE_INPUT,                         // Input mode
      .pull_up_en = GPIO_PULLUP_DISABLE,               // Disable pull-up
      .pull_down_en = GPIO_PULLDOWN_DISABLE,           // Disable pull-down
      .intr_type = GPIO_INTR_DISABLE                   // No interrupts
  };
  gpio_config(&io_conf);
}

inline int read_pir_motion_sensor() {
  return gpio_get_level(CONFIG_PIR_SENSOR_PIN);
}
inline void motion_sensor_led_on() { gpio_set_level(LED_BUILTIN, 1); }
inline void motion_sensor_led_off() { gpio_set_level(LED_BUILTIN, 0); }
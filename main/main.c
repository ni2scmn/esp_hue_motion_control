/* WiFi station Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include "bootloader_random.h"
#include "driver/gpio.h"
#include "esp_http_client.h"
#include "esp_log.h"
#include "esp_random.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "nvs_flash.h"
#include <string.h>

#include "hue_api_config.h"
#include "hue_api_parser.h"
#include "hue_api_wrapper.h"
#include "motion_sensor.h"
#include "ssd1306_driver.h"
#include "wifi_connector.h"

#define PIR_SENSOR_PIN GPIO_NUM_18 // Replace with your chosen GPIO pin
#define LED_BUILTIN GPIO_NUM_2 // Built-in LED pin for ESP32 (usually GPIO 2)

void app_main(void) {
  ESP_ERROR_CHECK(ssd1306_setup());
  ESP_ERROR_CHECK(ssd1306_write_text("Hello, world!", false));
  ESP_ERROR_CHECK(ssd1306_write_text("This is a test.", true));
  ESP_ERROR_CHECK(ssd1306_write_text("Wow, it works!", true));
  printf("Test complete\n");
}
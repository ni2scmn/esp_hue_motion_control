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

#include "config_server.h"
#include "hue_api_config.h"
#include "hue_api_parser.h"
#include "hue_api_wrapper.h"
#include "motion_sensor.h"
#include "ssd1306_driver.h"
#include "wifi_connector.h"

#define DRY_RUN true

static const char *TAG = "MAIN";

const app_config_t *app_config = NULL;

TaskHandle_t activateLightTaskHandle, deactivateGroupedLightTaskHandle,
    checkMotionTaskHandle;

void setupDisplayTask(void *parameters) {
  SSD1306Config ssd_config = ssd1306_default_config();
  ssd_config.invert_orientation = true;
  ssd_config.page_cursor = 1;
  SSD1306Handle *handle = malloc(sizeof(SSD1306Handle));
  esp_err_t res = ssd1306_setup(handle, &ssd_config);
  ssd1306_write_text(handle, "Hello, world!");
  free(handle);
  vTaskDelete(NULL);
}

void setupMotionSensorTask(void *parameters) {
  setup_motion_sensor_led();
  setup_pir_motion_sensor();
  vTaskDelete(NULL);
}

void activateLightTask(void *parameters) {
  esp_http_client_config_t cfg = {.url = "https://google.com"};
  esp_http_client_handle_t client = esp_http_client_init(&cfg);
  while (1) {
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

    if (!app_config->active) {
      ESP_LOGI(TAG, "Not active, skip activating light");
      continue;
    }

    if (DRY_RUN) {
      ESP_LOGI(TAG, "Dry run: Activating light");
    } else {
      turn_on_grouped_light(&client, "1ad5cedd-1053-42fe-8319-5997facf8423");
    }
  }
}

void deactivateGroupedLightTask(void *parameters) {
  esp_http_client_config_t cfg = {.url = "https://google.com"};
  esp_http_client_handle_t client = esp_http_client_init(&cfg);
  while (1) {
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

    if (!app_config->active) {
      ESP_LOGI(TAG, "Not active, skip deactivating light");
      continue;
    }

    if (DRY_RUN) {
      ESP_LOGI(TAG, "Dry run: Deactivating light");
    } else {
      turn_off_grouped_light(&client, "1ad5cedd-1053-42fe-8319-5997facf8423");
    }
  }
}

void checkMotionTask(void *parameters) {

  bool light_activated = false;
  int last_motion_detected = 0;

  while (1) {
    int motion_detected = read_pir_motion_sensor();

    if (motion_detected) {
      last_motion_detected = esp_timer_get_time();
      ESP_LOGD(TAG, "Motion detected!");
      if (!light_activated) {
        light_activated = true;
        motion_sensor_led_on();
        xTaskNotifyGive(activateLightTaskHandle);
      }
    }

    if (light_activated &&
        esp_timer_get_time() - last_motion_detected > (10 * 1000000)) {
      light_activated = false;
      motion_sensor_led_off();
      xTaskNotifyGive(deactivateGroupedLightTaskHandle);
    }

    vTaskDelay(pdMS_TO_TICKS(100)); // Delay
  }
}

void app_main(void) {
  // Initialize NVS
  esp_err_t ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES ||
      ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    ESP_ERROR_CHECK(nvs_flash_erase());
    ret = nvs_flash_init();
  }
  ESP_ERROR_CHECK(ret);

  // Initialize WiFi
  ESP_LOGI(TAG, "ESP_WIFI_MODE_STA");
  if (!wifi_init_sta()) {
    ESP_LOGE(TAG, "Failed to initialize WiFi");
    return;
  }

  // enable internal entropy source (SAR ADC)
  bootloader_random_enable();

  httpd_handle_t config_server_handle = create_config_server_handle();
  start_config_server(&config_server_handle);

  app_config = get_config();

  BaseType_t xReturned;

  xReturned = xTaskCreate(setupMotionSensorTask, "SetupMotionSensorTask", 4096,
                          NULL, 1, NULL);

  if (xReturned != pdPASS) {
    ESP_LOGE(TAG, "Failed to create SetupMotionSensorTask");
  }

  xReturned =
      xTaskCreate(setupDisplayTask, "setupDisplayTask", 8192, NULL, 1, NULL);
  if (xReturned != pdPASS) {
    ESP_LOGE(TAG, "Failed to create setupDisplayTask");
  }

  xReturned = xTaskCreate(activateLightTask, "ActivateLightTask", 8192, NULL,
                          configMAX_PRIORITIES - 1, &activateLightTaskHandle);
  if (xReturned != pdPASS) {
    ESP_LOGE(TAG, "Failed to create ActivateLightTask");
  }

  xReturned =
      xTaskCreate(deactivateGroupedLightTask, "DeactivateGroupedLightTask",
                  8192, NULL, 1, &deactivateGroupedLightTaskHandle);
  if (xReturned != pdPASS) {
    ESP_LOGE(TAG, "Failed to create DeactivateGroupedLightTask");
  }

  xReturned = xTaskCreate(checkMotionTask, "CheckMotionTask", 4096, NULL, 1,
                          &checkMotionTaskHandle);
  if (xReturned != pdPASS) {
    ESP_LOGE(TAG, "Failed to create CheckMotionTask");
  }
}
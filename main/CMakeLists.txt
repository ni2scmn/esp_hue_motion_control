idf_component_register(SRCS "main.c"
                       INCLUDE_DIRS "."
                       REQUIRES "nvs_flash" "wifi_connector" "hue_api" "bootloader_support" "esp_timer" "motion_sensor" "ssd1306_driver" "config_server"
)
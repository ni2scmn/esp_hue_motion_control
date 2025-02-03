idf_component_register(SRCS "src/ssd1306_driver.c"
                    INCLUDE_DIRS "include"
                    PRIV_INCLUDE_DIRS "lib/include"
                    REQUIRES "esp_driver_i2c")

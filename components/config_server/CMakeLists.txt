idf_component_register(SRCS "src/config_server.c"
                    INCLUDE_DIRS "include"
                    REQUIRES "esp_http_server" "tiny_json"
)

idf_component_register(SRCS "src/hue_api_parser.c" "src/hue_api_wrapper.c"
                    INCLUDE_DIRS "include"
                    REQUIRES "esp_http_client" "tiny_json"
)

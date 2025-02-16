#pragma once

#include "esp_err.h"
#include "esp_http_server.h"

// static configuration struct
typedef struct {
  bool active;
} app_config_t;

static app_config_t config = {
    .active = true,
};

const app_config_t *get_config();

httpd_handle_t create_config_server_handle(void);
esp_err_t start_config_server(httpd_handle_t *server);
esp_err_t stop_config_server(httpd_handle_t *server);

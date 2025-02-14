#include "config_server.h"

#include "esp_log.h"

static const char *TAG = "CONFIG_SERVER";

/* Our URI handler function to be called during GET /uri request */
esp_err_t get_handler(httpd_req_t *req) {
  /* Send a simple response */
  const char resp[] = "URI GET Response";
  httpd_resp_send(req, resp, HTTPD_RESP_USE_STRLEN);
  return ESP_OK;
}

/* URI handler structure for GET /uri */
httpd_uri_t uri_get = {.uri = "/uri",
                       .method = HTTP_GET,
                       .handler = get_handler,
                       .user_ctx = NULL};

httpd_handle_t create_config_server_handle(void) {
  httpd_handle_t server = NULL;
  return server;
}

esp_err_t start_config_server(httpd_handle_t *server) {
  /* Generate default configuration */
  httpd_config_t config = HTTPD_DEFAULT_CONFIG();

  esp_err_t server_start_err = httpd_start(server, &config);

  if (server_start_err != ESP_OK) {
    ESP_LOGE(TAG, "Failed to start server : %s",
             esp_err_to_name(server_start_err));
    return server_start_err;
  }
  ESP_LOGI(TAG, "Config Server started");

  /* URI handler structure for GET /uri */
  httpd_uri_t uri_get = {.uri = "/uri",
                         .method = HTTP_GET,
                         .handler = get_handler,
                         .user_ctx = NULL};

  ESP_ERROR_CHECK(httpd_register_uri_handler(*server, &uri_get));

  return ESP_OK;
}

esp_err_t stop_config_server(httpd_handle_t *server) {
  return httpd_stop(server);
}
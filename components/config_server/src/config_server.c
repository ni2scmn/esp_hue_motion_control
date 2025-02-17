#include "config_server.h"

#include "esp_log.h"
#include "tiny_json.h"
#include <string.h>

#define MIN(x, y) (((x) < (y)) ? (x) : (y))

static const char *TAG = "CONFIG_SERVER";

// format the status struct as JSON
char *format_status_response(void) {
  char *response = malloc(100);
  snprintf(response, 100, "{\"active\": %s}", config.active ? "true" : "false");
  return response;
}

const app_config_t *get_config() { return &config; }

esp_err_t get_status_handler(httpd_req_t *req) {
  ESP_LOGI(TAG, "GET /status");
  /* Send a simple response */
  httpd_resp_set_hdr(req, "Connection", "keep-alive");
  char *resp = format_status_response();
  httpd_resp_send(req, resp, HTTPD_RESP_USE_STRLEN);
  free(resp);
  return ESP_OK;
}

esp_err_t set_status_handler(httpd_req_t *req) {
  ESP_LOGI(TAG, "POST /status");
  char content[200];

  bool found_p_in_body = false;

  /* Truncate if content length larger than the buffer */
  size_t recv_size = MIN(req->content_len, sizeof(content));
  int ret = httpd_req_recv(req, content, recv_size);
  if (ret <= 0) {
    ESP_LOGE(TAG, "set_status_handler: Failed to receive data");
    return ESP_FAIL;
  }

  // append null terminator to content buffer
  content[recv_size] = '\0';

  json_t mem[20]; // TODO increase if extending JSON object
  json_t const *json = json_create(content, mem, sizeof mem / sizeof *mem);
  if (!json) {
    ESP_LOGE(TAG, "Failed to create JSON object");
    const char resp[] = "invalid JSON";
    httpd_resp_set_status(req, HTTPD_400);
    httpd_resp_send(req, resp, HTTPD_RESP_USE_STRLEN);
    return ESP_FAIL;
  }

  json_t const *active_field = json_getProperty(json, "active");
  if (active_field && JSON_BOOLEAN == json_getType(active_field)) {
    found_p_in_body = true;
    ESP_LOGD(TAG, "Setting active to %s",
             json_getBoolean(active_field) ? "true" : "false");
    config.active = json_getBoolean(active_field);
  } else {
    ESP_LOGD(TAG, "No active field found");
  }

  if (!found_p_in_body) {
    ESP_LOGE(TAG, "No config field found in body");
    const char resp[] = "No config field found in body";
    httpd_resp_set_status(req, HTTPD_400);
    httpd_resp_send(req, resp, HTTPD_RESP_USE_STRLEN);
    return ESP_FAIL;
  }

  /* Send a simple response */
  const char *resp = format_status_response();
  httpd_resp_send(req, resp, HTTPD_RESP_USE_STRLEN);
  free(resp);
  return ESP_OK;
}

/* URI handler structure for GET /status */
httpd_uri_t status_get = {.uri = "/status",
                          .method = HTTP_GET,
                          .handler = get_status_handler,
                          .user_ctx = NULL};

/* URI handler structure for POST /status */
httpd_uri_t active_post = {.uri = "/status",
                           .method = HTTP_POST,
                           .handler = set_status_handler,
                           .user_ctx = NULL};

httpd_handle_t create_config_server_handle(void) {
  httpd_handle_t server = NULL;
  return server;
}

esp_err_t start_config_server(httpd_handle_t *server) {
  /* Generate default configuration */
  httpd_config_t config = HTTPD_DEFAULT_CONFIG();
  // depending on max open sockets in lwip configuration (menuconfig)
  config.max_open_sockets = 10;
  // purge oldest socket if max open sockets is reached
  // otherwise new connections will stall
  config.lru_purge_enable = true;

  esp_err_t server_start_err = httpd_start(server, &config);

  if (server_start_err != ESP_OK) {
    ESP_LOGE(TAG, "Failed to start server : %s",
             esp_err_to_name(server_start_err));
    return server_start_err;
  }
  ESP_LOGI(TAG, "Config Server started");

  ESP_ERROR_CHECK(httpd_register_uri_handler(*server, &status_get));
  ESP_ERROR_CHECK(httpd_register_uri_handler(*server, &active_post));

  return ESP_OK;
}

esp_err_t stop_config_server(httpd_handle_t *server) {
  return httpd_stop(server);
}
#include "hue_api_wrapper.h"

#include "esp_http_client.h"
#include "hue_api_config.h"
#include <stdio.h>

void _read_resource_with_client(esp_http_client_handle_t *client,
                                char *local_response_buffer,
                                char *resource_type, char *resource_id) {
  char url[200];
  if (resource_id == NULL) {
    sprintf(url, "https://%s/clip/v2/resource/%s", CONFIG_HUE_BRIDGE_IP,
            resource_type);
  } else {
    sprintf(url, "https://%s/clip/v2/resource/%s/%s", CONFIG_HUE_BRIDGE_IP,
            resource_type, resource_id);
  }

  esp_http_client_set_url(*client, url);
  esp_http_client_set_method(*client, HTTP_METHOD_GET);
  ESP_ERROR_CHECK(esp_http_client_set_header(*client, "hue-application-key",
                                             CONFIG_HUE_BRIDGE_USERNAME));

  ESP_ERROR_CHECK(esp_http_client_open(*client, 0));
  esp_http_client_fetch_headers(*client);
  esp_http_client_read_response(*client, local_response_buffer, 2048);

  // esp_http_client_close(*client);
}

void _read_resource(char *local_response_buffer, char *resource_type,
                    char *resource_id) {
  esp_http_client_config_t cfg = {.url = "https://google.com"};
  esp_http_client_handle_t client = esp_http_client_init(&cfg);
  _read_resource_with_client(&client, local_response_buffer, resource_type,
                             resource_id);
  esp_http_client_cleanup(client);
}

void _send_command_with_client(esp_http_client_handle_t *client,
                               char *resource_type, char *ressource_id,
                               char *command) {
  char url[200];
  sprintf(url, "https://%s/clip/v2/resource/%s/%s", CONFIG_HUE_BRIDGE_IP,
          resource_type, ressource_id);

  esp_http_client_set_url(*client, url);
  esp_http_client_set_method(*client, HTTP_METHOD_PUT);
  ESP_ERROR_CHECK(esp_http_client_set_header(*client, "hue-application-key",
                                             CONFIG_HUE_BRIDGE_USERNAME));

  ESP_ERROR_CHECK(esp_http_client_open(*client, strlen(command)));
  int wlen = esp_http_client_write(*client, command, strlen(command));
  printf("Wrote %d bytes\n", wlen);

  esp_http_client_fetch_headers(*client);

  printf("status code: %d, content length: %" PRId64 "\n",
         esp_http_client_get_status_code(*client),
         esp_http_client_get_content_length(*client));

  // esp_http_client_close(*client);
}

void ___send_command(char *resource_type, char *ressource_id, char *command) {
  esp_http_client_config_t cfg = {.url = "https://google.com"};
  esp_http_client_handle_t client = esp_http_client_init(&cfg);
  _send_command_with_client(&client, resource_type, ressource_id, command);
  esp_http_client_cleanup(client);
}

void fetch_room_list(esp_http_client_handle_t *client,
                     char *local_response_buffer) {
  _read_resource_with_client(client, local_response_buffer, "room", NULL);
}

void fetch_device_info(esp_http_client_handle_t *client,
                       char *local_response_buffer, char *device_id) {
  _read_resource_with_client(client, local_response_buffer, "device",
                             device_id);
}

void fetch_light_info(esp_http_client_handle_t *client,
                      char *local_response_buffer, char *light_id) {
  _read_resource_with_client(client, local_response_buffer, "light", light_id);
}

void turn_off_grouped_light(esp_http_client_handle_t *client,
                            char *grouped_light_id) {
  _send_command_with_client(client, "grouped_light", grouped_light_id,
                            "{\"on\": {\"on\": false}}");
}

void turn_on_grouped_light(esp_http_client_handle_t *client,
                           char *grouped_light_id) {
  _send_command_with_client(client, "grouped_light", grouped_light_id,
                            "{\"on\": {\"on\": true}}");
}

void activate_scene(esp_http_client_handle_t *client, char *scene_id) {
  _send_command_with_client(client, "scene", scene_id,
                            "{\"recall\": {\"action\": \"active\"}}");
}

void set_light_state(esp_http_client_handle_t *client, char *light_id,
                     light_state_t *light_state) {
  char command[200];
  sprintf(command, "{\"on\": {\"on\": %s}, \"dimming\": {\"brightness\": %f}}",
          light_state->on ? "true" : "false", light_state->brightness);
}

void activate_light(esp_http_client_handle_t *client, char *light_id) {
  light_state_t light_state = {.on = true, .brightness = 50};
  set_light_state(client, light_id, &light_state);
}

void deactivate_light(esp_http_client_handle_t *client, char *light_id) {
  light_state_t light_state = {.on = false, .brightness = 50};
  set_light_state(client, light_id, &light_state);
}
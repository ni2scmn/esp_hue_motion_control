#pragma once

#include "esp_http_client.h"
#include "hue_api_state.h"

void fetch_room_list(esp_http_client_handle_t *client,
                     char *local_response_buffer);
void fetch_device_info(esp_http_client_handle_t *client,
                       char *local_response_buffer, char *device_id);
void fetch_light_info(esp_http_client_handle_t *client,
                      char *local_response_buffer, char *light_id);

void turn_off_grouped_light(esp_http_client_handle_t *client,
                            char *grouped_light_id);
void turn_on_grouped_light(esp_http_client_handle_t *client,
                           char *grouped_light_id);

void activate_scene(esp_http_client_handle_t *client, char *scene_id);

void set_light_state(esp_http_client_handle_t *client, char *light_id,
                     light_state_t *light_state);

void activate_light(esp_http_client_handle_t *client, char *light_id);
void deactivate_light(esp_http_client_handle_t *client, char *light_id);
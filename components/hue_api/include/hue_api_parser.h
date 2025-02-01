#pragma once

#include "hue_api_state.h"

void find_target_room_id(char *response_buffer, char *target_room_name,
                         char *target_room_id);
void find_room_devices(char *response, char *target_room_id, char **device_ids,
                       int *device_ids_count);
void parse_light_state(char *response, light_state_t *light_state);
void parse_device_response_to_light_id(char *response, char *light_id);
#include "hue_api_parser.h"

#include "hue_api_config.h"
#include "tiny_json.h"
#include <stdio.h>
#include <string.h>

void find_target_room_id(char *response_buffer, char *target_room_name,
                         char *target_room_id) {
  json_t mem[200];
  json_t const *json =
      json_create(response_buffer, mem, sizeof mem / sizeof *mem);

  if (!json) {
    printf("find_target_room_id: Error json create.");
    return;
  }

  json_t const *data_field = json_getProperty(json, "data");
  if (!data_field || JSON_ARRAY != json_getType(data_field)) {
    printf("find_target_room_id: Error json data field.");
    return;
  }

  json_t const *data_item;

  bool target_room_found = false;
  for (data_item = json_getChild(data_field); data_item != 0;
       data_item = json_getSibling(data_item)) {

    json_t const *metadata_field = json_getProperty(data_item, "metadata");
    if (!metadata_field || JSON_OBJ != json_getType(metadata_field)) {
      printf("find_target_room_id:: Error json metadata field.");
      return;
    }
    json_t const *name_field = json_getProperty(metadata_field, "name");
    if (!name_field || JSON_TEXT != json_getType(name_field)) {
      printf("find_target_room_id:: Error json name field.");
      return;
    }
    char const *name_value = json_getValue(name_field);
    if (strcmp(name_value, HUE_BRIDGE_ROOM_NAME) == 0) {
      strcpy(target_room_id, json_getValue(json_getProperty(data_item, "id")));
      return;
    }
  }

  if (!target_room_found) {
    printf("Target room not found.\n");
  }
  return;
}

void find_room_devices(char *response, char *target_room_id, char **device_ids,
                       int *device_ids_count) {
  json_t mem[100];
  json_t const *json = json_create(response, mem, sizeof mem / sizeof *mem);

  if (!json) {
    printf("find_room_devices: Error json create.");
    return;
  }

  json_t const *data_field = json_getProperty(json, "data");
  if (!data_field || JSON_ARRAY != json_getType(data_field)) {
    printf("find_room_devices: Error json data field.");
    return;
  }

  json_t const *data_item;

  for (data_item = json_getChild(data_field); data_item != 0;
       data_item = json_getSibling(data_item)) {
    char const *id = json_getValue(json_getProperty(data_item, "id"));
    if (strcmp(id, target_room_id) != 0) {
      continue;
    }
    json_t const *children_field = json_getProperty(data_item, "children");
    if (!children_field || JSON_ARRAY != json_getType(children_field)) {
      printf("find_room_devices: Error json children field.");
      return;
    }
    json_t const *children_item;
    int i = 0;
    for (children_item = json_getChild(children_field); children_item != 0;
         children_item = json_getSibling(children_item)) {
      char const *device_id =
          json_getValue(json_getProperty(children_item, "rid"));
      if (!device_id) {
        printf("find_room_devices: Error getting device id.\n");
        return;
      }
      device_ids[i] = strdup(device_id);
      i++;
    }
    *device_ids_count = i;
    return;
  }
}

void parse_light_state(char *response, light_state_t *light_state) {
  json_t mem[200];
  json_t const *json = json_create(response, mem, sizeof mem / sizeof *mem);

  if (!json) {
    printf("parse_light_state: Error json create.");
    return;
  }

  json_t const *data_field = json_getProperty(json, "data");
  if (!data_field || JSON_ARRAY != json_getType(data_field)) {
    printf("Error json data field.");
    return;
  }

  json_t const *data_item = json_getChild(data_field);
  if (!data_item || JSON_OBJ != json_getType(data_item)) {
    printf("Error json data item.");
    return;
  }

  json_t const *on_container_field = json_getProperty(data_item, "on");
  if (!on_container_field || JSON_OBJ != json_getType(on_container_field)) {
    printf("Error json on container field.");
    return;
  }
  json_t const *on_field = json_getProperty(on_container_field, "on");
  if (!on_field || JSON_BOOLEAN != json_getType(on_field)) {
    printf("Error json on field.");
    return;
  }
  light_state->on = json_getBoolean(on_field);

  json_t const *dimming_container_field =
      json_getProperty(data_item, "dimming");
  if (!dimming_container_field ||
      JSON_OBJ != json_getType(dimming_container_field)) {
    printf("Error json dimming container field.");
    return;
  }
  json_t const *brightness_field =
      json_getProperty(dimming_container_field, "brightness");
  if (!brightness_field || JSON_REAL != json_getType(brightness_field)) {
    printf("Error json brightness field.");
    return;
  }
  light_state->brightness = json_getReal(brightness_field);

  printf("On: %d\n", light_state->on);
  printf("Brightness: %f\n", light_state->brightness);

  return;
}

void parse_device_response_to_light_id(char *response, char *light_id) {
  json_t mem[100];
  json_t const *json = json_create(response, mem, sizeof mem / sizeof *mem);

  if (!json) {
    printf("parse_device_response_to_light_id: Error json create.");
    return;
  }

  json_t const *data_field = json_getProperty(json, "data");
  if (!data_field || JSON_ARRAY != json_getType(data_field)) {
    printf("parse_device_response_to_light_id: Error json data field.");
    return;
  }

  json_t const *data_item;

  for (data_item = json_getChild(data_field); data_item != 0;
       data_item = json_getSibling(data_item)) {
    json_t const *services_field = json_getProperty(data_item, "services");
    if (!services_field || JSON_ARRAY != json_getType(services_field)) {
      printf("parse_device_response_to_light_id: Error json services field.");
      return;
    }
    json_t const *services_item;
    for (services_item = json_getChild(services_field); services_item != 0;
         services_item = json_getSibling(services_item)) {
      char const *service_type =
          json_getValue(json_getProperty(services_item, "rtype"));
      if (strcmp(service_type, "light") == 0) {
        strcpy(light_id, json_getValue(json_getProperty(services_item, "rid")));
        return;
      }
    }
  }
  return;
}

void test_parse_room() {
  char room_response[] =
      "{\"errors\":[],\"data\":[{\"id\":\"0f18be3e-db90-4d62-99f8-"
      "0b157e4bed98\",\"id_v1\":\"/groups/"
      "81\",\"children\":[{\"rid\":\"7becd0f6-4304-40af-aa84-708b4902360a\","
      "\"rtype\":\"device\"}],\"services\":[{\"rid\":\"3328a125-9709-4944-92b6-"
      "eae8e33bca07\",\"rtype\":\"grouped_light\"}],\"metadata\":{\"name\":"
      "\"Erdgeschoss\",\"archetype\":\"downstairs\"},\"type\":\"room\"},{"
      "\"id\":\"4f0f9068-b47f-429f-8e89-e692c9ef9bf1\",\"id_v1\":\"/groups/"
      "1\",\"children\":[{\"rid\":\"3dafad1a-4911-4d85-a67b-2358fa29baef\","
      "\"rtype\":\"device\"},{\"rid\":\"4136cdc7-deae-428e-9f90-405a2c58cf0c\","
      "\"rtype\":\"device\"},{\"rid\":\"44c5e4d2-838a-41c7-bfdc-29523fc46fac\","
      "\"rtype\":\"device\"},{\"rid\":\"4b9c1439-c8fd-48d5-877e-4a4997b41f31\","
      "\"rtype\":\"device\"},{\"rid\":\"5b2c7b3d-ea63-4269-b2cf-5768dde1066b\","
      "\"rtype\":\"device\"},{\"rid\":\"6a1375e9-ba2f-4d79-b21c-a4152cb8a9ba\","
      "\"rtype\":\"device\"},{\"rid\":\"9127de62-fddc-4f6b-afe7-62412b2f9779\","
      "\"rtype\":\"device\"},{\"rid\":\"ce0c95c6-5b16-45ab-9e79-25c0146fe65e\","
      "\"rtype\":\"device\"},{\"rid\":\"eaba42be-5af2-4cf2-841a-64755222392a\","
      "\"rtype\":\"device\"},{\"rid\":\"f05f4da2-5a99-4848-92e9-77df2c934357\","
      "\"rtype\":\"device\"},{\"rid\":\"f42425e7-9900-4b8f-a5b1-3a7f69534751\","
      "\"rtype\":\"device\"}],\"services\":[{\"rid\":\"1ad5cedd-1053-42fe-8319-"
      "5997facf8423\",\"rtype\":\"grouped_light\"}],\"metadata\":{\"name\":"
      "\"Wohnzimmer\",\"archetype\":\"living_room\"},\"type\":\"room\"}]}";

  // char device_response[] =
  //     "{\"errors\":[],\"data\":[{\"id\":\"7becd0f6-4304-40af-aa84-"
  //     "708b4902360a\",\"id_v1\":\"/lights/"
  //     "12\",\"product_data\":{\"model_id\":\"1743830P7\",\"manufacturer_name\":"
  //     "\"Signify Netherlands B.V.\",\"product_name\":\"Hue Econic outdoor "
  //     "wall\",\"product_archetype\":\"wall_lantern\",\"certified\":true,"
  //     "\"software_version\":\"1.122.2\",\"hardware_platform_type\":\"100b-"
  //     "11f\"},\"metadata\":{\"name\":\"Hue Econic outdoor "
  //     "Auenleuchte\",\"archetype\":\"wall_lantern\"},\"identify\":{},"
  //     "\"services\":[{\"rid\":\"baab1d43-511f-4283-996b-9ed8eaa0a578\","
  //     "\"rtype\":\"zigbee_connectivity\"},{\"rid\":\"7669bc42-8881-42ee-97a7-"
  //     "e75181198874\",\"rtype\":\"light\"},{\"rid\":\"72f88c19-78b9-4b43-aa1f-"
  //     "8cfbe7617873\",\"rtype\":\"entertainment\"},{\"rid\":\"bbb00626-c8b7-"
  //     "46fb-8182-f9f5c43f3662\",\"rtype\":\"taurus_7455\"},{\"rid\":\"a294718d-"
  //     "625f-4be5-9a87-2afe1664fa4c\",\"rtype\":\"device_software_update\"}],"
  //     "\"type\":\"device\"}]}";

  char light_response[] =
      "{\"errors\":[],\"data\":[{\"id\":\"d6838c85-38c6-449f-a804-"
      "72a66b93d665\",\"id_v1\":\"/lights/"
      "5\",\"owner\":{\"rid\":\"f05f4da2-5a99-4848-92e9-77df2c934357\","
      "\"rtype\":\"device\"},\"metadata\":{\"name\":\"Sofa Lampe "
      "\",\"archetype\":\"hue_lightstrip\",\"function\":\"decorative\"},"
      "\"product_data\":{\"function\":\"decorative\"},\"identify\":{},"
      "\"service_id\":0,\"on\":{\"on\":false},\"dimming\":{\"brightness\":50.2,"
      "\"min_dim_level\":0.04},\"dimming_delta\":{},\"color_temperature\":{"
      "\"mirek\":null,\"mirek_valid\":false,\"mirek_schema\":{\"mirek_"
      "minimum\":153,\"mirek_maximum\":500}},\"color_temperature_delta\":{},"
      "\"color\":{\"xy\":{\"x\":0.5612,\"y\":0.4042},\"gamut\":{\"red\":{\"x\":"
      "0.6915,\"y\":0.3083},\"green\":{\"x\":0.17,\"y\":0.7},\"blue\":{\"x\":0."
      "1532,\"y\":0.0475}},\"gamut_type\":\"C\"},\"dynamics\":{\"status\":"
      "\"none\",\"status_values\":[\"none\",\"dynamic_palette\"],\"speed\":0,"
      "\"speed_valid\":false},\"alert\":{\"action_values\":[\"breathe\"]},"
      "\"signaling\":{\"signal_values\":[\"no_signal\",\"on_off\",\"on_off_"
      "color\",\"alternating\"]},\"mode\":\"normal\",\"effects\":{\"status_"
      "values\":[\"no_effect\",\"candle\",\"fire\",\"prism\",\"sparkle\","
      "\"opal\",\"glisten\"],\"status\":\"no_effect\",\"effect_values\":[\"no_"
      "effect\",\"candle\",\"fire\",\"prism\",\"sparkle\",\"opal\",\"glisten\"]"
      "},\"effects_v2\":{\"action\":{\"effect_values\":[\"no_effect\","
      "\"candle\",\"fire\",\"prism\",\"sparkle\",\"opal\",\"glisten\"]},"
      "\"status\":{\"effect\":\"no_effect\",\"effect_values\":[\"no_effect\","
      "\"candle\",\"fire\",\"prism\",\"sparkle\",\"opal\",\"glisten\"]}},"
      "\"powerup\":{\"preset\":\"powerfail\",\"configured\":true,\"on\":{"
      "\"mode\":\"previous\"},\"dimming\":{\"mode\":\"previous\"},\"color\":{"
      "\"mode\":\"previous\"}},\"type\":\"light\"}]}";

  // char *room_response_copy = strdup(room_response);
  // find_target_room_id(room_response_copy, HUE_BRIDGE_ROOM_NAME

  char **device_ids = malloc(20 * sizeof(char *)); // TODO dynamic size
  int device_ids_count = 0;
  find_room_devices(room_response, "4f0f9068-b47f-429f-8e89-e692c9ef9bf1",
                    device_ids, &device_ids_count);
  if (device_ids_count == 0) {
    printf("Error finding room devices.\n");
    return;
  }

  printf("Device ids: %s\n", device_ids[0]);
  printf("Device ids: %s\n", device_ids[9]);
  printf("Device ids: %s\n", device_ids[10]);

  for (int i = 0; i < device_ids_count; i++) {
    free(device_ids[i]);
  }
  free(device_ids);

  // char *light_id = parse_device_response_to_light_id(device_response);
  //  if (!light_id) {
  //    printf("Error parsing light id.\n");
  //    return;
  //  }
  //  printf("Light id: %s\n", light_id);

  light_state_t light_state;
  parse_light_state(light_response, &light_state);
}
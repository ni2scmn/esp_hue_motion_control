#include "mdns.h"

esp_err_t setup_mdns(void) {
  // initialize mDNS service
  esp_err_t err = mdns_init();
  if (err) {
    printf("MDNS Init failed: %d\n", err);
    return;
  }

  // set hostname
  mdns_hostname_set("esp32-motion-control");
  // set default instance
  mdns_instance_name_set("ESP32 Motion Control");

  mdns_service_add(NULL, "_hue_motion_control", "_tcp", 80, NULL, 0);
}
#pragma once

#include "driver/gpio.h"

#define PIR_SENSOR_PIN GPIO_NUM_18 // Replace with your chosen GPIO pin
#define LED_BUILTIN GPIO_NUM_2 // Built-in LED pin for ESP32 (usually GPIO 2)

void setup_motion_sensor_led();
void setup_pir_motion_sensor();

int read_pir_motion_sensor();
void motion_sensor_led_on();
void motion_sensor_led_off();
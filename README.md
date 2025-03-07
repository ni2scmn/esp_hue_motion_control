# ESP Hue Motion Control

A **work-in-progress** project to control Philips Hue lights using motion detection with an ESP32. This project uses the ESP-IDF framework to integrate a motion sensor (e.g., a PIR sensor) and communicate with your Philips Hue Bridge for automated light control.

## Features

- **Motion Detection:** Automatically triggers actions when motion is detected.
- **Philips Hue Integration:** Communicates with Philips Hue lights through the Hue Bridge.
- **Modular Architecture:** Organized using ESP-IDF components for scalability and maintenance.
- **Simulation Ready:** Includes a Wokwi configuration for virtual testing before deployment.

## Requirements

### Hardware:
- ESP32 development board
- Motion sensor (e.g., PIR sensor)
- Philips Hue lights and Hue Bridge

### Software:
- [ESP-IDF](https://github.com/espressif/esp-idf) (latest version recommended)
- CMake
- [Wokwi](https://wokwi.com/) for simulation (optional)

## Installation

1. **Clone the Repository:**
   `git clone https://github.com/ni2scmn/esp_hue_motion_control.git`
   `cd esp_hue_motion_control`

2. **Set Up the ESP-IDF Environment**: \
   Follow the ESP-IDF setup instructions for your operating system.

3. **Configure the Project**:\
   Run the configuration tool to set up Wi-Fi, sensor, and Philips Hue settings:\
   `idf.py menuconfig`

4. **Build the Project**:\
   `idf.py build`

5. **Flash and Monitor**:\
   Flash the firmware to your ESP32 and start the serial monitor:\
    `idf.py flash monitor`


## Configuration

Project settings can be adjusted in the `esp_idf_project_configuration.json` file and via `menuconfig`. Key parameters include:
- **Wi-Fi Settings:** SSID and password for your Wi-Fi network.
- **Hue Bridge Settings:** IP address and API key for your Hue Bridge.
- **Motion Sensor Settings:** GPIO pin and motion detection threshold.

## Simulation

For testing, a [Wokwi](wokwi.toml) configuration is provided. This allows you to simulate the project in a virtual environment to verify functionality before deploying to hardware.

## Project Structure

```text
esp_hue_motion_control/
├── components/                      # Custom components for motion detection and Hue control
├── CMakeLists.txt                   # Build configuration
├── esp_idf_project_configuration.json  # Project configuration file
├── diagram.json                     # System diagram
├── wokwi.toml                       # Wokwi simulation configuration
└── README.md                        # This README file
```

## Future Work
- **Improved Motion Algorithms**: Enhance accuracy and reduce false triggers.
- **Dynamic Light Effects**: Develop more sophisticated lighting control based on motion patterns.
- **Web Dashboard**: Create a web interface for real-time monitoring and manual control.
- **Power Optimization**: Optimize for lower power consumption in battery-powered setups.

_**This project is a work in progress. Your feedback and contributions are appreciated as we continue to develop and improve the project.**_
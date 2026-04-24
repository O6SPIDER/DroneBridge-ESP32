# DroneBridge ESP32 Telemetry Bridge

A lightweight ESP32-based bridge that captures MAVLink telemetry from a flight controller and forwards it to **ThingsBoard** via MQTT over WiFi.

## 🚀 Features

- **MAVLink Integration**: Decodes `GLOBAL_POSITION_INT`, `SYS_STATUS`, and `VFR_HUD` messages.
- **Real-time Telemetry**: Streams Latitude, Longitude, Altitude, Battery Voltage, Groundspeed, and Heading.
- **ThingsBoard Support**: Uses MQTT to send data in a JSON format compatible with ThingsBoard telemetry.
- **WiFi Connectivity**: Automatic reconnection logic for stable data transmission.

## 🛠️ Hardware Requirements

- **ESP32** (Development Module)
- **Flight Controller** (with MAVLink support, e.g., Pixhawk, Matek)
- **Connections**:
  - ESP32 Pin 16 (RX2) -> Flight Controller TX
  - ESP32 Pin 17 (TX2) -> Flight Controller RX
  - Common Ground (GND)

## 📦 Dependencies

Ensure you have the following libraries installed in your Arduino IDE / PlatformIO:
- `WiFi` (built-in)
- `PubSubClient` (by Nick O'Leary)
- `ArduinoJson` (by Benoit Blanchon)
- `MAVLink` (Oleg Kalachev's version recommended)

## ⚙️ Configuration

Open the source file and update your credentials:

```cpp
// WiFi Settings
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

// ThingsBoard Settings
const char* mqtt_server = "thingsboard.cloud";
const char* token = "YOUR_DEVICE_TOKEN";
```

## 📊 Data Mapping

The following telemetry keys are sent to ThingsBoard:

| Key | Description | Unit |
| :--- | :--- | :--- |
| `lat` | Latitude | Degrees |
| `lon` | Longitude | Degrees |
| `alt` | Relative Altitude | Meters |
| `battery` | Battery Voltage | Volts |
| `groundspeed` | Speed over ground | m/s |
| `heading` | Compass Heading | 0-360° |
| `rssi` | WiFi Signal Strength | dBm |

## 🕹️ Port Settings
- **Debug Serial**: 115200 baud (USB)
- **MAVLink Serial**: 57600 baud (Serial2)

---
*Created for the DroneBridge-ESP32 project.*

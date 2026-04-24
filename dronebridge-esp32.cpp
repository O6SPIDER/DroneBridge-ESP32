#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <MAVLink.h> // Using Oleg Kalachev's library

// --- WIFI CONFIGURATION ---
const char* ssid = "Samsung Galaxy A04s";
const char* password = "001234567899";

// --- THINGSBOARD CONFIGURATION ---
const char* mqtt_server = "thingsboard.cloud";
const char* token = "d4ijjg1xgpt93duejg6v";

// --- TELEMETRY VARIABLES ---
float lat = 0.0;
float lon = 0.0;
float alt = 0.0;
float battery = 0.0;
float groundspeed = 0.0;
float heading = 0.0;

WiFiClient espClient;
PubSubClient mqtt(espClient);

void setup_wifi() {
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");
}

void reconnect() {
  while (!mqtt.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (mqtt.connect("ESP32_Drone_Client", token, NULL)) {
      Serial.println("connected!");
    } else {
      Serial.print("failed, rc=");
      Serial.print(mqtt.state());
      delay(5000);
    }
  }
}

// --- MAVLINK DATA RETRIEVAL ---
void readMAVLink() {
  mavlink_message_t msg;
  mavlink_status_t status;

  while (Serial2.available() > 0) {
    uint8_t c = Serial2.read();

    if (mavlink_parse_char(MAVLINK_COMM_0, c, &msg, &status)) {
      switch (msg.msgid) {

        case MAVLINK_MSG_ID_GLOBAL_POSITION_INT: {
          mavlink_global_position_int_t pos;
          mavlink_msg_global_position_int_decode(&msg, &pos);
          lat = pos.lat / 1e7;
          lon = pos.lon / 1e7;
          alt = pos.relative_alt / 1000.0; // Altitude above home in meters
          break;
        }

        case MAVLINK_MSG_ID_SYS_STATUS: {
          mavlink_sys_status_t sys;
          mavlink_msg_sys_status_decode(&msg, &sys);
          battery = sys.voltage_battery / 1000.0; // Convert millivolts to Volts
          break;
        }

        case MAVLINK_MSG_ID_VFR_HUD: {
          mavlink_vfr_hud_t hud;
          mavlink_msg_vfr_hud_decode(&msg, &hud);
          groundspeed = hud.groundspeed; // m/s
          heading = hud.heading;         // 0-360 degrees
          break;
        }
      }
    }
  }
}

void setup() {
  // Debug Serial for MacBook
  Serial.begin(115200);

  // MAVLink Serial (Connect to Matek Telemetry Port)
  // Pin 16 = RX2 (Connect to Matek TX)
  // Pin 17 = TX2 (Connect to Matek RX)
  Serial2.begin(57600, SERIAL_8N1, 16, 17);

  setup_wifi();
  mqtt.setServer(mqtt_server, 1883);
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) setup_wifi();
  if (!mqtt.connected()) reconnect();

  mqtt.loop();

  // Continuously listen for data from the Matek board
  readMAVLink();

  // Send telemetry to ThingsBoard every 5 seconds
  static unsigned long lastSend = 0;
  if (millis() - lastSend > 5000) {
    StaticJsonDocument<512> doc;

    // Using the short keys as requested
    doc["lat"] = lat;
    doc["lon"] = lon;
    doc["alt"] = alt;
    doc["battery"] = battery;
    doc["groundspeed"] = groundspeed;
    doc["heading"] = heading;
    doc["rssi"] = WiFi.RSSI(); // Changed from 'signal' to 'rssi'

    char buffer[512];
    serializeJson(doc, buffer);

    if (mqtt.publish("v1/devices/me/telemetry", buffer)) {
      Serial.print("Data Sent: ");
      Serial.println(buffer);
    } else {
      Serial.println("Failed to send to ThingsBoard");
    }

    lastSend = millis();
  }
}
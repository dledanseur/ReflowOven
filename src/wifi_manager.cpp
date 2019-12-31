#include "WiFiMulti.h"
#include "esp_wifi.h"
#include "wifi_manager.hpp"


WiFiMulti WiFiMulti;
WiFiClient wifiClient;


void WifiManager::connect(const char* ssid, const char* password) {
    // We start by connecting to a WiFi network
    WiFi.persistent(false);
    WiFiMulti.addAP(ssid, password);

    Serial.printf("Connecting to %s\n",ssid);
    Serial.flush();
    while (WiFiMulti.run() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

    
    wifi_config_t current_conf;
    esp_wifi_get_config(WIFI_IF_STA, &current_conf);
    current_conf.sta.listen_interval = 50;
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &current_conf));
    esp_wifi_set_ps(WIFI_PS_MAX_MODEM);

    WiFi.onEvent(WiFiEvent);
}

void WifiManager::WiFiEvent(WiFiEvent_t event)
{
  Serial.printf("[WiFi-event] event: %d\n", event);

  switch (event) {
  case SYSTEM_EVENT_STA_GOT_IP:
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    break;
  case SYSTEM_EVENT_STA_DISCONNECTED:
    Serial.println("WiFi lost connection.  Attempting to reconnect...");
    WiFi.reconnect();
    break;
  case SYSTEM_EVENT_STA_START:
    Serial.println("ESP32 station start");
    break;
  case SYSTEM_EVENT_STA_CONNECTED:
    Serial.println("ESP32 station connected to AP");
    break;
  default:			
    Serial.println("Unhandled WiFi Event raised.");
    break;
  }

}

int status = 0;
void WifiManager::callback(char* topic, byte* payload, unsigned int length) {
    /*for (int i=0; i<length; i++) {
        Serial.print((char) payload[i]);
    }
    Serial.println();
    Serial.flush();*/
    digitalWrite(32, status);
    digitalWrite(33, 1-status);
    status = 1 - status;
}

void WifiManager::loop() {
  //esp_wifi_start()
  if (WiFi.status() != WL_CONNECTED) {
    WiFi.reconnect();
    /*esp_wifi_start();*/
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
  }
  delay(10);
  //esp_wifi_stop();
}




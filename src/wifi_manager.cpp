#include <WiFiMulti.h>
#include <esp_wifi.h>
#include <ESPmDNS.h>
#include <ArduinoLog.h>

#include "wifi_manager.hpp"

WiFiMulti WiFiMulti;
WiFiClient wifiClient;


void WifiManager::connect(const char* ssid, const char* password) {
    // We start by connecting to a WiFi network
    WiFi.persistent(false);
    WiFiMulti.addAP(ssid, password);

    Log.notice("Connecting to %s" CR,ssid);
    while (WiFiMulti.run() != WL_CONNECTED) {
        delay(500);
    }

    Log.notice("WiFi connected. IP address: %s", WiFi.localIP());  
    
    wifi_config_t current_conf;
    esp_wifi_get_config(WIFI_IF_STA, &current_conf);
    current_conf.sta.listen_interval = 50;
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &current_conf));
    esp_wifi_set_ps(WIFI_PS_MAX_MODEM);

    // WiFi.onEvent(WiFiEvent);
}

/*void WifiManager::WiFiEvent(WiFiEvent_t event)
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

}*/

void WifiManager::loop() {
  if (WiFi.status() != WL_CONNECTED) {
    Log.notice("WIFI not connected. Reconnecting" CR);
    int attempts=0;
    WiFi.reconnect();
    while (WiFi.status() != WL_CONNECTED) {
        if (attempts % 10 == 0) {
          Log.notice("WIFI not connected. Reconnecting" CR);
          WiFi.reconnect();
        }
        attempts ++;
        delay(500);
    }
  }
  delay(10);
}

void WifiManager::enableMdns(const char * domain) {
  if (!MDNS.begin(domain)) {
      Log.warning("Error setting up MDNS responder!" CR);
      while(1) {
          delay(1000);
      }
  }
}




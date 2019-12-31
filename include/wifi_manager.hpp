#include <Arduino.h>
#include <WiFi.h>

class WifiManager {
    private:

    public:
    void connect(const char* ssid, const char* password);
    static void callback(char* topic, byte* payload, unsigned int length);
    static void WiFiEvent(WiFiEvent_t event);
    void loop();
};
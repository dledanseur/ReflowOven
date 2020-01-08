#include <Arduino.h>
#include <WiFi.h>

class WifiManager {
    private:

    public:
    void connect(const char* ssid, const char* password);
    void enableMdns(const char* domain);
    //static void WiFiEvent(WiFiEvent_t event);
    void loop();
};
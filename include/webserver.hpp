#include <Arduino.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include "soldermanager.hpp"
#include "touchmanager/touch_manager.hpp"
#include "commands.hpp"

#ifndef WEBSERVER_HPP
#define WEBSERVER_HPP

class WebServer: private SolderManagerListener, private CommandListener {
  typedef void (*handler_ptr)(AsyncWebServerRequest *);
  typedef const std::function <void (JsonObject&)>& dataFormatter;
  private:
    static SolderManager* m_solderManager;
    static TouchManager* m_touchManager;
    static State* m_state;
    static WebServer* m_instance;
    static AsyncWebServer* webServer;
    static AsyncWebSocket* webSocket;

    static void getAllPoints(AsyncWebServerRequest *request);
    static void pointToJsonObject(const Point& p, JsonObject& o);
    
    virtual void started(String profile);
    virtual void newPoint(const Point& point);
    virtual void stopped();

    virtual void ranCommand(Command& command);

    static void onEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, 
                void * arg, uint8_t *data, size_t len);

    static void textMessage(String messageType, dataFormatter fomatter=NULL);

    WebServer(); 
    void addHandler(const char* endpoint, WebRequestMethod method, handler_ptr handler);
    void addRedirect(const char* from, const char* to);
  public:
    static WebServer& getInstance(SolderManager* solderManager, TouchManager* touchManager,
                                  State* state );
    void start();
    ~WebServer();
};



#endif
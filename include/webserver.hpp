#include <Arduino.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include "soldermanager.hpp"

#ifndef WEBSERVER_HPP
#define WEBSERVER_HPP

class WebServer: private SolderManagerListener {
  typedef void (*handler_ptr)(AsyncWebServerRequest *);
  typedef const std::function <void (JsonObject&)>& dataFormatter;
  private:
    static SolderManager* m_solderManager;
    static WebServer* m_instance;
    AsyncWebServer* webServer;
    AsyncWebSocket* webSocket;

    static void getAllPoints(AsyncWebServerRequest *request);
    static void pointToJsonObject(const Point& p, JsonObject& o);
    
    virtual void started(String profile);
    virtual void newPoint(const Point& point);
    virtual void stopped();

    WebServer(); 
    void addHandler(const char* endpoint, WebRequestMethod method, handler_ptr handler);
    void textMessage(String messageType, dataFormatter fomatter=NULL);
  public:
    static WebServer& getInstance(SolderManager* solderManager);
    void start();
    ~WebServer();
};



#endif
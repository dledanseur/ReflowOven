#include <SPIFFS.h>

#include "webserver.hpp"
#include "AsyncJson.h"
#include <ArduinoLog.h>

#define CONTENT_TYPE_JSON "application/json"
#define HTTP_OK 200

WebServer* WebServer::m_instance = NULL;
SolderManager* WebServer::m_solderManager = NULL;

WebServer& WebServer::getInstance(SolderManager* solderManager) {
    if (m_instance != NULL) {
        return *m_instance;
    }
    m_solderManager = solderManager;
    m_instance = new WebServer();
    return *m_instance;
}

void WebServer::textMessage(String messageType, dataFormatter data) {
    DynamicJsonDocument doc(1024);
    doc.to<JsonObject>();

    String dest;
    doc["type"] = messageType;
    if (data) {
        auto obj = doc.createNestedObject("data");
        data(obj);
    }
    serializeJson(doc, dest);
    webSocket->textAll(dest);

}

void WebServer::newPoint(const Point& p) {
    auto formatter = [p](JsonObject& dest) { 
        WebServer::pointToJsonObject(p, dest);
    };
    
    textMessage("newPoint", formatter);
}

void WebServer::started(String profile) {
    auto formatter = [profile](JsonObject& dest) { 
        dest["profile"] = profile;
    };

    textMessage("started", formatter);
}

void WebServer::stopped() {
    textMessage("stopped");
}

WebServer::WebServer() {
    webServer = new AsyncWebServer(80);
    webSocket = new AsyncWebSocket("/ws");
    webServer->addHandler(webSocket);
    webServer->serveStatic("/app", SPIFFS, "/");

    m_solderManager->addListener(*this);
    addHandler("/points", HTTP_GET, &getAllPoints);
    
    DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");
    DefaultHeaders::Instance().addHeader("Access-Control-Allow-Headers", "content-type");

}

void WebServer::start() {
    webServer->begin();
}
void WebServer::addHandler(const char* endpoint, WebRequestMethod method, handler_ptr handler) {
    webServer->on(endpoint, method, handler);
}

void WebServer::pointToJsonObject(const Point& p, JsonObject& o) {
    o["millis"] = p.millis;
    o["expectedTemperature"] = p.expected_temp;
    o["temperature"] = p.mesured_temperature;
    o["state"] = (int) p.state;
}

void WebServer::getAllPoints(AsyncWebServerRequest *request) {
    //auto response = request->beginResponseStream(CONTENT_TYPE_JSON);
    auto allPoints = m_solderManager->getAllPoints();

    AsyncJsonResponse * response = new AsyncJsonResponse();
    auto root = response->getRoot();

    root.to<JsonArray>();

    for(auto p: allPoints) {
        auto o = root.createNestedObject();
        pointToJsonObject(*p, o);
    }

    response->setLength();

    request->send(response);

}

WebServer::~WebServer() {
    delete webSocket;
    webSocket = NULL;
    delete webServer;
    webServer = NULL;
}
#include <SPIFFS.h>

#include "webserver.hpp"
#include "AsyncJson.h"
#include <ArduinoLog.h>


#define CONTENT_TYPE_JSON "application/json"
#define HTTP_OK 200

WebServer* WebServer::m_instance = NULL;
SolderManager* WebServer::m_solderManager = NULL;
TouchManager* WebServer::m_touchManager = NULL;
State* WebServer::m_state = NULL;
AsyncWebServer* WebServer::webServer = NULL;
AsyncWebSocket* WebServer::webSocket = NULL;

WebServer& WebServer::getInstance(SolderManager* solderManager, TouchManager* touchManager,
                                  State* state) {
    if (m_instance != NULL) {
        return *m_instance;
    }
    m_solderManager = solderManager;
    m_touchManager = touchManager;
    m_state = state;
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
    SPIFFS.begin(true);

    webServer = new AsyncWebServer(80);
    webSocket = new AsyncWebSocket("/ws");
    webSocket->onEvent(onEvent);
    webServer->addHandler(webSocket);
    webServer->serveStatic("/app", SPIFFS, "/");

    m_solderManager->addListener(*this);
    m_touchManager->addListener(this);

    addRedirect("/", "/app/index.html");
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

void WebServer::addRedirect(const char* from, const char* to) {
    webServer->on(from, HTTP_GET, [to](AsyncWebServerRequest* request) {
        request->redirect(to);
    });
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

void WebServer::ranCommand(Command& command) {
    if (command.getTag() == UpCommand::TAG || command.getTag() == DownCommand::TAG) {
        auto profile = m_state->currentProfile;
        auto formatter = [profile](JsonObject& dest) { 
            dest["profile"] = profile->name;
        };

        textMessage("changedProfile", formatter);
    }
}

void WebServer::onEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len){
    if(type == WS_EVT_CONNECT){
        //client connected
        auto profile = m_state->currentProfile;
        auto formatter = [profile](JsonObject& dest) { 
            dest["profile"] = profile->name;
            dest["state"] = (m_solderManager->isFinished() ? "Stopped" : "Started");
            dest["solderState"] = (int) m_solderManager->getSolderState();
        };

        textMessage("state", formatter);
    }
    else if(type == WS_EVT_DATA){
        data[len] = 0;
        Log.notice("Received message %s" CR,data);
        StaticJsonDocument<50> doc;
        DeserializationError error = deserializeJson(doc, data);

        if (error) {
            Log.warning("parseObject() failed");
            return;
        }

        const char* type = doc["type"];
        if (String(type) == "nextProfile") {
            Log.verbose("Next profile");
            m_touchManager->runRegisteredCommand(DownCommand::TAG);
        }
        else if (String(type) == "previousProfile") {
            Log.verbose("Previous profile");
            m_touchManager->runRegisteredCommand(UpCommand::TAG);
        }
        else if (String(type) == "start") {
            Log.verbose("Start");
            m_touchManager->runRegisteredCommand(StartCommand::TAG);
        }
        else if (String(type) == "stop") {
            Log.verbose("Stop");
            m_touchManager->runRegisteredCommand(StopCommand::TAG);
        }
    }
}
WebServer::~WebServer() {
    delete webSocket;
    webSocket = NULL;
    delete webServer;
    webServer = NULL;
}
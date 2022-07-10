#ifndef ServerConnect_h
#define ServerConnect_h

#include "ESP_Manager.h"

class ServerConnect
{
public:
    
    ServerConnect(WiFiClient* WiiFiclient, WebSocketClient* webSocketClient,AsyncWebSocket* asyncWebSocket);
    void        ClientSetting(char* path,char* host,uint16_t port);

    // ~ServerConnect();

    void        WebSocketConnect(char* host, const uint16_t port = 443);
    void        HandShakeConnect(char* host,char* path="/");

    void        ConnectStatus(wl_status_t status, JsonDocument &data);
    void        ConnectingWiFi(JsonDocument &data);
    void        SuccessWiFi(JsonDocument &data);

    void        internalCodeSync();


private:
    WiFiClient* client;
    WebSocketClient* WSC;
    AsyncWebSocket* AsynWS;


    char* _path;
    char* _host;
    uint16_t _port;
};

// extern ServerConnect ServerTools;


#endif

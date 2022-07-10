#ifndef InternalCode_h
#define InternalCode_h

#include "setup.h"
#include "ESP_Manager.h"

class InternalCode
{
public:
    InternalCode(AsyncWebServer* webserver, AsyncWebSocket* websocket,WebSocketClient* webSocketClient);

    // ~InternalCode();

    void            setup(AwsEventHandler onEvent);
    

    uint8_t         dBmtoPercentage(int32_t dBm);
    void            WiFiFilter(String *WiFiList, uint8_t *a, const uint8_t lt_length);

    void            allServerOn(const char *dirname);
    void            handle_Connect_WiFi();
    void            handle_LED_Button(const JsonDocument &data);
    void            Connect_SERVER(const JsonDocument &data);

private:
    AsyncWebServer      *server;
    AsyncWebSocket      *ws;
    WebSocketClient     *WSC;
};

// extern InternalCode InternalTools;

#endif

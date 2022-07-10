#ifndef SetUp_h
#define SetUp_h

#include "InternalCode.h"
#include "ServerConnect.h"
#include "ESP_Manager.h"

class SetUp
{
public:
    bool    CHECK = true;
    void    setup();

    void    onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type,void *arg, uint8_t *data, size_t len);

    void    ServerSend(WiFiClient* client,WebSocketClient* webSocketClient,AsyncWebSocket* ws);


private:
    unsigned int count = 0;
    // bool CHECK = true;
};

extern SetUp setupTools;

#endif

#include "ServerConnect.h"

ServerConnect::ServerConnect(WiFiClient *WiiFiclient, WebSocketClient *webSocketClient, AsyncWebSocket *asyncWebSocket)
{
    client = WiiFiclient;
    WSC = webSocketClient;
    AsynWS = asyncWebSocket;
}

/*
    path : Socket통신할 경로 지정
    host : 통신할 IP 주소
    port : 통신할 포트
*/
void ServerConnect::ClientSetting(char* path, char* host, uint16_t port)
{
    // 글로벌 초기화
    _path = path;
    _host = host;
    _port = port;
}

//////////////////////////////////////////

// ServerConnect::~ServerConnect()
// {
//     free();
// }

//////////////////////////////////////////

// WIFI 연결 확인
void ServerConnect::ConnectStatus(wl_status_t status, JsonDocument &data)
{
    
    File configFile = SPIFFS.open("/config.json", "w");
    String sendTEXT;
    // 와이파이 연결 성공시
    if (status == WL_CONNECTED)
    {
        if (!configFile)
        {
            Serial.println("failed to open config file for writing");
        }
        else
        {
            // 성공 내용 저장
            data["WiFi"] = "SUCCESS";
            data["IP"] = WiFi.localIP().toString();

            serializeJson(data, sendTEXT);
            Serial.println("Send HTML DATA: "+sendTEXT);
            AsynWS->textAll(sendTEXT);

            serializeJson(data, configFile);
            data.clear();
        }
        configFile.close();

        delay(333);
        WiFi.mode(WIFI_STA);

        // 와이파이만 연결 성공
        Serial.println("\r\nWiFi Connected!!");
        Serial.print("IP address: ");
        Serial.println(WiFi.localIP());
        Serial.println();

        // 서버연결 시도
        WebSocketConnect(_host, _port);
        // 서버페이지 연결 시도
        HandShakeConnect(_host, _path);
    }
    // 와이파이 연결 실패시
    else if (status == WL_DISCONNECTED)
    {
        if (!configFile)
        {
            Serial.println("failed to open config file for writing");
        }
        else
        {
            data["WiFi"] = "FAIL";
            data["IP"] = WiFi.softAPIP().toString();

            serializeJson(data, sendTEXT);
            Serial.println("Send HTML DATA: "+sendTEXT);
    
            AsynWS->textAll(sendTEXT);

            serializeJson(data, configFile);
            data.clear();
        }
        
        delay(100);
        Serial.println("WiFi Connect Fail...");
        WiFi.mode(WIFI_AP);

        Serial.print("IP address: ");
        Serial.println(WiFi.softAPIP());

        SPIFFS.remove("/config.json");
    }
}

// 서버와 연결
void ServerConnect::WebSocketConnect(char* host, const uint16_t port)
{
    Serial.println("host: " + String(host) + ", Port: " + String(port));
    Serial.print("Connecting to WebSocket");

    for (uint8_t i = 0; i < 3; i++)
    {
        Serial.print(".");
        delay(333);
    }

    while (!client->connect(host, port))
        Serial.print(".");

    delay(1000);
    Serial.println("\r\nWebSocket Connected!!\r\n");
}

// 서버 접속 성공후 해당 페이지 확인
void ServerConnect::HandShakeConnect(char* host, char* path)
{

    WSC->path = path;
    WSC->host = host;
    Serial.println("Connecting to Path( " + String(path) + " )");

    while (!WSC->handshake(*client))
    {
        Serial.print(".");
        delay(500);
    }

    // 모든접속 성공
    File configFile = SPIFFS.open("/Handshake_SUCCESS", "w");
    Serial.println();

    Serial.println("\r\nHandshake SUCCESS!!");
    Serial.println("Enjoy The WebSocket!\r\n\r\n");
}

// 와이파이 연결 시도
void ServerConnect::ConnectingWiFi(JsonDocument &data)
{
    const char *ssid = data["ssid"];
    const char *pass = data["password"];
    unsigned long startTime = millis();

    // 와이파이 내용 출력
    Serial.print("WiFi Name: ");
    Serial.print(ssid);
    Serial.print("  |  PW: ");
    Serial.print(pass);
    Serial.println();

    // 2가지 모드 모두켜기
    WiFi.mode(WIFI_AP_STA);
    WiFi.begin(ssid, pass);

    Serial.print("WIFI Connecting");
    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print(".");
        delay(333);

        // 15초동안 연결 안될시 빠져나옴
        if ((unsigned long)(millis() - startTime) >= 15000)
        {
            SPIFFS.remove("/config.json");
            break;
        }
    }

    // 와이파이 상태
    Serial.print("\r\n와이파이 연결 상태 : ");
    Serial.println(WiFi.status());

    ConnectStatus(WiFi.status(), data);
}

// 기존 와이파이 연결 되어 있는 상태
void ServerConnect::SuccessWiFi(JsonDocument &data)
{
    const char *ssid = data["ssid"];
    const char *pass = data["password"];
    unsigned long startTime = millis();

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, pass);

    Serial.print("WIFI Connecting");
    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print(".");
        delay(333);

        // 10초동안 연결 안될시 빠져나옴
        if ((unsigned long)(millis() - startTime) >= 10000)
        {
            SPIFFS.remove("/config.json");
            break;
        }
    }

    ConnectStatus(WiFi.status(), data);
}

// ServerConnect ServerTools;

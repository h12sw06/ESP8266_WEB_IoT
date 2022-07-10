#include "setup.h"

void SetUp::setup()
{
    // LED 기본 설정
    pinMode(GPIO0, OUTPUT);
    pinMode(GPIO2, OUTPUT);
    digitalWrite(GPIO0, LED_OFF);
    digitalWrite(GPIO2, LED_OFF);

    // 시리얼 기본 설정
    Serial.begin(115200);
    while (!Serial)
    {
        digitalWrite(GPIO2, !digitalRead(GPIO2));
        delay(500);
    };
    delay(200);

    // AP모드 설정
    WiFi.disconnect();
    WiFi.mode(WIFI_AP);
    WiFi.softAP(WIFI_SSID, WIFI_PASSWORD);
    WiFi.begin();

    // 파일 읽기 기본 설정
    while (!SPIFFS.begin())
        Serial.println(".");
    delay(100);

    Serial.println("\r\n--- Setup Complete!! ---");
    Serial.print("Handshake_SUCCESS 파일 제거: ");
    Serial.println(SPIFFS.remove("/Handshake_SUCCESS"));
}

/*
    WiFiClient* client : WiFiClient 따름 (예 &client)
    WebSocketClient* webSocketClient : WebSocketClient 따름 (예 &webSocketClient)
    AsyncWebSocket* ws : AsyncWebSocket 따름 (예 &ws)
*/
void SetUp::ServerSend(WiFiClient* client,WebSocketClient* webSocketClient,AsyncWebSocket* ws)
{
    String SEND_data;
    String CONTROL_data;

    // 클라이언트 연결될때까지 대기
    if (client->connected())
    {
        delay(1000);

        // 서버로부터 받기
        webSocketClient->getData(CONTROL_data);
        Serial.println(CONTROL_data);

        // ON 했을떄
        if (CONTROL_data == "ON" || CONTROL_data == "on"|| CONTROL_data == "On")
        {
            digitalWrite(GPIO2, LED_ON); // Turn the LED on (Note that LOW is the voltage level
            digitalWrite(GPIO0, LED_ON); // Turn the LED on (Note that LOW is the voltage level
            webSocketClient->sendData(CONTROL_data);

            ws->textAll(String(!digitalRead(GPIO2)));

        }
        // OFF 했을때
        else if (CONTROL_data == "OFF"||CONTROL_data == "off"||CONTROL_data == "Off")
        {
            digitalWrite(GPIO2, LED_OFF); // Turn the LED off by making the voltage HIGH
            digitalWrite(GPIO0, LED_OFF); // Turn the LED off by making the voltage HIGH
            webSocketClient->sendData(CONTROL_data);
    
            ws->textAll(String(!digitalRead(GPIO2)));
        }

        SEND_data = "TEST ESP8266 -> " + String(count++);
        // 서버로 전송
        webSocketClient->sendData(SEND_data);

        // return true;
    }
    /*
    else
    {
        Serial.println("Client Disconnected!!");
        Serial.print("Trying Connecting again");
        // client.stopAll();

        for (uint8_t i = 0; i < 5; i++)
        {
            Serial.print(".");
            delay(333);
        }

        Serial.println("\r\n");

        //연결될때까지 시도
        while (true)
        {
            // Connect to the websocket server
            WebSocketConnect(&client, host, port);

            //Handshake with the server
            HandShakeConnect(&webSocketClient, host, path);

            delay(3000);

            if (client.connected())
            {
                break;
            }
        }

        // return false;
    }
    //*/
}

SetUp setupTools;

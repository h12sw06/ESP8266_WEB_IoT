#include "setup.h"

// 로컬 서버 연결
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

// 클라이언트 연결
WiFiClient client;
WebSocketClient webSocketClient;

// 로컬, 클라이언트 정의
InternalCode InternalTools(&server,&ws,&webSocketClient);
ServerConnect ServerTools(&client,&webSocketClient,&ws);



// 외부 서버 기본 설정
char path[] = "/외부_웹서버_하위경로";
char host[] = "외부.ip.주소(http/https 제외)";
const uint16_t port = 80;

// WIFI 체크용 변수
bool CHECK = true;

//////////////// SETUP ////////////////////

// FS에 있는 HTML 연결(로컬 서버)
void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len)
{
    AwsFrameInfo *info = (AwsFrameInfo *)arg;
    DynamicJsonDocument doc(JSON_OBJECT_SIZE(2) + JSON_ARRAY_SIZE(0) + len);

    if (type == WS_EVT_DATA && info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT)
    {
        data[len] = 0; //쓰레기값 지우기
        deserializeJson(doc, (char *)data);
        Serial.println("----- 로컬 작동 -----");
        Serial.printf_P(PSTR("free heap memory: %d\n"), ESP.getFreeHeap());
    }

    switch (type) //페이지 접속 순서 {처음시작시 : [1->3->2] | 시작후 : [2->3->1]}
    {

        // 페이지 이동 성공 (1번)
        case WS_EVT_CONNECT:
            Serial.printf("\r\nWebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
            ws.textAll(String(digitalRead(GPIO2)));
            break;

            // 페이지 나갈시 (2번)
        case WS_EVT_DISCONNECT:
            Serial.printf("WebSocket client #%u disconnected\n", client->id());
            break;

        // 페이지 확인 (3번)
        case WS_EVT_DATA:
            //json전체 출력
            serializeJsonPretty(doc, Serial);
            Serial.println();

            // LED_Button.html일시
            if (doc["HTTP"] == "LED_Button.html")
            {
                InternalTools.handle_LED_Button(doc);
            }
            // control.html일시
            else if (doc["HTTP"] == "connect.html")
            {
                InternalTools.handle_Connect_WiFi();
            }
            // WIFI 입력 정보 Json파일로 가공
            else if (doc["WiFi"] == "CONNECT")
            {
                InternalTools.Connect_SERVER(doc);
                CHECK = true;
            }

            // Json파일 내용 클린
            doc.clear();

            // 확인용
            Serial.printf_P(PSTR("free heap memory: %d\n"), ESP.getFreeHeap());
            Serial.println("---------------------");
            break;

        // ping 요청시(사용안함)
        case WS_EVT_PONG:

        // 에러시(ESP8266기기 에러)
        case WS_EVT_ERROR:
            break;
    }
}

///////////////////////////////////////////




void setup()
{
    // 기본 설정
    setupTools.setup();
    // 외부 서버와 연결
    ServerTools.ClientSetting(path,host,port);

    // HTTP 기본설정
    // 루트로 지정할 경로 입력
    InternalTools.allServerOn("/LED_Button.html");

    // 최종 로컬 서버 시작
    InternalTools.setup(onEvent);
}

void loop()
{
    ws.cleanupClients();

    // 와이파이 연결 여부
    WiFiConnect();

    // 서버와 연결 통신
    setupTools.ServerSend(&client,&webSocketClient,&ws);
}




//////////////// LOOP ////////////////////

// 서버와 연결 세팅
void WiFiConnect()
{

    // 파일 불러오기
    if (SPIFFS.exists("/config.json"))
    {
        File configFile = SPIFFS.open("/config.json", "r"); //wifi 데이터 메모리에 읽기
        // 내용이 있고 로컬 작업 끝날시
        if (configFile && CHECK)
        {
            Serial.println("----- 외부 작동 -----");
            Serial.printf_P(PSTR("free heap memory: %d\n"), ESP.getFreeHeap());

            DynamicJsonDocument doc(JSON_OBJECT_SIZE(4) + JSON_ARRAY_SIZE(0) + configFile.size());
            deserializeJson(doc, configFile);
            configFile.close();

            serializeJsonPretty(doc, Serial);
            Serial.println();

            if (doc["WiFi"] == "CONNECT"){
                // 와이파이 연결 시도시
                ServerTools.ConnectingWiFi(doc);

            }else if(doc["WiFi"] == "SUCCESS"){
                // 재부팅시
                // ServerTools.ConnectingWiFi(doc);

                ServerTools.SuccessWiFi(doc);
            }else{
                Serial.println(SPIFFS.remove("/config.json"));
                WiFi.mode(WIFI_AP);
            }
            doc.clear();
            CHECK = false;

            Serial.printf_P(PSTR("free heap memory: %d\n"), ESP.getFreeHeap());
            Serial.println("---------------------");
        }
        configFile.close();
    }


}

//////////////////////////////////////////

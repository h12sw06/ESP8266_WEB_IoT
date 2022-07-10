#include "InternalCode.h"

InternalCode::InternalCode(AsyncWebServer *webserver, AsyncWebSocket *websocket, WebSocketClient *webSocketClient)
{
    server = webserver;
    ws = websocket;
    WSC = webSocketClient;
}

//////////////////////////////////////////

// InternalCode::~InternalCode()
// {
//     free();
// }

//////////////////////////////////////////

/*
    AwsEventHandler onEvent 함수 따름
*/
void InternalCode::setup(AwsEventHandler onEvent)
{
    // 최종 서버 시작
    server->begin();
    ws->onEvent(onEvent);
    server->addHandler(ws);
    Serial.println("Server SUCCESS started!");
    Serial.println("------------------------");
}

// 와이파이 세기 계산
uint8_t InternalCode::dBmtoPercentage(int32_t dBm)
{
    uint8_t quality;
    if (dBm <= RSSI_MIN)
    {
        quality = 0;
    }
    else if (dBm >= RSSI_MAX)
    {
        quality = 100;
    }
    else
    {
        quality = 2 * (dBm + 100);
    }

    return quality;
} //dBmtoPercentage

// 와이파이 세기 순서 정렬
/*
WiFiList      : 와이파이 리스트(HTML형태의 문자열)
intensityList : 와이파이 세기 리스트
lt_length     : 와이파이 개수
*/
void InternalCode::WiFiFilter(String *WiFiList, uint8_t *intensityList, const uint8_t lt_length)
{
    // 선택정렬
    for (uint8_t i = 0; i < lt_length; i++)
    {
        for (uint8_t j = i + 1; j < lt_length; j++)
        {
            if (intensityList[j] > intensityList[i])
            {
                std::swap(intensityList[i], intensityList[j]);
                std::swap(WiFiList[i], WiFiList[j]);
            }
        }
    }
}

/*
    allServerOn(const char *dirname)
    allServerOn("루트로 지정할 경로 입력")
*/
void InternalCode::allServerOn(const char *dirname)
{
    const char rootDir[] = "/";

    // SPIFFS방식으로 ESP에 파일 넣기
    Dir dir = SPIFFS.openDir(rootDir);
    uint32_t i = 0;

    Serial.printf("Listing directory: %s\r\n", rootDir);
    // 모든데이터 있을때까지
    while (dir.next())
    {
        // 파일이름으로 저장
        String ServerOn = dir.fileName();
        Serial.print("Server On Complet File Name: ");
        Serial.print(ServerOn);
        // 파일 있는지 체크
        if (dir.fileSize())
        {
            File f = dir.openFile("r");
            Serial.print(", Size: ");
            Serial.println(f.size());
            i = i + f.size();

            // Async방식으로 html 넣기
            server->on(f.fullName(), HTTP_GET, [ServerOn](AsyncWebServerRequest *request) {
                request->send(SPIFFS, ServerOn, String(), false);
            });
        }
    }

    Serial.printf("Total File Size: %d (Bytes)\r\n", i);

    // html 루트 지정
    server->on("/", HTTP_GET, [dirname](AsyncWebServerRequest *request) {
        request->send(SPIFFS, dirname, String(), false);
    });

    Serial.println("Finshed All Client Setup!!");
}

// control.html
void InternalCode::handle_Connect_WiFi()
{
    // Async(비동기) 와이파이 스캔
    WiFi.scanNetworksAsync([&](int numNetworks) {
        uint8_t RSSIList[numNetworks];
        String WiFiList[numNetworks];
        // html 와이파이 스캔 생성
        String strList = "<ul>";

        // 와이파이 스캔 리스트(와이파이 약하면 여기서 제거)
        for (uint8_t i = 0; i < numNetworks; ++i)
        {
            // 와이파이 강도 변환
            RSSIList[i] = dBmtoPercentage(WiFi.RSSI(i));

            WiFiList[i] = "<li><a name='" + WiFi.SSID(i) + "' onclick='select(this.name)'>" + WiFi.SSID(i);
            WiFiList[i] += "<br>(ch:" + String(WiFi.channel(i)) + ",dBm:" + String(RSSIList[i]) + "%)";
            
            // 50%이하일시
            if (RSSIList[i] < 70 && RSSIList[i] > 50){
                WiFiList[i] += " [신호 보통]";
            }else if(RSSIList[i] <= 50){
                WiFiList[i] += " [신호 약함]<br><주의 : 오류 발생 강도 높음>";

            }

            WiFiList[i] += "</a></li>";
        }

        // 와이파이 강도 순서 정렬
        WiFiFilter(WiFiList, RSSIList, numNetworks);

        // 정렬된 와이파이 순서 최종 HTML변수 넣기
        for (uint8_t i = 0; i < numNetworks; i++)
        {
            strList += WiFiList[i];
        }

        strList += "</ul>";

        Serial.println(strList);
        // html에 뿌려줌
        ws->textAll(strList);
    });
}

// LED_Button.html
void InternalCode::handle_LED_Button(const JsonDocument &data)
{
    // html 버튼 클릭시 
    if (data["BUTTON1"] == "ON")
    {
        digitalWrite(GPIO0, LED_ON);//PIN 0 번 켜기
        digitalWrite(GPIO2, LED_ON);//PIN 2 번 켜기
    }
    else if (data["BUTTON1"] == "OFF")
    {
        digitalWrite(GPIO0, LED_OFF);//PIN 0 번 끄기
        digitalWrite(GPIO2, LED_OFF);//PIN 2 번 끄기
    }
    // 기기와 동기화
    ws->textAll(String(digitalRead(GPIO2)));

    // 외부 서버에서 통신할때
    File configFile = SPIFFS.open("/Handshake_SUCCESS", "r"); //wifi 데이터 메모리에 읽기
    if (configFile)
    {
        WSC->sendData(String(digitalRead(GPIO2)));
    }
    else
    {
        Serial.println("외부 서버와 연결할수 없습니다...");
    }
}

// 와이파이 연결중일때
void InternalCode::Connect_SERVER(const JsonDocument &data)
{
    File configFile = SPIFFS.open("/config.json", "w");
    if (!configFile)
    {
        Serial.println("failed to open config file for writing");
    }
    else
    {
        serializeJson(data, configFile);
    }
    configFile.close();
}

// InternalCode InternalTools;

#ifndef ESP_Manager_H
#define ESP_Manager_H

// 공통
#include "Arduino.h"
#include <ArduinoJson.h>

// InternalCode 필요
#include <ESPAsyncWebServer.h>

// ServerConnect 필요
#include <WebSocketClient.h>


/////////////////////////////////////////////////////////

// AP 이름 & 비번 정의
#define WIFI_SSID "ESP01_WIFI"
#define WIFI_PASSWORD "12345678"

// WIFI 세기 정의
#define RSSI_MAX -50  // define maximum strength of signal in dBm
#define RSSI_MIN -100 // define minimum strength of signal in dBm


// LED 정의
#define LED_ON HIGH
#define LED_OFF LOW

#define GPIO2 2
#define GPIO0 0


#endif    // ESPAsync_WiFiManager_Debug_H

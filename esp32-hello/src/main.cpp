#include <Arduino.h>
#include "SPIFFS.h"

#pragma region FastLED
#include "FastLED.h"
#define NUM_LEDS      150
#define LED_TYPE   WS2811
#define COLOR_ORDER   GRB
#define DATA_PIN        32
CRGB leds[NUM_LEDS];
#pragma endregion FastLED

#pragma region WifiManager
#include <WiFiManager.h>

WiFiManager wm;
#pragma endregion WifiManager

#pragma region WebServer
#include <DNSServer.h>
#ifdef ESP32
#include <WiFi.h>
#include <AsyncTCP.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#endif
#include "ESPAsyncWebServer.h"

void notFound(AsyncWebServerRequest *request);

AsyncWebServer server(80);
const char* PARAM_MESSAGE = "message";
#pragma endregion WebServer

#pragma region JSON
#include <ArduinoJson.h>
#pragma endregion JSON

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("Init!");

#pragma region SPIFFS
  if(!SPIFFS.begin(true)){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  File file = SPIFFS.open("/hello.txt");
  if(!file){
    Serial.println("Failed to open file for reading");
    return;
  }
  
  Serial.println("File Content:");
  while(file.available()){
    Serial.write(file.read());
  }
  file.close();

#pragma endregion SPIFFS

#pragma region WiFiManager
  WiFi.mode(WIFI_STA); // explicitly set mode, esp defaults to STA+AP
  //WiFiManager, Local intialization. Once its business is done, there is no need to keep it around

  // reset settings - wipe stored credentials for testing
  // these are stored by the esp library
  // wm.resetSettings();

  bool res;
  // res = wm.autoConnect(); // auto generated AP name from chipid
  // res = wm.autoConnect("AutoConnectAP"); // anonymous ap
  res = wm.autoConnect("super-cat"); // password protected ap

  if(!res) {
      Serial.println("Failed to connect");
      // ESP.restart();
  } 
  else {
      //if you get here you have connected to the WiFi    
      Serial.println("connected...yeey :)");
  }

#pragma endregion WiFiManager

#pragma region WebServer
  server.serveStatic("/statics/", SPIFFS, "/").setDefaultFile("index.html");

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(200, "text/plain", "Hello, world");
    });

    // Send a GET request to <IP>/get?message=<message>
    server.on("/get", HTTP_GET, [] (AsyncWebServerRequest *request) {
        String message;
        if (request->hasParam(PARAM_MESSAGE)) {
            message = request->getParam(PARAM_MESSAGE)->value();
        } else {
            message = "No message sent";
        }
        request->send(200, "text/plain", "Hello, GET: " + message);
    });

    // Send a POST request to <IP>/post with a form field message set to <message>
    server.on("/post", HTTP_POST, [](AsyncWebServerRequest *request){
        String message;
        if (request->hasParam(PARAM_MESSAGE, true)) {
            message = request->getParam(PARAM_MESSAGE, true)->value();
        } else {
            message = "No message sent";
        }
        request->send(200, "text/plain", "Hello, POST: " + message);
    });

    server.onNotFound(notFound);

    server.begin();
#pragma endregion WebServer

#pragma regiom FastLED
  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(255);
  FastLED.clear();
  leds[0] = CRGB::Red;
  leds[1] = CRGB::Red;
  leds[2] = CRGB::Red;
  FastLED.show();
#pragma endregion FastLED
}

void loop() {
  static int i = 0;
  // put your main code here, to run repeatedly:
  StaticJsonDocument<500> doc;
  auto data = doc.createNestedArray("data");
  auto obj = doc.createNestedObject("obj");
  obj["key"] = "value";
  data.add(123);
  data.add(321);
  Serial.println("loop");
  Serial.print(F("Sending: "));
  serializeJsonPretty(doc, Serial);
  Serial.println();
  i++;
  delay(1000);
}

void notFound(AsyncWebServerRequest *request) {
    request->send(404, "text/plain", "Not found");
}


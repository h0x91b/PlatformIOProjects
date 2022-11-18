#include <Arduino.h>
#include <SPIFFS.h>
#include <ArduinoOTA.h>
#include "globals.h"

#pragma region includes

#pragma region Wifi
  #include <WiFiManager.h>
  #include <ESPmDNS.h>
  
  WiFiManager wm;
#pragma endregion Wifi

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
  #include "AsyncJson.h"
  #include "ArduinoJson.h"

  void notFound(AsyncWebServerRequest *request);

  AsyncWebServer server(80);
  const char* PARAM_MESSAGE = "message";
#pragma endregion WebServer

#pragma region FastLED
#include "FastLED.h"
#define LED_TYPE   WS2812
#define COLOR_ORDER   GRB
CRGB leds[NUM_LEDS];
#pragma endregion FastLED

#include "rocketWithTrail.h"
#include "rainbowRocket.h"
#include "randomColorsRocket.h"

std::vector<Rocket*> rockets;
#pragma endregion includes

#pragma region declarations
void initFileSystem();
void initWifi();
void initLeds();
void initOTA();
void initServer();
void initMdns();
#pragma endregion declarations

void setup() {
  Serial.begin(115200);
  Serial.println("Init!");

  initLeds();
  initFileSystem();
  initWifi();
  initOTA();
  initServer();
  initMdns();

  randomSeed(micros());
}

void loop() {
  static int nextSpawn = 0;
  static unsigned int lastTick = millis();
  ArduinoOTA.handle();

  unsigned int dt = millis() - lastTick;
  lastTick = millis();

  FastLED.clear(false);
  // for(int i=0; i<NUM_LEDS; i++) {
  //   leds[i] = CRGB::Black;
  // }
  // leds[millis() / (1000/25) % NUM_LEDS] = CRGB::Cyan;

  // auto aVal = analogRead(35);
  // Serial.printf("aVal: %d\n", aVal);

  nextSpawn -= dt;
  if(nextSpawn < 0) {
    nextSpawn = random(7000, 15000);
    static unsigned int curColor=0;
    auto r = random8(0, 100);
    if(r < 20) {
      rockets.push_back(new RainbowRocket(
        leds, 
        0.5f + ((float)random16(0, 32000) / 32000.0) * 0.5f
      ));
    } else if(r<50) {
      rockets.push_back(new RandomColorsRocket(
        leds, 
        0.5f + ((float)random16(0, 32000) / 32000.0) * 0.5f
      ));
    } else {
      rockets.push_back(
        new RocketWithTrail(
          leds, 
          0.5f + ((float)random16(0, 32000) / 32000.0) * 0.5f,
          rainbow[++curColor % 7],
          random8(10, 25)
        )
      );
    }
  }

  for(int i=0; i<rockets.size(); i++) {
    rockets[i]->tick((float)dt / 1000.0);
    if(rockets[i]->isDead()) {
      delete rockets[i];
      rockets.erase(rockets.begin() + i);
    }
  }
  FastLED.show();
  // something around 140 fps, no need to overheat the CPU
  delay(7);
}

void initFileSystem() {
  if (!SPIFFS.begin(true)) {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  File file = SPIFFS.open("/foo.txt", "r");
  if (!file) {
    Serial.println("Failed to open file for reading");
    return;
  }

  Serial.println("File Content:");
  while (file.available()) {
    Serial.write(file.read());
  }
  file.close();
}

void initWifi() {
  WiFi.mode(WIFI_STA); // explicitly set mode, esp defaults to STA+AP
  //WiFiManager, Local intialization. Once its business is done, there is no need to keep it around

  // reset settings - wipe stored credentials for testing
  // these are stored by the esp library
  // wm.resetSettings();
  wm.setMinimumSignalQuality(5);

  bool res;
  res = wm.autoConnect("super-cat"); // password protected ap

  if(!res) {
      Serial.println("Failed to connect");
      // ESP.restart();
  } 
  else {
      Serial.println("connected... To Wifi");
  }
}

void initLeds() {
  FastLED.addLeds<LED_TYPE, LEDS_DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(255);
  FastLED.clear();
  FastLED.show();
}

void initOTA() {
  ArduinoOTA.setHostname("myesp32");
  ArduinoOTA.setPassword("annuta");

  ArduinoOTA
    .onStart([]() {
      String type;
      if (ArduinoOTA.getCommand() == U_FLASH)
        type = "sketch";
      else // U_SPIFFS
        type = "filesystem";

      // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
      Serial.println("Start updating " + type);
    })
    .onEnd([]() {
      Serial.println("\nEnd");
    })
    .onProgress([](unsigned int progress, unsigned int total) {
      Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    })
    .onError([](ota_error_t error) {
      Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
      else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
      else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
      else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
      else if (error == OTA_END_ERROR) Serial.println("End Failed");
    });

  ArduinoOTA.begin();

  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void initServer() {
  server.serveStatic("/statics/", SPIFFS, "/").setDefaultFile("index.html");

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    auto signal = WiFi.RSSI();
    auto level = wm.getRSSIasQuality(signal);
    request->send(200, "text/plain", "Signal strength: " + String(signal) + "dBm level: " + String(level) + "%");
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

  AsyncCallbackJsonWebHandler* handler = new AsyncCallbackJsonWebHandler("/spawn", [](AsyncWebServerRequest *request, JsonVariant &json) {
    const JsonObject& jsonObj = json.as<JsonObject>();
    int speed = jsonObj["speed"];
    int trail = jsonObj["trail"];
    int hue = jsonObj["hue"];
    int sat = jsonObj["sat"];
    int val = jsonObj["val"];
    rockets.push_back(
      new RocketWithTrail(
        leds, 
        speed, 
        CHSV(hue, sat, val),
        trail
      )
    );
    // String response = "Spawned rocket with speed " + String(speed) + " and trail " + String(trail) + " in " + String(millis() - start) + "ms";
    String response;
    serializeJson(jsonObj, response);
    request->send(200, "application/json", response);
  });
  server.addHandler(handler);

  server.onNotFound(notFound);

  server.begin();
}

void notFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Not found");
}

void initMdns()
{
  //initialize mDNS service
  esp_err_t err = mdns_init();
  if (err) {
    Serial.printf("MDNS Init failed: %d\n", err);
    return;
  }

  //set hostname
  mdns_hostname_set(DNS_HOST);
  //set default instance
  mdns_instance_name_set(DNS_INSTANCE);
}

// My libs
#include "header.h"
#include "LCD.h"
#include "WifiAndTime.h"

// Network libraries
#include <HTTPClient.h>
#include <ArduinoJson.h>

VixState vixState = {"0.00%", "0.00", "0.00", "0.00", "0.00", false, false};

// Site to get data
String tickerDataPath = "https://www.plus500.com/pl/api/LiveData/FeedUpdate?instrumentID=3468";

// Internal timer
unsigned long vixTimer = 0;
unsigned long vixTimerDelay = 30000;

void setup() {

  #ifdef DEBUG_MODE
  Serial.begin(115200);
  #endif

  setupLCD();
  while (!setupWifiAndTime()) delay(500);
  updateBacklightState(true);
}

void getVix(){

  if ((millis() - vixTimer) > vixTimerDelay) {
    if(WiFi.status()== WL_CONNECTED) {
      HTTPClient http;

      http.begin(tickerDataPath.c_str());
      http.addHeader("User-Agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/75.0.3770.142 Safari/537.36");
      
      int httpResponseCode = http.GET();

      #ifdef DEBUG_MODE
      DEBUG_PRINTLN(httpResponseCode);
      #endif
      
      if (httpResponseCode == 200) {
        String payload = http.getString();
        DEBUG_PRINTLN(payload);

        JsonDocument payloadJSON;
        DeserializationError error = deserializeJson(payloadJSON, payload);

        if (error) {
          http.end();
          return;
        }

        vixState.buyPrice  = String(payloadJSON["BuyPrice"].as<float>(), 2);
        vixState.sellPrice = String(payloadJSON["SellPrice"].as<float>(), 2);
        vixState.change    = String(payloadJSON["ChangePercent"].as<float>(), 2);
        vixState.highPrice = String(payloadJSON["HighPrice"].as<float>(), 2);
        vixState.lowPrice  = String(payloadJSON["LowPrice"].as<float>(), 2);

        updateLCD();
      }
      else {
        DEBUG_PRINT("Error code vix_get: ");
        DEBUG_PRINTLN(httpResponseCode);
      }

      // Free resources
      http.end();
    } else {
      WiFi.reconnect();
    }
    vixTimer = millis();
  }
}

void loop() {
  updateButtonState();
  updateBacklightState(false);
  if (vixState.backlightState){
    getVix();
  }
}
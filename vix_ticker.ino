/**************
PINOUT ESP32 -> ST7735
  GND -> GND
  VCC -> 3.3
  SCL -> D18 (SCLK)
  SDA -> D23 (MISO)
  RES -> D16
  DC  -> D15
  CS  -> D2
  BL  -> D12

PINOUT ESP32 -> BUTTON
  LEG1 -> GND
  LEG2 -> D13
**************/

// Network libraries
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

// Graphic libraries
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SPI.h>
#include <Fonts/FreeSansBold12pt7b.h>
#include <esp_sleep.h>

// Pin definition
#define TFT_CS 2
#define TFT_DC 15
#define TFT_RST 16

// WIFI credentials
const char* ssid = "<SSID>";
const char* password = "<PASSWORD>";

// VIX values variables
String change = "0.00%";
double change_double;
int change_index;
String sell_price = "0.00";
double sell_price_double;
int sell_price_index;
String buy_price = "0.00";
double buy_price_double;
int buy_price_index;
String high_price = "0.00";
int high_price_index;
String low_price = "0.00";
int low_price_index;

// Site to get data
String serverPath = "https://www.plus500.com/pl/instruments/vix/";
String timeApiPath = "https://timeapi.io/api/time/current/zone?timeZone=Europe%2FWarsaw";

// LCD intialization
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);
const int backlightPin = 12;
bool backlightState = 0;
const int buttonPin = 13;
const int longPressDuration = 1000;
unsigned long buttonPressStartTime = 0;
int16_t x, y;
uint16_t w, h;

// Internal timer
unsigned long lastTime = 0;
unsigned long lastTimeHour = 0;
unsigned long timerDelay = 30000;
unsigned long timerDelayHour = 1800000;
int currentHour = 8;
String currentDay = "Monday";

void setup() {
  // Serial.begin(115200);
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(backlightPin, OUTPUT);
  digitalWrite(backlightPin, LOW);

  setup_lcd();
  setup_wifi();
  get_time(true);

  if (
    currentHour > 6 
    and currentHour < 22
    and (currentDay == "Monday" || currentDay == "Tuesday" || currentDay == "Wednesday" || currentDay == "Thursday" || currentDay ==  "Friday")
  ) {
    if (backlightState == 0) {
      Serial.println("wlacz");
      digitalWrite(backlightPin, HIGH);
      backlightState = 1;
    }
  } else {
    if (backlightState == 1) {
      Serial.println("wylacz");
      digitalWrite(backlightPin, LOW);
      backlightState = 0;
    }
  }
}

void loop() {

  get_time(false);

  int reading = digitalRead(buttonPin);

  if (reading == LOW) {

    if (buttonPressStartTime == 0) {
      buttonPressStartTime = millis();
    }

    if (millis() - buttonPressStartTime >= longPressDuration) {
      delay(1000);
      if (backlightState == 1) {
        digitalWrite(backlightPin, LOW);
        backlightState = 0;
      } else {
        digitalWrite(backlightPin, HIGH);
        backlightState = 1;
      }
    }
  } else {
    buttonPressStartTime = 0;
  }

  if (
    currentHour > 6 
    and currentHour < 22
    and (currentDay == "Monday" || currentDay == "Tuesday" || currentDay == "Wednesday" || currentDay == "Thursday" || currentDay ==  "Friday")
  ) {
    if (backlightState == 0) {
      digitalWrite(backlightPin, HIGH);
      backlightState = 1;
    }
  } else {
    if (backlightState == 1) {
      digitalWrite(backlightPin, LOW);
      backlightState = 0;
    }
  }

  if (backlightState == 1){
    get_vix();
  }
}

void setup_wifi(){
  WiFi.begin(ssid, password);
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
}

void setup_lcd(){ 
  tft.initR(INITR_BLACKTAB);
  tft.fillScreen(ST77XX_BLACK);

  //Header
  tft.setTextColor(ST77XX_WHITE);
  tft.setFont(&FreeSansBold12pt7b);
  tft.getTextBounds("VIX", 0, 0, &x, &y, &w, &h);
  tft.setCursor((tft.width() - w) / 2, 25);
  tft.print("VIX");

  //Initialize
  tft.setCursor(3, 140);
  tft.print("Initializing");
}

void get_vix(){
  if ((millis() - lastTime) > timerDelay) {
    if(WiFi.status()== WL_CONNECTED) {
      HTTPClient http;

      http.begin(serverPath.c_str());
      http.addHeader("User-Agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/75.0.3770.142 Safari/537.36");
      
      int httpResponseCode = http.GET();

      // Serial.println(httpResponseCode);
      
      if (httpResponseCode == 200) {
        String payload = http.getString();

        //Scrape buy price
        buy_price_index = payload.indexOf("buyPrice");
        buy_price = payload.substring(buy_price_index, buy_price_index+20);
        buy_price = buy_price.substring(buy_price.indexOf(">")+1, buy_price.indexOf("<"));

        // Scrape sell price
        sell_price_index = payload.indexOf("sellPrice");
        sell_price = payload.substring(sell_price_index, sell_price_index+20);
        sell_price = sell_price.substring(sell_price.indexOf(">")+1, sell_price.indexOf("<"));

        //Scrape daily change
        change_index = payload.indexOf("changePercentText");
        change = payload.substring(change_index, change_index+40);
        change = change.substring(change.indexOf(">")+1, change.indexOf("<"));

        //Scrape high price
        high_price_index = payload.indexOf("highPrice");
        high_price = payload.substring(high_price_index, high_price_index+40);
        high_price = high_price.substring(high_price.indexOf(">")+1, high_price.indexOf("<"));

        //Scrape low price
        low_price_index = payload.indexOf("lowPrice");
        low_price = payload.substring(low_price_index, low_price_index+40);
        low_price = low_price.substring(low_price.indexOf(">")+1, low_price.indexOf("<"));
      }
      else {
        // Serial.print("Error code vix_get: ");
        // Serial.println(httpResponseCode);
      }

      // Free resources
      http.end();

      // Check if there is no garbage text
      if (buy_price.length() > 5 || sell_price.length() > 5) {
        return;
      } else {
        //Buy banner
        tft.drawRect(0, 35, 54, 40, ST77XX_GREEN);
        tft.fillRect(0, 35, 54, 40, ST77XX_GREEN);
        tft.drawTriangle(54, 35, 54, 74, 93, 35, ST77XX_GREEN);
        tft.fillTriangle(54, 35, 54, 74, 93, 35, ST77XX_GREEN);

        //Sell banner
        tft.drawRect(74, 70, 54, 40, ST77XX_RED);
        tft.fillRect(74, 70, 54, 40, ST77XX_RED);
        tft.drawTriangle(73, 70, 73, 109, 34, 109, ST77XX_RED);
        tft.fillTriangle(73, 70, 73, 109, 34, 109, ST77XX_RED);

        tft.drawRect(0, 115, 128, 45, ST77XX_BLACK);
        tft.fillRect(0, 115, 128, 45, ST77XX_BLACK);     

        tft.setFont(&FreeSansBold12pt7b);

        //Buy price
        tft.getTextBounds(buy_price.c_str(), 0, 0, &x, &y, &w, &h);
        tft.setCursor((tft.width()/2 - w) / 2, 35+h+((40-h)/2));
        tft.print(buy_price.c_str());

        //Sell price
        tft.getTextBounds(sell_price.c_str(), 0, 0, &x, &y, &w, &h);
        tft.setCursor(tft.width()/2 + (tft.width()/2 - w)/2-3, 109-(40-h)/2);
        tft.print(sell_price.c_str());

        //Change
        if (change.indexOf("-") == 0) {
          tft.setTextColor(ST77XX_RED);
        } else if (change == "0.00%") {
          tft.setTextColor(ST77XX_WHITE);
        } else {
          tft.setTextColor(ST77XX_GREEN);
        }
        tft.getTextBounds(change.c_str(), 0, 0, &x, &y, &w, &h);
        tft.setCursor((tft.width() - w) / 2, 135);
        tft.print(change.c_str());
        tft.setTextColor(ST77XX_WHITE);

        tft.setFont();

        //High text
        tft.setFont();
        tft.setCursor(3, 140);
        tft.print("HIGH");

        //Low text
        tft.setCursor(108, 140);
        tft.print("LOW");  

        //High Value
        tft.setCursor(2, 150);
        tft.print(high_price.c_str());

        //Low Value
        tft.getTextBounds(low_price.c_str(), 0, 0, &x, &y, &w, &h);
        tft.setCursor(tft.width()-w-1, 150);
        tft.print(low_price.c_str());
      }
    } else {
      WiFi.reconnect();
    }
    lastTime = millis();
  }
}

void get_time(bool force){
  if (((millis() - lastTimeHour) > timerDelayHour) || force == true) {
      if(WiFi.status()== WL_CONNECTED) {
        HTTPClient http;

        http.begin(timeApiPath.c_str());
        http.addHeader("User-Agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/75.0.3770.142 Safari/537.36");
        
        int httpResponseCode = http.GET();

        // Serial.println(httpResponseCode);
        
        if (httpResponseCode == 200) {
          String payload = http.getString();

          StaticJsonDocument<200> payloadJSON;
          DeserializationError error = deserializeJson(payloadJSON, payload);

          if (error) {
            return;
          }

          currentHour = payloadJSON["hour"];
          const char* day = payloadJSON["dayOfWeek"];
          currentDay = day;
      }
    }
    lastTimeHour = millis();
  }
}

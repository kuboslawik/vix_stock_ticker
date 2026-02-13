#include "LCD.h"
#include "header.h"

// Graphic libraries
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <Fonts/FreeSansBold12pt7b.h>

// LCD Pin definition
#define TFT_CS 2
#define TFT_DC 15
#define TFT_RST 16

// LCD intialization
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);
const int backlightPin = 12;
const int backlightBrightness = map(30, 0, 100, 0 , 255);
const int buttonPin = 13;
const int longPressDuration = 1000;
unsigned long buttonPressStartTime = 0;
int16_t x, y;
uint16_t w, h;

unsigned long backlightStateTimer = 1800001;
unsigned long backlightStateTimerDelay = 1800000;

void setupLCD() {
    pinMode(buttonPin, INPUT_PULLUP);
    pinMode(backlightPin, OUTPUT);
    digitalWrite(backlightPin, LOW);

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

void updateLCD(){
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
    tft.getTextBounds(vixState.buyPrice.c_str(), 0, 0, &x, &y, &w, &h);
    tft.setCursor((tft.width()/2 - w) / 2, 35+h+((40-h)/2));
    tft.print(vixState.buyPrice.c_str());

    //Sell price
    tft.getTextBounds(vixState.sellPrice.c_str(), 0, 0, &x, &y, &w, &h);
    tft.setCursor(tft.width()/2 + (tft.width()/2 - w)/2-3, 109-(40-h)/2);
    tft.print(vixState.sellPrice.c_str());

    //Change
    if (vixState.change.indexOf("-") == 0) {
      tft.setTextColor(ST77XX_RED);
    } else if (vixState.change == "0.00%") {
      tft.setTextColor(ST77XX_WHITE);
    } else {
      tft.setTextColor(ST77XX_GREEN);
    }
    tft.getTextBounds(vixState.change.c_str(), 0, 0, &x, &y, &w, &h);
    tft.setCursor((tft.width() - w) / 2, 135);
    tft.print(vixState.change.c_str());
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
    tft.print(vixState.highPrice.c_str());

    //Low Value
    tft.getTextBounds(vixState.lowPrice.c_str(), 0, 0, &x, &y, &w, &h);
    tft.setCursor(tft.width()-w-1, 150);
    tft.print(vixState.lowPrice.c_str());
}

void updateBacklightState() {
  if ((millis() - backlightStateTimer) > backlightStateTimerDelay) {
    getLocalTime(&timeContainer);
    if (
        timeContainer.tm_hour > 6 && timeContainer.tm_hour < 22 &&
        (timeContainer.tm_wday == 1 || timeContainer.tm_wday == 2 || timeContainer.tm_wday == 3 || timeContainer.tm_wday == 4 || timeContainer.tm_wday == 5)
    ) {
        if (vixState.backlightState == false) {
        analogWrite(backlightPin, backlightBrightness);
        vixState.backlightState = true;
        }
    } else {
        if (vixState.backlightState == true) {
        digitalWrite(backlightPin, LOW);
        vixState.backlightState = false;
        }
    }
    backlightStateTimer = millis();
  }
}

void updateButtonState() {
  bool isPressed = (digitalRead(buttonPin) == LOW);
  static bool longPressHandled = false;
  static unsigned long pressStartTime = 0;

  if (isPressed) {
    if (pressStartTime == 0) { 
      pressStartTime = millis();
    }
    if (millis() - pressStartTime >= longPressDuration && !longPressHandled) {
      vixState.backlightState = !vixState.backlightState;
      analogWrite(backlightPin, vixState.backlightState ? backlightBrightness : 0);
      longPressHandled = true;
    }
  } else {
    if (pressStartTime > 0 && !longPressHandled) {
      // Short press action can be added here if needed in the future
    }
    pressStartTime = 0;
    longPressHandled = false;
  }
}
#ifndef HEADER_H
#define HEADER_H

#include <Arduino.h>

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

// Debug functions
#ifdef DEBUG_MODE
  #define DEBUG_PRINT(x)     { Serial.print("["); Serial.print(millis()); Serial.print(" ms] "); Serial.print(x); }
  #define DEBUG_PRINTLN(x)   { Serial.print("["); Serial.print(millis()); Serial.print(" ms] "); Serial.println(x); }
  #define DEBUG_PRINTF(...)  { Serial.print("["); Serial.print(millis()); Serial.print(" ms] "); Serial.printf(__VA_ARGS__); }
#else
  #define DEBUG_PRINT(x)
  #define DEBUG_PRINTLN(x)
  #define DEBUG_PRINTF(...)
#endif

struct VixState {
    String buyPrice;
    String sellPrice;
    String change;
    String highPrice;
    String lowPrice;
    bool backlightState;
};

extern VixState vixState;

extern struct tm timeContainer;

#endif
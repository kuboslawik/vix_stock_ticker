#ifndef LCD_H
#define LCD_H

extern bool backlightState;

void setupLCD();
void updateLCD();
void updateBacklightState(bool);
void updateButtonState();

#endif
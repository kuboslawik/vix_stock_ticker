# VIX ESP32 Ticker
ESP32 based VIX ticker with LCD display. Supports automatic screen off based on time and day.
![VIX ESP32 Ticker](assets/vix_ticker_photo.jpg)

# Requirements
 - ESP-32S WROOM-32 WiFi
 - TFT LCD 1.8 ST7735 Display
 - 3D printer to print case
 - Momentary Pushbutton Switch - 12mm Square

# Wiring

Wiring between  ESP32 and ST7735
 - GND -> GND
 - VCC -> 3.3
 - SCL -> D18 (SCLK)
 - SDA -> D23 (MISO)
 - RES -> D16
 - DC  -> D15
 - CS  -> D2
 - BL  -> D12

Wiring between ESP32 and button
 - LEG1 -> GND
 - LEG2 -> D13

USB-C is used for power delivery.

# Code

Providing any suitable API path in <code>tickerDataPath</code> variable, chosen ticker is displayed on display in intervals specified in <code>vixTimerDelay</code> variable. Connecting to your WiFi is handled via WiFiManager library, that creates anonymous vixStockTickerAP access point to which you connect and configure your credentials. Push button for 1 second do disable LCD screen without powering off device. Edit sixth argument of <code>vixState</code> variable to set backlight intensity between 0 and 100 percent, default is set to 10.

# STL Model

Case consists of two parts, main case and back panel. Switch is pressfitted into back panel. Gap over the screen cut-out can be hard to print without sagging, it is advised to print this part with supports enabled.
![stl_part_front](assets/stl_image_front.png)
![stl_part_back](assets/stl_image_back.png)

#include <WiFi.h>
#include <time.h>
#include "header.h"
#include "WifiAndTime.h"
#include "credential.h"

static bool setupTime() {
    DEBUG_PRINTLN("Time sync...");
    configTzTime("CET-1CEST,M3.5.0,M10.5.0", "pool.ntp.org", "time.google.com");

    struct tm timeinfo;
    int attemps = 0;
    const int maxAttemps = 10;

    while (!getLocalTime(&timeinfo) && attemps < maxAttemps) {
        attemps++;
        delay(1000);
        DEBUG_PRINT(".");
    }

    if (attemps < maxAttemps) {
        DEBUG_PRINTLN("");
        DEBUG_PRINTLN("Time synchronized");
        return true;
    } else {
        DEBUG_PRINTLN("");
        DEBUG_PRINTLN("ERROR: Unable to get time (Timeout).");
        return false;
    }
}

static bool setupWiFi() {
    DEBUG_PRINTLN("Connecting to WiFi...");
    WiFi.mode(WIFI_STA); 
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    int attemps = 0;
    const int maxAttemps = 10;

    while (WiFi.status() != WL_CONNECTED && attemps < maxAttemps) {
        attemps++;
        delay(1000);
        DEBUG_PRINT(".");
    }

    if (WiFi.status() == WL_CONNECTED) {
        DEBUG_PRINTLN("");
        DEBUG_PRINT("Connected! IP: ");
        DEBUG_PRINTLN(WiFi.localIP());
        
        return true; 
    } else {
        DEBUG_PRINTLN("");
        DEBUG_PRINTLN("ERROR: Couldn't connect - offline mode");
        WiFi.disconnect(true);
        WiFi.mode(WIFI_OFF);
        
        return false;
    }
}

bool setupWifiAndTime() {
    return setupWiFi() && setupTime();
}
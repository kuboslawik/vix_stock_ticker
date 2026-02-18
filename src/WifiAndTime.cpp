#include "header.h"
#include "WifiAndTime.h"

// Network libraries
#include <WiFi.h>
#include <WiFiManager.h>
#include <time.h>


static bool setupTime() {
    DEBUG_PRINTLN("Time sync...");
    configTzTime("CET-1CEST,M3.5.0,M10.5.0", "pool.ntp.org", "time.google.com");

    struct tm timeinfo;

    while (!getLocalTime(&timeinfo)) {
        delay(1000);
        DEBUG_PRINT(".");
    }
    
    DEBUG_PRINTLN("Time synchronized");

    return true;
}

static bool setupWiFi() {
    DEBUG_PRINTLN("Connecting to WiFi...");
    WiFi.mode(WIFI_STA); 

    WiFiManager wm;
    wm.setConnectTimeout(60); 
    wm.setConfigPortalTimeout(180); 

    if (!wm.autoConnect("vixStockTickerAP")) {
        DEBUG_PRINTLN("Portal timeout, restarting device");
        ESP.restart();
    }

    DEBUG_PRINTLN("");
    DEBUG_PRINT("Connected! IP: ");
    DEBUG_PRINTLN(WiFi.localIP());
        
    return true;
}

bool setupWifiAndTime() {
    return setupWiFi() && setupTime();
}

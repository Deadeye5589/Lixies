#ifndef WIFI_FUNCTIONS_H
#define WIFI_FUNCTIONS_H
    #include <ESP8266WiFi.h>
    #include <ESP8266WebServer.h>

    // Wifi credentials for access point that is opened if the above ap cannot be connected
    extern const char own_ap_ssid[];  
    extern const char own_ap_pass[];

    extern const IPAddress local_IP;
    extern const IPAddress gateway;
    extern const IPAddress subnet;  

    void initSoftAP();
#endif
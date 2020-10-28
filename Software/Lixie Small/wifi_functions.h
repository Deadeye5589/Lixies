#ifndef WIFI_FUNCTIONS_H
#define WIFI_FUNCTIONS_H
    #include <ESP8266WiFi.h>
    #include <ESP8266WebServer.h>

    // Wifi credentials for access point that is opened if the above ap cannot be connected
    extern char own_ap_ssid[];  
    extern char own_ap_pass[];

    extern IPAddress local_IP;
    extern IPAddress gateway;
    extern IPAddress subnet;  

    void initSoftAP();
#endif
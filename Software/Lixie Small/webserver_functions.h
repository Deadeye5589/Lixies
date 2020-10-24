#ifndef WEBSERVER_FUNCTIONS_H
#define WEBSERVER_FUNCTIONS_H
    #include <ESP8266WiFi.h>
    #include <ESP8266WebServer.h>
    // #include "rtc_functions.h"

    // Set port of web serve to 80
    extern ESP8266WebServer server;

    void printWebsiteToClient();
    void handleForm();
    void initWebserver();
#endif
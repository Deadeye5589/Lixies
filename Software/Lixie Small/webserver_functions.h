#ifndef WEBSERVER_FUNCTIONS_H
#define WEBSERVER_FUNCTIONS_H

    #include <ESP8266WiFi.h>
    #include <ESP8266WebServer.h>
    // #include "rtc_functions.h"

    extern uint32_t zeit;
    extern uint_fast8_t tag;
    extern uint_fast8_t monat;
    extern uint_fast16_t jahr;
    extern volatile uint_fast8_t sekunde;
    extern uint_fast8_t minu;
    extern uint_fast8_t stunde;
    extern volatile bool updateclock;

    extern void setRTC(uint16_t jahr,uint8_t monat,uint8_t tag,uint8_t stunde,uint8_t minu,uint8_t sekunde);

    // Set port of web serve to 80
    extern ESP8266WebServer server;

    void printWebsiteToClient();
    void handleForm();
    void initWebserver();

#endif
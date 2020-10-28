#ifndef NTP_FUNCTIONS_H
#define NTP_FUNCTIONS_H
    #include "time_ntp.h"
    #include <ESP8266WiFi.h>
    #include <WiFiUdp.h>    // Built-in Arduino
    #include <TimeLib.h>    // Time   -> Michael Margolis
    #include <Timezone.h>   // Timezone  -> Jack Christensen
    #include <WiFiClient.h>
    #include <ESP8266WebServer.h>


    extern uint32_t zeit;
    extern uint_fast8_t tag;
    extern uint_fast8_t monat;
    extern uint_fast16_t jahr;
    extern volatile uint_fast8_t sekunde;
    extern uint_fast8_t minu;
    extern uint_fast8_t stunde;

    extern Timezone CE;
    extern bool bNTPStarted;
    
    void getNTPTime();
    time_t getNTP_UTCTime1970();

#endif
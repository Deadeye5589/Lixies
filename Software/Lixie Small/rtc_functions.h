#ifndef RTC_FUNCTIONS_H
#define RTC_FUNCTIONS_H
    // #include <ESP8266WiFi.h>
    #include <RtcDS3231.h>
    #include <Wire.h>       // Built-in Arduino
    // #include <cstdint>
    // using namespace std;
    // Set port of web serve to 80

    void setRTC(uint16_t jahr,uint8_t monat,uint8_t tag,uint8_t stunde,uint8_t minu,uint8_t sekunde);
    void startRTC();
    void initRTC();
    RtcDateTime getTimeFromMemory();

#endif
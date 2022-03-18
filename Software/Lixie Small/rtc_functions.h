#ifndef RTC_FUNCTIONS_H
#define RTC_FUNCTIONS_H

    #include <RtcDS3231.h>
    #include <Wire.h>       // Built-in Arduino
    // #include <cstdint>
    // using namespace std;

    extern volatile bool marker;
    extern volatile bool updateclock;
    extern volatile bool updatecolors;

    // Set port of web serve to 80
    extern uint32_t zeit;
    extern uint_fast8_t tag;
    extern uint_fast8_t monat;
    extern uint_fast16_t jahr;
    extern volatile uint_fast8_t sekunde;
    extern uint_fast8_t minu;
    extern uint_fast8_t stunde;

    void setRTC(uint16_t jahr,uint8_t monat,uint8_t tag,uint8_t stunde,uint8_t minu,uint8_t sekunde);
    void connectRTC();
    void initRTC();

    RtcDateTime getTimeFromMemory();
    void ISR_ATTR SecondsTick();
#endif

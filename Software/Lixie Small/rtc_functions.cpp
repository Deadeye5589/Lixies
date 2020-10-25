#include "rtc_functions.h"

// Create RTC object for HW I2C
static RtcDS3231<TwoWire> rtcObject(Wire);

void setRTC(uint16_t jahr,uint8_t monat,uint8_t tag,uint8_t stunde,uint8_t minu,uint8_t sekunde){
    RtcDateTime currentTime = RtcDateTime(jahr,monat,tag,stunde,minu,sekunde); //define date and time object
    rtcObject.SetDateTime(currentTime);
}

void startRTC(){
    rtcObject.Begin(); // connect to RTC
}

void initRTC(){
  rtcObject.Enable32kHzPin(false);
  rtcObject.SetSquareWavePin(DS3231SquareWavePin_ModeClock);
  rtcObject.SetSquareWavePinClockFrequency(DS3231SquareWaveClock_1Hz);
  rtcObject.SetIsRunning(true);
}

RtcDateTime getTimeFromMemory(){
    return rtcObject.GetDateTime();
}
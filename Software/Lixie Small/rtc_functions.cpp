#include "rtc_functions.h"


// Create RTC object for HW I2C
 static RtcDS3231<TwoWire> rtcObject(Wire);

void setRTC(uint16_t jahr,uint8_t monat,uint8_t tag,uint8_t stunde,uint8_t minu,uint8_t sekunde){
    RtcDateTime currentTime = RtcDateTime(jahr,monat,tag,stunde,minu,sekunde); //define date and time object
    zeit = currentTime.Hour() * 100 + currentTime.Minute();
    rtcObject.SetDateTime(currentTime);
    updateclock = 1;
}

void connectRTC(){
    rtcObject.Begin(); // connect to RTC
}

void initRTC(){
  rtcObject.Enable32kHzPin(false);
  rtcObject.SetSquareWavePin(DS3231SquareWavePin_ModeClock);
  rtcObject.SetSquareWavePinClockFrequency(DS3231SquareWaveClock_1Hz);
  rtcObject.SetIsRunning(true);
}

RtcDateTime getTimeFromMemory(){
    Serial.println("No Wifi, fetching last saved time from RTC");

    RtcDateTime currentTime = rtcObject.GetDateTime();
    zeit = currentTime.Hour() * 100 + currentTime.Minute();
    sekunde = currentTime.Second();
    Serial.println(zeit);
}

// ISR triggered each second by RTC
// Activate main loop to update clock each minute
// Also helps to toggle :
void ISR_ATTR SecondsTick(){
  Serial.println(sekunde);
  sekunde++;
  if(sekunde > 59){
    sekunde = 0;
    updateclock = 1;
  }
  if(marker == 1){
    marker = 0;
  }
  else{
    marker = 1; 
  }
}
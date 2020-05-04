/*
   ____ ___  ____ _  _     ____ ____ ____ _ ____ _        _  _ ____ _  _ _ ___ ____ ____   /
   |  | |__] |___ |\ |     [__  |___ |__/ | |__| |        |\/| |  | |\ | |  |  |  | |__/  / 
   |__| |    |___ | \|     ___] |___ |  \ | |  | |___     |  | |__| | \| |  |  |__| |  \ .  

  (115200 Baud)
*/                                                                                                                

#include "Lixiesbig.h" // Include Lixie Library
#include "time_ntp.h"
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <TimeLib.h>
#include <Timezone.h>
#include <Wire.h> 
#include <RtcDS3231.h>


// Wifi credentials
char ssid[] = "Change to your SSID";  
char pass[] = "Change to your password";  

// Configure Lixie
#define DATA_PIN   0 // Pin to drive Lixies
#define NUM_LIXIES 7  // How many Lixies you have
Lixie lix(DATA_PIN, NUM_LIXIES);

// Define RTC interrupt pin
#define RTCsecondInterrupt 13

// Create RTC object for HW I2C
RtcDS3231<TwoWire> rtcObject(Wire);

// Variables
volatile bool marker = 1;
volatile int sekunde = 0;
volatile bool updateclock = 0;
uint32_t zeit = 0;


// ntp flag
bool bNTPStarted=false;

//////////////////////////////////////////
// for time conversion
//Central European Time (Frankfurt, Paris)
//////////////////////////////////////////
TimeChangeRule CEST = {"CEST", Last, Sun, Mar, 2, 120};     //Central European Summer Time
TimeChangeRule CET = {"CET ", Last, Sun, Oct, 3, 60};       //Central European Standard Time
Timezone CE(CEST, CET);


///////////////////////////////////////////
// get UTC time referenced to 1970 by NTP
///////////////////////////////////////////
time_t getNTP_UTCTime1970() 
{ 
  bNTPStarted=false;  // invalidate; time-lib functions crash, if not initalized poperly
  unsigned long t = getNTPTimestamp();
  if (t==0) return(0);

  // scale to 1970 
  // may look like back & forth with ntp code; wrote it to make needed conversions more clear
  return(t+946684800UL);
}


// Greet the user.
void title(){
  Serial.println(F("----------------------------------"));
  Serial.println(F(" Lixie Clock                      "));
  Serial.println(F(" by Christopher                   "));
  Serial.println(F(" For RTCUSTOMZ of B.I.E.R.        "));
  Serial.println();
  Serial.println(F(" V0.1                             "));
  Serial.println(F("----------------------------------"));
}


// Writes zeroes to all llxie displays.
void fill_zero(){
  lix.write(pow(10,(NUM_LIXIES))); // gets number big enough for NUM_LIXIES
}

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
  zeit++;
}



// Setup Routine
void setup() {
  Serial.begin(115200);

  title();

  WiFi.begin(ssid, pass);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting...");
  }

  rtcObject.Begin(); // conneect to RTC

  lix.begin(); // Initialize LEDs
  lix.brightness(255); // 0-255
  lix.white_balance(Tungsten100W); // Default
    // Can be: Tungsten40W, Tungsten100W,  
    //         Halogen,     CarbonArc,
    //         HighNoonSun, DirectSunlight,
    //         OvercastSky, ClearBlueSky  
    // 2,600K - 20,000K
  fill_zero();

  if (timeStatus()==timeNotSet)
    {
      bNTPStarted=false;
      Serial.println("Setup sync with NTP service.");
      setSyncProvider(getNTP_UTCTime1970);
      setSyncInterval(86400); // NTP re-sync; i.e. 86400 sec would be once per day
    }
  else
    {
      bNTPStarted=true;   // only position, where we set this flag to true
    }
  yield();

  time_t tT = now();
  time_t tTlocal = CE.toLocal(tT);

  int tag = day(tTlocal);
  int monat = month(tTlocal);
  int jahr = year(tTlocal);
  sekunde = second(tTlocal);
  int minu = minute(tTlocal);
  int stunde = hour(tTlocal);
  Serial.println(tag);
  Serial.println(monat);
  Serial.println(jahr);
  Serial.println(stunde);
  Serial.println(minu);
  Serial.println(sekunde);

  yield();

  RtcDateTime currentTime = RtcDateTime(jahr,monat,tag,stunde,minu,sekunde); //define date and time object
  rtcObject.SetDateTime(currentTime);
  rtcObject.Enable32kHzPin(false);
  rtcObject.SetSquareWavePin(DS3231SquareWavePin_ModeClock);
  rtcObject.SetSquareWavePinClockFrequency(DS3231SquareWaveClock_1Hz);
  rtcObject.SetIsRunning(true);

  zeit = (stunde * 10000) + (minu * 100) + sekunde;

  yield();
  
  // Intro
  //fill_zero();
  lix.nixie_mode(true); // Activate Nixie Mode
  lix.nixie_aura_intensity(8); // Default aural intensity is 8 (0-255)

  //lix.color(CHSV(0,255,255));
  //lix.color_off(CHSV(127,255,127));

  lix.write(zeit);

  // setup external interupt 
  attachInterrupt(digitalPinToInterrupt(RTCsecondInterrupt), SecondsTick, FALLING);
}

void loop() {
    if(marker == 1)
      lix.write_fade(zeit+3000000);
    else
      lix.write_fade(zeit+6000000);
    if(updateclock){
      updateclock = 0;
      RtcDateTime currentTime = rtcObject.GetDateTime();
      zeit = currentTime.Hour() * 10000 + currentTime.Minute() *100 + currentTime.Second();
      Serial.println(zeit);
    }
}

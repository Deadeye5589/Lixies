
/*
   ____ ___  ____ _  _     ____ ____ ____ _ ____ _        _  _ ____ _  _ _ ___ ____ ____   /
   |  | |__] |___ |\ |     [__  |___ |__/ | |__| |        |\/| |  | |\ | |  |  |  | |__/  / 
   |__| |    |___ | \|     ___] |___ |  \ | |  | |___     |  | |__| | \| |  |  |__| |  \ .  

  (115200 Baud)
*/                                                                                                                

#define LIXIE_IS_SMALL
#ifdef LIXIE_IS_SMALL
	#define SMALL_LIXIE
#else
	#define BIG_LIXIE
#endif

#include "Lixiesmall.h" // Include Lixie Library
#include "time_ntp.h"
#include "webserver_functions.h"
#include "rtc_functions.h"
#include "wifi_functions.h"
#include "ntp_functions.h"
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>    // Built-in Arduino
#include <TimeLib.h>    // Time   -> Michael Margolis
#include <Timezone.h>   // Timezone  -> Jack Christensen
#include <WiFiClient.h>
#include <ESP8266WebServer.h>


// Wifi credentials
char ssid[] = "your_ssid";  
char pass[] = "your_password";  

// Wifi credentials for access point that is opened if the above ap cannot be connected
char own_ap_ssid[] = "Lixie";  
char own_ap_pass[] = "happylixie";  

// Wifi configuration
IPAddress local_IP(192,168,4,22);
IPAddress gateway(192,168,4,9);
IPAddress subnet(255,255,255,0);

// Set port of web serve to 80
ESP8266WebServer server(80);

// Variable for the HTTP Request
String header;


// Configure Lixie
#define DATA_PIN   2 // Pin to drive Lixies
#define NUM_LIXIES 5  // How many Lixies you have
Lixie_II lix(DATA_PIN, NUM_LIXIES);

// Define RTC interrupt pin
#define RTCsecondInterrupt 13

// Variables
volatile bool marker = 1;
volatile bool updateclock = 0;
bool wifi_connected = 0;

// Timekeeping variables
uint32_t zeit = 0;
uint_fast8_t tag = 0;
uint_fast8_t monat = 0;
uint_fast16_t jahr = 0;
volatile uint_fast8_t sekunde = 0;
uint_fast8_t minu = 0;
uint_fast8_t stunde = 0;


// ntp flag
bool bNTPStarted=false;

//////////////////////////////////////////
// for time conversion
//Central European Time (Frankfurt, Paris)
//////////////////////////////////////////
TimeChangeRule CEST = {"CEST", Last, Sun, Mar, 2, 120};     //Central European Summer Time
TimeChangeRule CET = {"CET ", Last, Sun, Oct, 3, 60};       //Central European Standard Time
Timezone CE(CEST, CET);

// Greet the user.
void title(){
  Serial.println(F("----------------------------------"));
  Serial.println(F(" Lixie Clock                      "));
  Serial.println(F(" by Christopher and Pia           "));
  Serial.println(F(" For RTCUSTOMZ of B.I.E.R.        "));
  Serial.println();
  Serial.println(F(" V0.2                             "));
  Serial.println(F("----------------------------------"));
}


// Writes zeroes to all llxie displays.
void fill_zero(){
  lix.write(pow(10,(NUM_LIXIES))); // gets number big enough for NUM_LIXIES
}

// Setup Routine
void setup() {
  Serial.begin(115200);
  title();
  lix.begin(); // Initialize LEDs
  lix.brightness(1.0); // 0-255
  WiFi.begin(ssid, pass); // Try to connect to the WLAN
  
  for (int cnt = 0; cnt < 10; cnt++)                                          // check 15 seconds if connected
  {
      lix.sweep_color(CRGB(0,0,255), 20, 5);                                   //Sweep idle
      if (WiFi.status() == WL_CONNECTED)
      {
          wifi_connected = true;
          break;
      } 
      Serial.print(".");
      delay(500);
  }

  Serial.println("");
  connectRTC(); // Connect to the RTC

  
  if(wifi_connected){			// If we have Wifi, we will get the current time from the NTP

    getNTPTime();
    yield();
    setRTC(jahr,monat,tag,stunde,minu,sekunde);
    zeit = (stunde * 100) + minu;
  } 
   
  else{ 				// Otherwise we load the time stored in the RTC and hope that it was keeping track
    
    getTimeFromMemory();
    initSoftAP();
    initWebserver();      
  }

  
  initRTC();		// Set the RTC to give as an interrupt each second
  

  
  yield();			// feed the watchdogs

  // Start the lixie and fill display with zeros
  // Then display the current time
  lix.begin(); // Initialize LEDs
  lix.brightness(1.0); // 0-255
  lix.white_balance(Tungsten100W); // Default
    // Can be: Tungsten40W, Tungsten100W,  
    //         Halogen,     CarbonArc,
    //         HighNoonSun, DirectSunlight,
    //         OvercastSky, ClearBlueSky  
    // 2,600K - 20,000K
  fill_zero();

  lix.nixie_mode(true); // Activate Nixie Mode
  lix.nixie_aura_intensity(8); // Default aural intensity is 8 (0-255)

  lix.write(zeit);

  // setup external interupt and now the main loop will take over the clock display
  attachInterrupt(digitalPinToInterrupt(RTCsecondInterrupt), SecondsTick, FALLING);
}




void loop() {
    server.handleClient();
    if(marker == 1)
      lix.write_fade(zeit+30000);
    else
      lix.write_fade(zeit+60000);
    if(updateclock){
      updateclock = 0;
      getTimeFromMemory();
    }
}

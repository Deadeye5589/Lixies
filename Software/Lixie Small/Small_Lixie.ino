
/*
   ____ ___  ____ _  _     ____ ____ ____ _ ____ _        _  _ ____ _  _ _ ___ ____ ____   /
   |  | |__] |___ |\ |     [__  |___ |__/ | |__| |        |\/| |  | |\ | |  |  |  | |__/  / 
   |__| |    |___ | \|     ___] |___ |  \ | |  | |___     |  | |__| | \| |  |  |__| |  \ .  

  (115200 Baud)
*/                                                                                                                

#include "Lixiesmall.h" // Include Lixie Library
#include "time_ntp.h"
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>    // Built-in Arduino
#include <TimeLib.h>    // Time   -> Michael Margolis
#include <Timezone.h>   // Timezone  -> Jack Christensen
#include <Wire.h>       // Built-in Arduino
#include <RtcDS3231.h>
// #include <DS3231.h>  // DS3231 -> Andrew Wickert


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
WiFiServer server(80);

// Variable for the HTTP Request
String header;


// Configure Lixie
#define DATA_PIN   2 // Pin to drive Lixies
#define NUM_LIXIES 5  // How many Lixies you have
Lixie_II lix(DATA_PIN, NUM_LIXIES);

// Define RTC interrupt pin
#define RTCsecondInterrupt 13

// Create RTC object for HW I2C
RtcDS3231<TwoWire> rtcObject(Wire);

// Variables
volatile bool marker = 1;
volatile bool updateclock = 0;
bool wifi_connected = 0;

// Timekeeping variables
uint32_t zeit = 0;
int tag = 0;
int monat = 0;
int jahr = 0;
volatile int sekunde = 0;
int minu = 0;
int stunde = 0;


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



// Setup Routine
void setup() {
  Serial.begin(115200);

  title();

  lix.begin(); // Initialize LEDs
  lix.brightness(1.0); // 0-255

  // Try to connect to the WLAN
  WiFi.begin(ssid, pass);
  
  for (int cnt = 0; cnt < 10; cnt++)                                          // check 15 seconds if connected
  {
      lix.sweep_color(CRGB(0,0,255), 20, 5);                                   //Sweep idle
      if (WiFi.status() == WL_CONNECTED)
      {
          wifi_connected = true;
          break;
      } 
      Serial.print("Trying to connect... Try number:");
      Serial.println(cnt);
      delay(500);
  }

  // Connect to the RTC
  rtcObject.Begin(); // conneect to RTC

  // If we have Wifi, we will get the current time from the NTP
  if(wifi_connected){
    Serial.println("Setup sync with NTP service.");
    setSyncProvider(getNTP_UTCTime1970);
    setSyncInterval(86400); // NTP re-sync; i.e. 86400 sec would be once per day
    yield();
    time_t tT = now();
    time_t tTlocal = CE.toLocal(tT);
    tag = day(tTlocal);
    monat = month(tTlocal);
    jahr = year(tTlocal);
    sekunde = second(tTlocal);
    minu = minute(tTlocal);
    stunde = hour(tTlocal);
    Serial.println(tag);
    Serial.println(monat);
    Serial.println(jahr);
    Serial.println(stunde);
    Serial.println(minu);
    Serial.println(sekunde);
  
    yield();
  
    RtcDateTime currentTime = RtcDateTime(jahr,monat,tag,stunde,minu,sekunde); //define date and time object
    rtcObject.SetDateTime(currentTime);
    zeit = (stunde * 100) + minu;
  } 
  // Otherwise we load the time stored in the RTC and hope that it was keeping track 
  else{
    Serial.println("No Wifi, fetching last saved time from RTC");
    RtcDateTime currentTime = rtcObject.GetDateTime();
    zeit = currentTime.Hour() * 100 + currentTime.Minute();
    sekunde = currentTime.Second();
    
    Serial.print("Setting soft-AP configuration ... ");
    Serial.println(WiFi.softAPConfig(local_IP, gateway, subnet) ? "Ready" : "Failed!");

    Serial.print("Setting soft-AP ... ");

    bool open_softAP_worked = WiFi.softAP(own_ap_ssid, own_ap_pass); 
    Serial.println( open_softAP_worked ? "Ready" : "Failed!");

    Serial.print("Soft-AP IP address = ");
    Serial.println(WiFi.softAPIP());
    
    server.begin();
  }

  // Set the RTC to give as an interrupt each second
  rtcObject.Enable32kHzPin(false);
  rtcObject.SetSquareWavePin(DS3231SquareWavePin_ModeClock);
  rtcObject.SetSquareWavePinClockFrequency(DS3231SquareWaveClock_1Hz);
  rtcObject.SetIsRunning(true);

  // feed the watchdogs
  yield();

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

void makeWebServer(){
  WiFiClient client = server.available();   // Auf Clients (Server-Aufrufe) warten

  if (client) {                             // Bei einem Aufruf des Servers
    Serial.println("Client available");
    String currentLine = "";                // String definieren für die Anfrage des Clients

    while (client.connected()) { // Loop, solange Client verbunden ist

      if (client.available()) {
        char c = client.read();             // Ein (1) Zeichen der Anfrage des Clients lesen
        Serial.write(c);                    // und es im Seriellen Monitor ausgeben
        header += c;
        if (c == '\n') {                    // bis eine Neue Zeile ausgegeben wird

          // Wenn der Client eine Leerzeile sendet, ist das Ende des HTTP Request erreicht
          if (currentLine.length() == 0) {

            // Der Server sendet nun eine Antwort an den Client
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();

            // Die Webseite anzeigen
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\"></head>");
            client.println("<body><h1 align=\"center\">Hier spricht dein Server! :)</h1></body></html>");

            // Die Antwort mit einer Leerzeile beenden
            client.println();
            // Den Loop beenden
            break;
          } else { // Bei einer Neuen Zeile, die Variable leeren
            currentLine = "";
          }
        } else if (c != '\r') {  // alles andere als eine Leerzeile wird
          currentLine += c;      // der Variable hinzugefüht
        }
      }
    }
    // Variable für den Header leeren
    header = "";
    // Die Verbindung beenden
    client.stop();
    Serial.println("Client disconnected");
    Serial.println("");
  }
}

void loop() {
    makeWebServer();
    if(marker == 1)
      lix.write_fade(zeit+30000);
    else
      lix.write_fade(zeit+60000);
    if(updateclock){
      updateclock = 0;
      RtcDateTime currentTime = rtcObject.GetDateTime();
      zeit = currentTime.Hour() * 100 + currentTime.Minute();
      Serial.println(zeit);
    }
}

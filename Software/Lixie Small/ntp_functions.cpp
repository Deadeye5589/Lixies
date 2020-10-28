#include "ntp_functions.h"


void getNTPTime(){
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
}

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

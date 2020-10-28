#include "webserver_functions.h"


void printWebsiteToClient(){
  String htmlPage =
    String("<!DOCTYPE HTML>") +
    "<html>" 
      "<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">" 
      "<link rel=\"icon\" href=\"data:,\">" 
    "</head>" 
    "<body>" 
      "<h1 align=\"center\">Hier spricht dein Server! :)</h1>" 
      "Aktuelle IP:  " + WiFi.softAPIP().toString().c_str() +
      "<h1>Time</h1>" 
      "<form action=\"/action_page\">" 
        "<input type='text' name='time_hh' id='time_hh' size=2 autofocus> hh" 
        "<input type='text' name='time_mm' id='time_mm' size=2 autofocus> mm" 
        "<input type='text' name='time_ss' id='time_ss' size=2 autofocus> ss" 
        "<div>" 
          "<br><button id=\"save_button\">Save</button>" 
        "</div>" 
      "</form>" 
    "</body>" 
    "</html>" 
    "\r\n";
  server.send(200, "text/html", htmlPage); //Send web page
}

void handleForm() {
  String time_hh = server.arg("time_hh"); 
  String time_mm = server.arg("time_mm");
  String time_ss = server.arg("time_ss"); 
  
  Serial.print("time_hh:");
  Serial.println(time_hh);
  
  Serial.print("time_mm:");
  Serial.println(time_mm);

  Serial.print("time_ss:");
  Serial.println(time_ss);
  

  stunde = time_hh.toInt();
  minu = time_mm.toInt();
  sekunde = time_ss.toInt();

  setRTC(jahr,monat, tag, stunde, minu, sekunde);
  zeit = (stunde * 100) + minu;

  String s = "<a href='/'> Go Back </a>";
  server.send(200, "text/html", s); //Send web page
}

void initWebserver(){
    server.on("/", printWebsiteToClient);
    server.on("/action_page", handleForm);

    server.begin();
    Serial.printf("Webserver started, open %s in a web browser\n", WiFi.softAPIP().toString().c_str());  
}

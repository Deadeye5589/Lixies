#include "wifi_functions.h"
 

void initSoftAP(){
    Serial.print("Setting soft-AP configuration ... ");
    Serial.println(WiFi.softAPConfig(local_IP, gateway, subnet) ? "Ready" : "Failed!");

    Serial.print("Setting soft-AP ... ");

    bool open_softAP_worked = WiFi.softAP(own_ap_ssid, own_ap_pass); 
    Serial.println( open_softAP_worked ? "Ready" : "Failed!");
}
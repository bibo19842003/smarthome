#include <ESP8266WiFi.h>          //ESP8266 Core WiFi Library (you most likely already have this in your sketch)

#include <DNSServer.h>            //Local DNS Server used for redirecting all requests to the configuration portal
#include <ESP8266WebServer.h>     //Local WebServer used to serve the configuration portal
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager WiFi Configuration Magic 

//https://github.com/tzapu/WiFiManager

  
void setup()
{

  Serial.begin(115200);
  Serial.println("=================================");
  
  WiFiManager wifiManager;
  wifiManager.autoConnect("111111111", "222222");

}

void loop()
{
  delay(5000);
  Serial.println("111111");
}


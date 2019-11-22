#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino

//needed for library
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>         //https://github.com/tzapu/WiFiManager

int button = 2;
int temp = 0;

char* aaa;
void setup() {
    // put your setup code here, to run once:
    Serial.begin(115200);
    Serial.println("=================================");

    //WiFiManager
    //Local intialization. Once its business is done, there is no need to keep it around
    WiFiManager wifiManager;
    //reset saved settings
    //wifiManager.resetSettings();
    aaa = "888888888888888888888";
    wifiManager.setCustomHeadElement(aaa);
    //set custom ip for portal
    //wifiManager.setAPStaticIPConfig(IPAddress(10,0,1,1), IPAddress(10,0,1,1), IPAddress(255,255,255,0));

    //fetches ssid and pass from eeprom and tries to connect
    //if it does not connect it starts an access point with the specified name
    //here  "AutoConnectAP"
    //and goes into a blocking loop awaiting configuration
    wifiManager.autoConnect("AutoConnectAP");
    

    // wifiManager.addParameter(WiFi.localIP());
    //or use this for auto generated name ESP + ChipID
    //wifiManager.autoConnect();

    
    //if you get here you have connected to the WiFi
    Serial.println("connected...yeey :)");

    pinMode(button, INPUT);
    

}

void loop() {
    // put your main code here, to run repeatedly:
  temp = digitalRead(button);
  if (temp == LOW) {
      WiFiManager wifiManager;
      wifiManager.resetSettings();
      Serial.println("reset password");
  }

  Serial.println("1111111111111111)");
  Serial.println(WiFi.localIP());
  Serial.println(WiFi.localIP()[0]);
  delay(2000);
}

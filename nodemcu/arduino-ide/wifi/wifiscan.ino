
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

#ifndef AP_SSID
#define AP_SSID "hello"
#define AP_PSK  "123456789"
#endif


/* Set these to your desired credentials. */
const char *ssid = AP_SSID;
const char *password = AP_PSK;


IPAddress local_IP(192,168,8,1);
IPAddress gateway(192,168,8,1);
IPAddress subnet(255,255,255,0);

ESP8266WebServer server(80);

const char* www_username = "admin";
const char* www_password = "hello";


int blinkled = LED_BUILTIN;

void blink(){
  digitalWrite(blinkled, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1000);                       // wait for a second
  digitalWrite(blinkled, LOW);    // turn the LED off by making the voltage LOW
  delay(1000);                       // wait for a second  
}


void handleRoot() {
  server.send(200, "text/html", "<h1>You are connected</h1>");
}


void wifiscanresult(){

  String myhtml ="<html><head><title>Smarthome</title></head><body><h1>Test Smarthome<h1></body></html>";

  Serial.println("scan start");

  // WiFi.scanNetworks will return the number of networks found
  int n = WiFi.scanNetworks();
  Serial.println("scan done");
  if (n == 0) {
    Serial.println("no networks found");
  } else {
    Serial.print(n);
    Serial.println(" networks found");
    for (int i = 0; i < n; ++i) {
      // Print SSID and RSSI for each network found
      myhtml += "<br>";
      myhtml += WiFi.SSID(i) ;
      myhtml += "   ";
      myhtml += WiFi.RSSI(i) ;

      delay(10);
    }
  }
  Serial.println("");

  // Wait a bit before scanning again


  server.send(200,"text/html",myhtml);
}









void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);

  delay(1000);
  Serial.begin(115200);
  Serial.println();
  Serial.print("Configuring access point...");
  /* You can remove the password parameter if you want the AP to be open. */
  WiFi.softAP(ssid, password);

  server.on("/", []() {
    if (!server.authenticate(www_username, www_password)) {
      return server.requestAuthentication();
    }
    server.send(200, "text/plain", "Login OK");
  });
  server.on("/a", handleRoot);  
  server.on("/b",wifiscanresult);
  server.begin();
  Serial.println("HTTP server started");
  
  Serial.print("Soft-AP IP address = ");
  Serial.println(WiFi.softAPIP());
  Serial.printf("MAC address = %s\n", WiFi.softAPmacAddress().c_str());
  
}













// the loop function runs over and over again forever
void loop() {

  // blink
  // blink();

  server.handleClient();

  Serial.printf("Stations connected to soft-AP = %d\n", WiFi.softAPgetStationNum());

  delay(2000);






  delay(5000);


  
}








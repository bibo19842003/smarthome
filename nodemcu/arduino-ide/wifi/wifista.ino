#include<ESP8266WebServer.h>

ESP8266WebServer server;

const char* ssid = "username"; //要加入的WiFi名
const char* password = "password"; //要加入的WiFi密码

#define testled 16
#define greenled 14


void setup() 
{
 pinMode(testled,OUTPUT);
 pinMode(greenled,OUTPUT);
 Serial.begin(115200);
 WiFi.mode(WIFI_STA); //设置为STA模式
 WiFi.begin(ssid, password); //连接目标WiFi
 Serial.println();
 Serial.print("AP IP address :");
 while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(F("."));
 }
 Serial.println();
 server.begin();
 Serial.println(WiFi.localIP());
 server.on("/control",control);
 server.on("/",control);
}
void loop() 
{
  server.handleClient();
}
void control()
{
  String myhtml ="<html><head><title>Test Smarthome</title></head><body><h1>Test Smarthome<h1><form>test led:<button type=\"submit\" name=\"testledstatus\"value=\"0\">off</button><button type=\"submit\" name=\"testledstatus\" value=\"1\">on</button></form><form>green led:<button type=\"submit\" name=\"greenledstatus\" value=\"0\">off</button><button type=\"submit\" name=\"greenledstatus\" value=\"1\">on</button></form></body></html>";
  server.send(200,"text/html",myhtml);

  if (server.arg("testledstatus")=="0") {
    digitalWrite(testled,LOW);
  } else if (server.arg("testledstatus")=="1") {
    digitalWrite(testled,HIGH);
  }

  if (server.arg("greenledstatus")=="0") {
    digitalWrite(greenled,LOW);
  } else if (server.arg("greenledstatus")=="1") {
    digitalWrite(greenled,HIGH);
  }

}


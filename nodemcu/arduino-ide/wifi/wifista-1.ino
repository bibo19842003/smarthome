#include<ESP8266WebServer.h>
ESP8266WebServer server;

const char* ssid = "username"; //要加入的WiFi名
const char* password = "password"; //要加入的WiFi密码

#define pin 16


void setup() 
{
 pinMode(pin,OUTPUT);
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
 server.on("/led",led);
}
void loop() 
{
  server.handleClient();
}
void led()
{
  String myhtml ="<html><head><title>my first web</title></head><body style=\"background-color: blue\"><h1><center>led</center><h1><form>what do u want to do<br><button type=\"submit\" name=\"state\" value=\"0\">on</button><button type=\"submit\" name=\"state\" value=\"1\">off</button></form></body></html>";
  server.send(200,"text/html",myhtml);
  if(server.arg("state")=="0")
  {
  digitalWrite(pin,LOW);
  }
  else
  {
    digitalWrite(pin,1);
  }
}


#include <SimpleDHT.h>
#include <Arduino.h>
#include <U8g2lib.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <Wire.h> //IIC
#include <math.h> 
#include "pitches.h"
#include <DNSServer.h>
#include <WiFiManager.h>


// pin 使用说明：
// D0(16): wifiled
// D1(5): SCL
// D2(4): SDA
// D3(0): 
// D4(2): wifibutton 用于清除wifi密码
// D5(14): 
// D6(12): dht11 信号
// D7(13): 
// D8(15): speaker


// DHT11: https://blog.bbskali.cn/index.php/archives/1337/

// notes in the melody:
int melody[] = {NOTE_C4, NOTE_G3, NOTE_G3, NOTE_A3, NOTE_G3, 0, NOTE_B3, NOTE_C4};
// note durations: 4 = quarter note, 8 = eighth note, etc.:
int noteDurations[] = {4, 8, 8, 4, 4, 4, 4, 4};

int led = 16;     // LED引脚
int wifibutton = D4;  //wifibutton
int temp = 0;    // 用于读取按键状态的临时变量
int speakerpin = D8;  // 用于测试


int pinDHT11 = D6;
SimpleDHT11 dht11(pinDHT11);


// Create an instance of the server
// specify the port to listen on as an argument
ESP8266WebServer server;
const char HTTP_HEADER_NEW[] PROGMEM          = "<!DOCTYPE html><html lang=\"en\"><head><meta charset=\"UTF-8\"><meta name=\"viewport\" content=\"width=device-width, initial-scale=1, user-scalable=no\"/><meta http-equiv=\"refresh\" content=\"5\"><title></title>";

// 温度 湿度
byte temperature = 0;
byte humidity = 0;

void mytone() {
    Serial.println("LED Turned ON, buzzer on");

    // iterate over the notes of the melody:
    for (int thisNote = 0; thisNote < 8; thisNote++) {

      // to calculate the note duration, take one second divided by the note type.
      //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
      int noteDuration = 1000 / noteDurations[thisNote];
      tone(speakerpin, melody[thisNote], noteDuration);

      // to distinguish the notes, set a minimum time between them.
      // the note's duration + 30% seems to work well:
      int pauseBetweenNotes = noteDuration * 1.30;
      delay(pauseBetweenNotes);
      // stop the tone playing:
      noTone(8);  
    }
}


void valsensor()
{
  // FPSTR from <WiFiManager.h>
  String myhtml = FPSTR(HTTP_HEADER_NEW);
  myhtml += FPSTR(HTTP_SCRIPT);
  myhtml += FPSTR(HTTP_STYLE);
  myhtml += FPSTR(HTTP_HEADER_END);
  myhtml += "Temperature: ";
  myhtml += temperature;
  myhtml += " C";
  myhtml += "<br>";
  myhtml += "Humidity: ";
  myhtml += humidity;
  myhtml += " %";
  myhtml += "<br>";

  myhtml += FPSTR(HTTP_END);
  
  server.send(200,"text/html",myhtml);
}


void nocss()
{
//  String myhtml ="<html><head><title>Smart Home</title></head><body><h1>Test Smarthome<h1><form>test led:<button type=\"submit\" name=\"testledstatus\"value=\"0\">off</button><button type=\"submit\" name=\"testledstatus\" value=\"1\">on</button></form><form>buzzer and led:<button type=\"submit\" name=\"ledstatus\" value=\"0\">off</button><button type=\"submit\" name=\"ledstatus\" value=\"1\">on</button></form><form>Wifi mode:<button type=\"submit\" name=\"wifiled\" value=\"0\">STA</button><button type=\"submit\" name=\"wifiled\" value=\"1\">STAAP</button></form></body></html>";
  String myhtml ="<html><head><title>Smart Home</title></head><body><h1>Smart Home<h1><form>buzzer and led:<button type=\"submit\" name=\"ledstatus\" value=\"0\">off</button><button type=\"submit\" name=\"ledstatus\" value=\"1\">on</button></form></body></html>";

  myhtml += "Temperature: ";
  myhtml += temperature;
  myhtml += " C";
  myhtml += "<br>";
  myhtml += "Humidity: ";
  myhtml += humidity;
  myhtml += " %";
  myhtml += "<br>";
  server.send(200,"text/html",myhtml);

  if (server.arg("ledstatus")=="0") {
    digitalWrite(led,LOW);
  } else if (server.arg("ledstatus")=="1") {
    mytone();
    digitalWrite(led,HIGH);
  }
}






// handleWebRequests
String getContentType(String filename){  
  if(server.hasArg("download")) return "application/octet-stream";  
  else if(filename.endsWith(".htm")) return "text/html";  
  else if(filename.endsWith(".html")) return "text/html";  
  else if(filename.endsWith(".css")) return "text/css";  
  else if(filename.endsWith(".js")) return "application/javascript";  
  else if(filename.endsWith(".png")) return "image/png";  
  else if(filename.endsWith(".gif")) return "image/gif";  
  else if(filename.endsWith(".jpg")) return "image/jpeg";  
  else if(filename.endsWith(".ico")) return "image/x-icon";  
  else if(filename.endsWith(".xml")) return "text/xml";  
  else if(filename.endsWith(".pdf")) return "application/x-pdf";  
  else if(filename.endsWith(".zip")) return "application/x-zip";  
  else if(filename.endsWith(".gz")) return "application/x-gzip";  
  return "text/plain";  
}

  
// NotFound处理 用于处理没有注册的请求地址 一般是处理一些页面请求 
void handleNotFound() {  
  String path = server.uri();  
  Serial.print("load url:");  
  Serial.println(path);  
  String contentType = getContentType(path);  
  String message = "File Not Found\n\n";  
  message += "URI: ";  
  message += server.uri();  
  message += "\nMethod: ";  
  message += ( server.method() == HTTP_GET ) ? "GET" : "POST";  
  message += "\nArguments: ";  
  message += server.args();  
  message += "\n";  
  for ( uint8_t i = 0; i < server.args(); i++ ) {  
    message += " " + server.argName ( i ) + ": " + server.arg ( i ) + "\n";  
  }  
  server.send ( 404, "text/plain", message );  
}  


void clearwifiinfor() {
  Serial.println("it will clear password!!!");
  WiFiManager wifiManager;
  wifiManager.resetSettings();
  Serial.println("it had cleared password!!!");
}


void setup() {

  Serial.begin(115200);
  Serial.println("=================================");

  // Connect to WiFi network
  WiFiManager wifiManager;
  wifiManager.autoConnect("dragonfly");

  // server route
  server.on("/",nocss);
  server.on("/sensor",valsensor);
  server.on("/nocss",nocss);
  server.onNotFound(handleNotFound);
  
  // Start the server
  server.begin();
  Serial.println(WiFi.localIP());

  // set pinmode
  pinMode(led, OUTPUT);   // LED设置为输入
  pinMode(wifibutton, INPUT); // 按键设置为输出
  pinMode(pinDHT11, OUTPUT); // 温度传感器设置为输入
  pinMode(speakerpin, OUTPUT);   // LED设置为输入

}






void loop() {
  
  // 获取温度 湿度

  int err = SimpleDHTErrSuccess;
  digitalWrite(pinDHT11, HIGH);
  
  err = dht11.read(&temperature, &humidity, NULL);

  // wait for http client
  server.handleClient();

  // wifibutton
  temp = digitalRead(wifibutton);
  if (temp == LOW) {
    clearwifiinfor();
  }

  delay(1000);
}

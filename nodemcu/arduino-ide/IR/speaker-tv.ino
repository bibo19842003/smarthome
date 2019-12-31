#include <SimpleDHT.h>
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <Wire.h> //IIC
#include <math.h> 
#include "pitches.h"
#include <WiFiManager.h>
#include <ESP8266mDNS.h>
#include <WiFiClient.h>
#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRutils.h>
#include <IRsend.h>




// pin 使用说明：
// D0(16): wifiled
// D1(5): SCL
// D2(4): SDA
// D3(0): 
// D4(2): wifibutton 用于清除wifi密码
// D5(14): 
// D6(12): dht11 信号
// D7(13): 红外线发送
// D8(15): speaker


// DHT11: https://blog.bbskali.cn/index.php/archives/1337/


// 各种变量定义
// 各种变量定义 - begin

// mdns name 定义
// String mdnsname = "dragonfly-" + String(ESP.getChipId()); //dragonfly-7914686.local
  String mdnsname = "speaker";

// http web page ,FPSTR from <WiFiManager.h>
const char HTTP_HEADER_NEW[] PROGMEM          = "<!DOCTYPE html><html lang=\"en\"><head><meta charset=\"UTF-8\"><meta name=\"viewport\" content=\"width=device-width, initial-scale=1, user-scalable=no\"/><meta http-equiv=\"refresh\" content=\"5\"><title></title>";
const char HTTP_STYLE_NEW[] PROGMEM           = "<style>.c{text-align: center;} div,input{padding:5px;font-size:1em;} input{width:95%;} body{text-align: center;font-family:verdana;} button{border:0;border-radius:0.3rem;background-color:#1fa3ec;color:#fff;line-height:2.4rem;font-size:1.2rem;width:100%;} .q{float: right;width: 64px;text-align: right;} </style>";
const char HTTP_SCRIPT_NEW[] PROGMEM          = "<script>function c(l){document.getElementById('s').value=l.innerText||l.textContent;document.getElementById('p').focus();}</script>";
const char HTTP_HEADER_END_NEW[] PROGMEM        = "</head><body><div style='text-align:center;display:inline-block;min-width:260px;'>";
const char HTTP_END_NEW[] PROGMEM             = "</div></body></html>";

const char HTTP_HEADER_NEW_IR[] PROGMEM          = "<!DOCTYPE html><html lang=\"en\"><head><meta charset=\"UTF-8\"><meta name=\"viewport\" content=\"width=device-width, initial-scale=1, user-scalable=no\"/><title></title>";
const char HTTP_STYLE_NEW_IR[] PROGMEM           = "<style>.c{text-align: center;} div,input{padding:5px;font-size:1em;} input{width:95%;} body{text-align: center;font-family:verdana;} button{border:0;border-radius:0.3rem;background-color:#1fa3ec;color:#fff;line-height:2.4rem;font-size:1.2rem;width:130px;} .q{float: right;width: 64px;text-align: right;} </style>";

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

// 温度 湿度
byte temperature = 0;
byte humidity = 0;

// 红外线 
const uint16_t kIrLed = D7; //发送
IRsend irsend(kIrLed);  // Set the GPIO to be used to sending the message.

// 各种变量定义 - end


// ***********************************


// 各种服务定义
// 各种服务定义 - begin

// TCP server at port 80 will respond to HTTP requests
ESP8266WebServer server;



// 各种服务定义 - end

// ***********************************




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
  myhtml += FPSTR(HTTP_SCRIPT_NEW);
  myhtml += FPSTR(HTTP_STYLE_NEW);
  myhtml += FPSTR(HTTP_HEADER_END_NEW);

  myhtml += "Temperature: ";
  myhtml += temperature;
  myhtml += " C";
  myhtml += "<br>";
  myhtml += "Humidity: ";
  myhtml += humidity;
  myhtml += " %";
  myhtml += "<br>";

  myhtml += FPSTR(HTTP_END_NEW);  
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


// tv - page
void tv()
{  
  String myhtml = FPSTR(HTTP_HEADER_NEW_IR);
  myhtml += FPSTR(HTTP_SCRIPT_NEW);
  myhtml += FPSTR(HTTP_STYLE_NEW_IR);
  myhtml += FPSTR(HTTP_HEADER_END_NEW);

  myhtml += "<form>";
 // myhtml += "<br/><br/>";
  myhtml += "<button type=\"submit\" name=\"tv\" value=\"32A650AF\">电源</button><br/><br/>";
  myhtml += "<button type=\"submit\" name=\"tv\" value=\"32A6C23D\">电视</button>";
  myhtml += "<button type=\"submit\" name=\"tv\" value=\"32A6827D\">HDMI</button><br/><br/>";
  myhtml += "<button type=\"submit\" name=\"tv\" value=\"32A6807F\">1</button>";
  myhtml += "<button type=\"submit\" name=\"tv\" value=\"32A640BF\">2</button>";
  myhtml += "<button type=\"submit\" name=\"tv\" value=\"32A6C03F\">3</button><br/><br/>";
  myhtml += "<button type=\"submit\" name=\"tv\" value=\"32A620DF\">4</button>";
  myhtml += "<button type=\"submit\" name=\"tv\" value=\"32A6A05F\">5</button>";
  myhtml += "<button type=\"submit\" name=\"tv\" value=\"32A6609F\">6</button><br/><br/>";
  myhtml += "<button type=\"submit\" name=\"tv\" value=\"32A6E01F\">7</button>";
  myhtml += "<button type=\"submit\" name=\"tv\" value=\"32A610EF\">8</button>";
  myhtml += "<button type=\"submit\" name=\"tv\" value=\"32A6906F\">9</button><br/><br/>";
  myhtml += "<button type=\"submit\" name=\"tv\" value=\"32A6A25D\">0</button><br/><br/>";
  myhtml += "<button type=\"submit\" name=\"tv\" value=\"32A6A857\">音量+</button>";
  myhtml += "<button type=\"submit\" name=\"tv\" value=\"32A6F00F\">静音</button>";
  myhtml += "<button type=\"submit\" name=\"tv\" value=\"32A6F807\">频道+</button><br/><br/>";
  myhtml += "<button type=\"submit\" name=\"tv\" value=\"32A638C7\">音量-</button>";
  myhtml += "<button type=\"submit\" name=\"tv\" value=\"32A600FF\">设置</button>";
  myhtml += "<button type=\"submit\" name=\"tv\" value=\"32A67887\">频道-</button><br/><br/>";
  myhtml += "<button type=\"submit\" name=\"tv\" value=\"32A602FD\">屏显</button>";
  myhtml += "<button type=\"submit\" name=\"tv\" value=\"32A630CF\">信号源</button><br/><br/>";
  myhtml += "<button type=\"submit\" name=\"tv\" value=\"32A6D02F\">上</button><br/><br/>";
  myhtml += "<button type=\"submit\" name=\"tv\" value=\"32A608F7\">左</button>";
  myhtml += "<button type=\"submit\" name=\"tv\" value=\"32A6B04F\">确定</button>";
  myhtml += "<button type=\"submit\" name=\"tv\" value=\"32A68877\">右</button><br/><br/>";
  myhtml += "<button type=\"submit\" name=\"tv\" value=\"32A6708F\">下</button><br/><br/>";
  myhtml += "<button type=\"submit\" name=\"tv\" value=\"32A6EC13\">菜单</button>";
  myhtml += "<button type=\"submit\" name=\"tv\" value=\"32A66897\">主页</button>";
  myhtml += "<button type=\"submit\" name=\"tv\" value=\"32A6B847\">返回</button><br/><br/>";
  myhtml += "</form>";

  myhtml += FPSTR(HTTP_END_NEW);  
  server.send(200,"text/html",myhtml);

  // irsend.sendNEC(0x32A650AF); 发送信号
  // irsend.sendNEC(atoi(("0x" + server.arg("tv")).c_str()));
  if (server.arg("tv")=="32A650AF") {
    irsend.sendNEC(0x32A650AF);
  } else if (server.arg("tv")=="32A6C23D") {
    irsend.sendNEC(0x32A6C23D);
  } else if (server.arg("tv")=="32A6827D") {
    irsend.sendNEC(0x32A6827D);
  } else if (server.arg("tv")=="32A6807F") {
    irsend.sendNEC(0x32A6807F);
  } else if (server.arg("tv")=="32A640BF") {
    irsend.sendNEC(0x32A640BF);
  } else if (server.arg("tv")=="32A6C03F") {
    irsend.sendNEC(0x32A6C03F);
  } else if (server.arg("tv")=="32A620DF") {
    irsend.sendNEC(0x32A620DF);
  } else if (server.arg("tv")=="32A6A05F") {
    irsend.sendNEC(0x32A6A05F);
  } else if (server.arg("tv")=="32A6609F") {
    irsend.sendNEC(0x32A6609F);
  } else if (server.arg("tv")=="32A6E01F") {
    irsend.sendNEC(0x32A6E01F);
  } else if (server.arg("tv")=="32A610EF") {
    irsend.sendNEC(0x32A610EF);
  } else if (server.arg("tv")=="32A6906F") {
    irsend.sendNEC(0x32A6906F);
  } else if (server.arg("tv")=="32A6A25D") {
    irsend.sendNEC(0x32A6A25D);
  } else if (server.arg("tv")=="32A6A857") {
    irsend.sendNEC(0x32A6A857);
  } else if (server.arg("tv")=="32A6F00F") {
    irsend.sendNEC(0x32A6F00F);
  } else if (server.arg("tv")=="32A6F807") {
    irsend.sendNEC(0x32A6F807);
  } else if (server.arg("tv")=="32A638C7") {
    irsend.sendNEC(0x32A638C7);
  } else if (server.arg("tv")=="32A600FF") {
    irsend.sendNEC(0x32A600FF);
  } else if (server.arg("tv")=="32A67887") {
    irsend.sendNEC(0x32A67887);
  }  else if (server.arg("tv")=="32A602FD") {
    irsend.sendNEC(0x32A602FD);
  } else if (server.arg("tv")=="32A630CF") {
    irsend.sendNEC(0x32A630CF);
  } else if (server.arg("tv")=="32A6D02F") {
    irsend.sendNEC(0x32A6D02F);
  } else if (server.arg("tv")=="32A608F7") {
    irsend.sendNEC(0x32A608F7);
  } else if (server.arg("tv")=="32A6B04F") {
    irsend.sendNEC(0x32A6B04F);
  } else if (server.arg("tv")=="32A68877") {
    irsend.sendNEC(0x32A68877);
  } else if (server.arg("tv")=="32A6708F") {
    irsend.sendNEC(0x32A6708F);
  } else if (server.arg("tv")=="32A6EC13") {
    irsend.sendNEC(0x32A6EC13);
  } else if (server.arg("tv")=="32A66897") {
    irsend.sendNEC(0x32A66897);
  } else if (server.arg("tv")=="32A6B847") {
    irsend.sendNEC(0x32A6B847);
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


void mdnsresponder() {
 
  IPAddress ip = WiFi.localIP();
  String ipStr = String(ip[0]) + '.' + String(ip[1]) + '.' + String(ip[2]) + '.' + String(ip[3]);

  String myhtml = FPSTR(HTTP_HEADER_NEW);
  myhtml += FPSTR(HTTP_SCRIPT_NEW);
  myhtml += FPSTR(HTTP_STYLE_NEW);
  myhtml += FPSTR(HTTP_HEADER_END_NEW);
  
  myhtml += "Hello dragonfly! ";
  myhtml += "<br>";
  myhtml += "IP: ";
  myhtml += ipStr;

  myhtml += FPSTR(HTTP_END_NEW);
  server.send(200,"text/html",myhtml);
}


void setup() {

  Serial.begin(115200);
  Serial.println("=================================");

  // 红外线发送初始化
  irsend.begin();

  // set pinmode
  pinMode(led, OUTPUT);   // LED设置为输入
  pinMode(wifibutton, INPUT); // 按键设置为输出
  pinMode(pinDHT11, OUTPUT); // 温度传感器设置为输入
  pinMode(speakerpin, OUTPUT);   // LED设置为输入

  // Connect to WiFi network
  WiFiManager wifiManager;
  wifiManager.autoConnect("dragonfly");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println(WiFi.localIP());

  // Set up mDNS responder:
  if (!MDNS.begin(mdnsname)) {
    Serial.println("Error setting up MDNS responder!");
    while (1) {
      delay(1000);
    }
  }
  Serial.println("mDNS responder started");
  Serial.println(mdnsname);
  //往mDNS里面注册服务
  MDNS.addService("speaker", "tcp", 8080); 

  // server route
  server.on("/",nocss);
  server.on("/sensor",valsensor);
  server.on("/nocss",nocss);
  server.on("/mdns",mdnsresponder);
  server.on("/tv",tv);
  server.onNotFound(handleNotFound);
  
  // Start the server
  server.begin();
  Serial.println(WiFi.localIP());

}




void loop() {
  
  MDNS.update();

  // 获取温度 湿度
//  int err = SimpleDHTErrSuccess;
//  digitalWrite(pinDHT11, HIGH);  
//  err = dht11.read(&temperature, &humidity, NULL);

  // wait for http client
  server.handleClient();

  // wifibutton
  temp = digitalRead(wifibutton);
  if (temp == LOW) {
    clearwifiinfor();
  }

  delay(1000);
}

#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiClient.h>
#include <WiFiManager.h>
#include <Arduino.h>
#include <ESP8266httpUpdate.h>


// pin 使用说明：
// D0(16): wifiled
// D1(5): SCL
// D2(4): SDA
// D3(0): st7796
// D4(2): st7796 wifibutton 用于清除wifi密码
// D5(14): st7796
// D6(12): st7796 dht11 信号
// D7(13): st7796
// D8(15): st7796 空气质量
// SD2(9): 
// SD3(10): 


// ***********************************

// 各种自定义配置，如无特殊要求，可以用默认配置。

// OTA 配置（默认不更新）
// OTA-定义软件版本，用于OTA升级校验使用
float localversion = 1.0;

// OTA-定义服务器端bin文件的最新 version ，例如路径：http://192.168.1.9:8888/static/log/member/version 
// 通过 django 数据库进行管理，api 创建参考 https://blog.csdn.net/weixin_43420032/article/details/94631231
String versionpath = "";

// OTA-定义bin文件的下载路径，用于OTA升级下载bin文件，例如：http://192.168.1.9:8888/static/log/member/test1.hello.ino.nodemcu.bin
String otapath = "";

// mdns 配置
// mdns name 定义
String mdnsname = "dragonfly-" + String(ESP.getChipId()); //dragonfly-7914686.local




// ***********************************


// 各种变量定义
// 各种变量定义 - begin


// http web page ,FPSTR from <WiFiManager.h>
const char HTTP_HEADER_NEW[] PROGMEM          = "<!DOCTYPE html><html lang=\"en\"><head><meta charset=\"UTF-8\"><meta name=\"viewport\" content=\"width=device-width, initial-scale=1, user-scalable=no\"/><meta http-equiv=\"refresh\" content=\"5\"><title></title>";
const char HTTP_STYLE_NEW[] PROGMEM           = "<style>.c{text-align: center;} div,input{padding:5px;font-size:1em;} input{width:95%;} body{text-align: center;font-family:verdana;} button{border:0;border-radius:0.3rem;background-color:#1fa3ec;color:#fff;line-height:2.4rem;font-size:1.2rem;width:100%;} .q{float: right;width: 64px;text-align: right;} </style>";
const char HTTP_SCRIPT_NEW[] PROGMEM          = "<script>function c(l){document.getElementById('s').value=l.innerText||l.textContent;document.getElementById('p').focus();}</script>";
const char HTTP_HEADER_END_NEW[] PROGMEM        = "</head><body><div style='text-align:left;display:inline-block;min-width:260px;'>";
const char HTTP_END_NEW[] PROGMEM             = "</div></body></html>";

// pin 定义
int wifibutton = D4;  //wifibutton

// 各种变量定义 - end


// ***********************************


// 各种服务定义
// 各种服务定义 - begin

// TCP server at port 80 will respond to HTTP requests
ESP8266WebServer server;



// 各种服务定义 - end

// *********************************** *********************************** ***********************************




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




void valsensor()
{
  String myhtml = FPSTR(HTTP_HEADER_NEW);
  myhtml += FPSTR(HTTP_SCRIPT);
  myhtml += FPSTR(HTTP_STYLE);
  myhtml += FPSTR(HTTP_HEADER_END_NEW);
  
  myhtml += "Temperature: ";
//  myhtml += temperature;
  myhtml += " C";
  myhtml += "<br>";
  myhtml += "Humidity: ";
//  myhtml += humidity;
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
 // myhtml += temperature;
  myhtml += " C";
  myhtml += "<br>";
  myhtml += "Humidity: ";
//  myhtml += humidity;
  myhtml += " %";
  myhtml += "<br>";
  server.send(200,"text/html",myhtml);

//  if (server.arg("ledstatus")=="0") {
 //   digitalWrite(led,LOW);
 // } else if (server.arg("ledstatus")=="1") {
 //   mytone();
 //   digitalWrite(led,HIGH);
 // }
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


void otaupdate(){
  if(otapath != ""){
      t_httpUpdate_return ret = ESPhttpUpdate.update(otapath); // 编译好的固件文件
      switch(ret) {
          case HTTP_UPDATE_FAILED:
              Serial.printf("HTTP_UPDATE_FAILD Error (%d): %s", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
              break;
          case HTTP_UPDATE_NO_UPDATES:
              Serial.println("HTTP_UPDATE_NO_UPDATES");
              break;
          case HTTP_UPDATE_OK:
              Serial.println("HTTP_UPDATE_OK");
              break;
        }
  } 
}


// clear wifi infor. it needs set the wifi after press the wifibutton.
void clearwifiinfor() {
  Serial.println("it will clear password!!!");
  WiFiManager wifiManager;
  wifiManager.resetSettings();
  Serial.println("it had cleared password!!!");
}



void setup(void) {
  
  Serial.begin(115200);
  Serial.println("=================================");

  // set pinmode
  //pinMode(wifiled, OUTPUT);   // LED设置为输入
  //pinMode(pinDHT11, OUTPUT); // 温度传感器设置为输入
  pinMode(wifibutton, INPUT); // wifi重置
  //pinMode(ledPower,OUTPUT);  // 空气质量
  //pinMode(dustPin, INPUT);

  // Connect to WiFi network
  WiFiManager wifiManager;
  wifiManager.autoConnect("dragonfly");
  Serial.println("");

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

  // server route
  server.on("/",nocss);
  server.on("/sensor",valsensor);
  server.on("/nocss",nocss);
  server.on("/mdns",mdnsresponder);
  server.on("/otaupdate",otaupdate);
  server.onNotFound(handleNotFound);

  // Start TCP (HTTP) server
  server.begin();
  Serial.println("TCP server started");

}

void loop(void) {

  MDNS.update();

  // wait for http client
  server.handleClient();

  // wifibutton clear wifi infor
  temp = digitalRead(wifibutton);
  if (temp == LOW) {
    clearwifiinfor();
  }

  delay(1000);


}

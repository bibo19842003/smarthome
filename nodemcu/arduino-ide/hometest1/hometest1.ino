#include <SimpleDHT.h>
#include <Arduino.h>
#include <U8g2lib.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <time.h>
#include <Wire.h> //IIC
#include <math.h> 
#include <FS.h>
#include <DNSServer.h>
#include <WiFiManager.h>
#include <Adafruit_Sensor.h>


// pin 使用说明：
// D0(16): led buzzer 信号 待用
// D1(5): SCL 待用
// D2(4): SDA 待用
// D3(0): 光强度 信号
// D4(2): dht11 信号
// D5(14): 信号灯 红
// D6(12): 信号灯 黄
// D7(13): 信号灯 绿
// D8(15): 灯，与光强度（D3）关联


// DHT11: https://blog.bbskali.cn/index.php/archives/1337/


int led_r = 14;     // 信号灯 红
int led_y = 12; // 信号灯 黄
int led_g = 13;    // 信号灯 绿
int led_light = 15;  // 灯，与光强度（D3）关联

int pinDHT11 = D4;
SimpleDHT11 dht11(pinDHT11);


// Create an instance of the server
// specify the port to listen on as an argument
ESP8266WebServer server;
const char HTTP_HEADER_NEW[] PROGMEM          = "<!DOCTYPE html><html lang=\"en\"><head><meta charset=\"UTF-8\"><meta name=\"viewport\" content=\"width=device-width, initial-scale=1, user-scalable=no\"/><meta http-equiv=\"refresh\" content=\"5\"><title></title>";

// 温度 湿度
byte temperature = 0;
byte humidity = 0;

// 时间
int timezone = 8;
int dst = 0;

// 光强度
float voltage = 0;






//void blink(){
//  digitalWrite(wifimodeled, HIGH);   // turn the LED on (HIGH is the voltage level)
//  delay(1000);                       // wait for a second
////  digitalWrite(wifimodeled, LOW);    // turn the LED off by making the voltage LOW
//  delay(1000);                       // wait for a second  
//}

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
  myhtml += "Lumen: ";
  myhtml += voltage;
  myhtml += " lx";
  myhtml += "<br>";
  myhtml += FPSTR(HTTP_END);
  
  
  server.send(200,"text/html",myhtml);
}


void nocss()
{
//  String myhtml ="<html><head><title>Smart Home</title></head><body><h1>Test Smarthome<h1><form>test led:<button type=\"submit\" name=\"testledstatus\"value=\"0\">off</button><button type=\"submit\" name=\"testledstatus\" value=\"1\">on</button></form><form>buzzer and led:<button type=\"submit\" name=\"ledstatus\" value=\"0\">off</button><button type=\"submit\" name=\"ledstatus\" value=\"1\">on</button></form><form>Wifi mode:<button type=\"submit\" name=\"wifiled\" value=\"0\">STA</button><button type=\"submit\" name=\"wifiled\" value=\"1\">STAAP</button></form></body></html>";
  String myhtml ="<html><head><title>Smart Home</title></head><body><h1>Smart Home<h1><form>Red led:<button type=\"submit\" name=\"rledstatus\" value=\"0\">off</button><button type=\"submit\" name=\"rledstatus\" value=\"1\">on</button></form></body></html>";

  myhtml += "<form>Yellow led:<button type=\"submit\" name=\"yledstatus\" value=\"0\">off</button><button type=\"submit\" name=\"yledstatus\" value=\"1\">on</button></form>";
  myhtml += "<form>Green led:<button type=\"submit\" name=\"gledstatus\" value=\"0\">off</button><button type=\"submit\" name=\"gledstatus\" value=\"1\">on</button></form>";
  myhtml += "Temperature: ";
  myhtml += temperature;
  myhtml += " C";
  myhtml += "<br>";
  myhtml += "Humidity: ";
  myhtml += humidity;
  myhtml += " %";
  myhtml += "<br>";
  myhtml += "Lumen: ";
  myhtml += voltage;
  myhtml += " lx";
  server.send(200,"text/html",myhtml);

  if (server.arg("rledstatus")=="0") {
    digitalWrite(led_r,LOW);
  } else if (server.arg("rledstatus")=="1") {
    digitalWrite(led_r,HIGH);
  }

  if (server.arg("yledstatus")=="0") {
    digitalWrite(led_y,LOW);
  } else if (server.arg("yledstatus")=="1") {
    digitalWrite(led_y,HIGH);
  }

  if (server.arg("gledstatus")=="0") {
    digitalWrite(led_g,LOW);
  } else if (server.arg("gledstatus")=="1") {
    digitalWrite(led_g,HIGH);
  }
  
}


// 用于数字两位显示，如果是一位数字，那么首位为0，
String toe(int xxx) {
  if (xxx < 10) {
    return "0"+ String(xxx);
    }
  else {
    return String(xxx);
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
  String pathWithGz = path + ".gz";  
  if(SPIFFS.exists(pathWithGz) || SPIFFS.exists(path)){  
    if(SPIFFS.exists(pathWithGz))  
      path += ".gz";  
    File file = SPIFFS.open(path, "r");  
    size_t sent = server.streamFile(file, contentType);  
    file.close();  
    return;  
  }  
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


void clearwifipass() {
  WiFiManager wifiManager;
  wifiManager.resetSettings();
  Serial.println("clear password!!!");
}



void setup() {

  Serial.begin(115200);
  Serial.println("=================================");


  // Connect to WiFi network
  WiFiManager wifiManager;
  wifiManager.autoConnect("dragonfly");

  // SPIFFS 初始化
  SPIFFS.begin();  

  // server route
  server.on("/",nocss);
  server.on("/sensor",valsensor);
  server.on("/nocss",nocss);
  server.onNotFound(handleNotFound);
  
  // Start the server
  server.begin();
  Serial.println(WiFi.localIP());

  // 初始化 time
  configTime(timezone * 3600, dst * 0, "pool.ntp.org");
  Serial.println("\nWaiting for time");
  while (!time(nullptr)) {
    Serial.print(".");
    delay(1000);
    }
  Serial.println("");  



  // set pinmode
  pinMode(led_r, OUTPUT);   // LED设置为输入
  pinMode(led_y, OUTPUT);   // LED设置为输入
  pinMode(led_g, OUTPUT);   // LED设置为输入
  pinMode(led_light, OUTPUT);   // LED设置为输入
  pinMode(pinDHT11, OUTPUT); // 温度传感器设置为输入

}






void loop() {
  
  // 获取温度 湿度

  int err = SimpleDHTErrSuccess;
  digitalWrite(pinDHT11, 1);
  
  if ((err = dht11.read(&temperature, &humidity, NULL)) != SimpleDHTErrSuccess) {
//    Serial.print("Read DHT11 failed, err="); 
//    Serial.println(err);
    return;
  }

  Serial.print((int)temperature); Serial.print(" °C, "); 
  Serial.print((int)humidity); Serial.println(" H");

// 光强取值
  int lightsensorValue = analogRead(A0); // 读取模拟引脚A0上的输入
  float voltage = lightsensorValue * (3.3 / 1023.0); // 将模拟读数（从0 - 1023变为0）转换为电压（0 - 5V）
  Serial.println(voltage); // 打印出模拟口读取的值





// 显示时间
  time_t now;
  struct tm * timeinfo;
  time(&now);
  timeinfo = localtime(&now); 

  Serial.print(timeinfo->tm_year + 1900);
  Serial.print("-");
  Serial.print(toe(timeinfo->tm_mon));
  Serial.print("-");
  Serial.print(toe(timeinfo->tm_mday));
  Serial.print(" ");
  Serial.print(toe(timeinfo->tm_hour));
  Serial.print(":");
  Serial.print(toe(timeinfo->tm_min));
  Serial.print(":");
  Serial.println(toe(timeinfo->tm_sec));

  Serial.print("星期");
  Serial.print(timeinfo->tm_wday);
  Serial.print(" ");
  Serial.print("今年第");
  Serial.print(timeinfo->tm_yday);
  Serial.println("天");



  // wait for http client
  server.handleClient();




  delay(1000);
}

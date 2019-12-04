#include <SimpleDHT.h>
#include <Arduino.h>
#include <U8g2lib.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <time.h>
#include <Wire.h> //IIC
#include <math.h> 
#include "pitches.h"
#include <FS.h>
#include <DNSServer.h>
#include <WiFiManager.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>


// pin 使用说明：
// D0(16): led buzzer 信号
// D1(5): SCL
// D2(4): SDA
// D3(0): dht11 信号
// D4(2): button 信号
// D5(14): greed red only test 信号
// D6(12): 
// D7(13): 


U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, /* clock=*/ SCL, /* data=*/ SDA, /* reset=*/ U8X8_PIN_NONE);   // All Boards without Reset of the Display

// DHT11: https://blog.bbskali.cn/index.php/archives/1337/
// u8g2: https://blog.csdn.net/dpjcn1990/article/details/92831760


// notes in the melody:
int melody[] = {NOTE_C4, NOTE_G3, NOTE_G3, NOTE_A3, NOTE_G3, 0, NOTE_B3, NOTE_C4};
// note durations: 4 = quarter note, 8 = eighth note, etc.:
int noteDurations[] = {4, 8, 8, 4, 4, 4, 4, 4};

int led = 16;     // LED引脚
int button = 2; // 按键连接的引脚
int temp = 0;    // 用于读取按键状态的临时变量
int testled = 14;  // 用于测试


int pinDHT11 = 0;
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

// 光强
int BH1750address = 0x23; 
byte buff[2];
uint16_t vallx=0;

// 压强
Adafruit_BMP280 bme;
float dpresssure;

void mytone() {
    digitalWrite(led, HIGH);
    Serial.println("LED Turned ON, buzzer on");

    // iterate over the notes of the melody:
    for (int thisNote = 0; thisNote < 8; thisNote++) {

      // to calculate the note duration, take one second divided by the note type.
      //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
      int noteDuration = 1000 / noteDurations[thisNote];
      tone(led, melody[thisNote], noteDuration);

      // to distinguish the notes, set a minimum time between them.
      // the note's duration + 30% seems to work well:
      int pauseBetweenNotes = noteDuration * 1.30;
      delay(pauseBetweenNotes);
      // stop the tone playing:
      noTone(8);  
    }
}


void control()
{
  File file = SPIFFS.open("/index.html", "r");  
  size_t sent = server.streamFile(file, "text/html");  
  file.close();  

  if (server.arg("testledstatus")=="0") {
    digitalWrite(testled,LOW);
  } else if (server.arg("testledstatus")=="1") {
    digitalWrite(testled,HIGH);
  }

  if (server.arg("ledstatus")=="0") {
    digitalWrite(led,LOW);
  } else if (server.arg("ledstatus")=="1") {
    mytone();
    digitalWrite(led,HIGH);
  }

//  if (server.arg("wifiled")=="0") {
//    digitalWrite(wifimodeled,LOW);
//  } else if (server.arg("wifiled")=="1") {
//    digitalWrite(wifimodeled,HIGH);
//  }

}


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
  myhtml += vallx;
  myhtml += " lx";
    myhtml += "<br>";
  myhtml += "Pressure: ";
  myhtml += dpresssure;
  myhtml += " Pa";
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
  myhtml += "Lumen: ";
  myhtml += vallx;
  myhtml += " lx";
  server.send(200,"text/html",myhtml);

  if (server.arg("ledstatus")=="0") {
    digitalWrite(led,LOW);
  } else if (server.arg("ledstatus")=="1") {
    mytone();
    digitalWrite(led,HIGH);
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


// 光强函数
int BH1750_Read(int address) {

  int i=0;
  Wire.beginTransmission(address);
  Wire.requestFrom(address, 2);
  while(Wire.available()) {
    buff[i] = Wire.read();  // receive one byte
    i++;
  }

  Wire.endTransmission(); 
  return i;
}


// 光强函数
void BH1750_Init(int address) {

  Wire.beginTransmission(address);
  Wire.write(0x10);//1lx reolution 120ms
  Wire.endTransmission();
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

  // display setting
  u8g2.begin();
  u8g2.enableUTF8Print(); // enable UTF8 support for the print() function
  Serial.begin(115200);
  Serial.println("=================================");


  // Connect to WiFi network
  WiFiManager wifiManager;
  wifiManager.autoConnect("dragonfly");

  // SPIFFS 初始化
  SPIFFS.begin();  

  // server route
  server.on("/control",control);
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

  // 初始化 光强
  Wire.begin();

  // 初始化 压强
  bme.begin();

  // set pinmode
  pinMode(led, OUTPUT);   // LED设置为输入
  pinMode(button, INPUT); // 按键设置为输出
  pinMode(pinDHT11, OUTPUT); // 温度传感器设置为输入
  pinMode(testled, OUTPUT);   // LED设置为输入

}






void loop() {
  
  // 获取温度 湿度

  int err = SimpleDHTErrSuccess;
  digitalWrite(pinDHT11, 1);
  
  if ((err = dht11.read(&temperature, &humidity, NULL)) != SimpleDHTErrSuccess) {
    Serial.print("Read DHT11 failed, err="); 
    Serial.println(err);
//    return;
  }

//  Serial.print((int)temperature); Serial.print(" °C, "); 
//  Serial.print((int)humidity); Serial.println(" H");

// 光强取值
//  int lightsensorValue = analogRead(A0); // 读取模拟引脚A0上的输入
//  float voltage = lightsensorValue * (5.0 / 1023.0); // 将模拟读数（从0 - 1023变为0）转换为电压（0 - 5V）
//  Serial.println(lightsensorValue); // 打印出模拟口读取的值


// 光强 BH1750 GY-40
  int i;
//  uint16_t vallx=0;
  BH1750_Init(BH1750address);
  delay(200);
  if(2==BH1750_Read(BH1750address)) {
    vallx=((buff[0]<<8)|buff[1])/1.2;
    Serial.print(vallx,DEC);
    Serial.println(" lx");
  }

// 压强
  dpresssure = bme.readPressure();


  // 显示屏显示
  // 显示屏显示
  u8g2.clearBuffer();         // 清除内部缓冲区  
  u8g2.setFont(u8g2_font_wqy12_t_gb2312); // choose a suitable font u8g2_font_wqy12_t_gb2312 u8g2_font_ncenB08_tr

  // 显示温度 湿度
  u8g2.setCursor(0,10); // display temerature
  u8g2.print("温度/湿度: ");
  u8g2.print((int)temperature);
  u8g2.print("°C/");
  u8g2.print((int)humidity);
  u8g2.print("%");

// 光强显示
//  u8g2.setCursor(0,60); // 光强
//  u8g2.print("光强: ");
//  u8g2.print(lightsensorValue);
//  u8g2.print(" xxx");

  // 显示 IP
  u8g2.setCursor(0,20); // IP
  u8g2.print("IP:");
  u8g2.print(WiFi.localIP());
  u8g2.print(" 流明:");
  u8g2.print(vallx);
  u8g2.print("lx");

  // 显示时间
  time_t now;
  struct tm * timeinfo;
  time(&now);
  timeinfo = localtime(&now); 
  u8g2.setCursor(0,30); // date time
  u8g2.print(timeinfo->tm_year + 1900);
  u8g2.print("-");
  u8g2.print(toe(timeinfo->tm_mon));
  u8g2.print("-");
  u8g2.print(toe(timeinfo->tm_mday));
  u8g2.print(" ");
  u8g2.print(toe(timeinfo->tm_hour));
  u8g2.print(":");
  u8g2.print(toe(timeinfo->tm_min));
  u8g2.print(":");
  u8g2.print(toe(timeinfo->tm_sec));

  u8g2.setCursor(0,40); // date time
  u8g2.print("星期");
  u8g2.print(timeinfo->tm_wday);
  u8g2.print(" ");
  u8g2.print("今年第");
  u8g2.print(timeinfo->tm_yday);
  u8g2.print("天");

  // 显示 压强
  u8g2.setCursor(0,50); // 
  u8g2.print("压强:");
//  u8g2.print(bme.readPressure());
  u8g2.print(dpresssure);
  u8g2.print(" Pa/");
  u8g2.setCursor(0,60); // 
  u8g2.print("温度:");
  u8g2.print(bme.readTemperature());
  u8g2.print("°C");

  // 发送显示数据
  u8g2.sendBuffer();          // transfer internal memory to the display

    Serial.print("Temperature = ");
    Serial.print(bme.readTemperature());
    Serial.println(" *C");
    
    Serial.print("Pressure = ");
    Serial.print(bme.readPressure());
    Serial.println(" Pa");
  // wait for http client
  server.handleClient();


  // button control buzzer
  temp = digitalRead(button);
  if (temp == LOW) {
      mytone();
  }

  delay(1000);
}

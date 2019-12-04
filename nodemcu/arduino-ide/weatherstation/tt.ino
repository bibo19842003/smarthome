#include <SimpleDHT.h>
#include <Arduino.h>
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
#include "Free_Fonts.h" // Include the header file attached to this sketch
#include <TFT_eSPI.h> // Hardware-specific library ILI9488-driver
#include <SPI.h>


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


// DHT11: https://blog.bbskali.cn/index.php/archives/1337/


// notes in the melody:
int melody[] = {NOTE_C4, NOTE_G3, NOTE_G3, NOTE_A3, NOTE_G3, 0, NOTE_B3, NOTE_C4};
// note durations: 4 = quarter note, 8 = eighth note, etc.:
int noteDurations[] = {4, 8, 8, 4, 4, 4, 4, 4};

int temp = 0;
int wifibutton = 2;  //wifibutton

int wifiled = 16;     // LED引脚

int pinDHT11 = 12; //用于dht11 温湿度
SimpleDHT11 dht11(pinDHT11);


// Create an instance of the server
// specify the port to listen on as an argument
ESP8266WebServer server;
const char HTTP_HEADER_NEW[] PROGMEM          = "<!DOCTYPE html><html lang=\"en\"><head><meta charset=\"UTF-8\"><meta name=\"viewport\" content=\"width=device-width, initial-scale=1, user-scalable=no\"/><meta http-equiv=\"refresh\" content=\"5\"><title>SmartHome</title>";

// 温度 湿度
byte temperature = 0;
byte humidity = 0;

// 时间
int timezone = 8;
int dst = 0;

// 压强
Adafruit_BMP280 bme;
float dpresssure;

// tt7796 
TFT_eSPI tft = TFT_eSPI();   // Invoke custom library with default width and height
unsigned long drawTime = 0;

// 空气质量
int dustPin=A0;
float dustVal=0;
 
int ledPower=D8;
int delayTime=280;
int delayTime2=40;
float offTime=9680;
float air;
String air_a="Normal"; // 用于修正空气质量取负数时进行修正

// 空气质量
String airresult(){
 
  if(air <= 0) return air_a;  
  else if(0 < air <= 75){
    air_a="Perfect";
    return "Perfect";  
  }
  else if(75 < air <= 150){
    air_a="Very Good";
    return "Very Good";  
  }
  else if(150 < air <= 300){
    air_a="Good";
    return "Good";  
  }
  else if(300 < air <= 1050){
    air_a="Normal";
    return "Normal"; 
  }
  else if(1050 < air <= 3000){
    air_a="Poor";
    return "Poor"; 
  }
  else if(3000 < air){
    air_a="Very Poor";
    return "Very Poor"; 
  }
}


void blink1000(){
  digitalWrite(wifiled, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1000);                       // wait for a second
  digitalWrite(wifiled, LOW);    // turn the LED off by making the voltage LOW
  delay(1000);                       // wait for a second  
}


void control()
{
  File file = SPIFFS.open("/index.html", "r");  
  size_t sent = server.streamFile(file, "text/html");  
  file.close();  
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
  myhtml += "Pressure: ";
  myhtml += dpresssure;
  myhtml += " Pa";
  myhtml += "<br>";
  myhtml += "Air: ";
  myhtml += airresult(); 
  myhtml += FPSTR(HTTP_END);
  
  
  server.send(200,"text/html",myhtml);
}


void nocss()
{
//  String myhtml ="<html><head><title>Smart Home</title></head><body><h1>Test Smarthome<h1><form>test led:<button type=\"submit\" name=\"testledstatus\"value=\"0\">off</button><button type=\"submit\" name=\"testledstatus\" value=\"1\">on</button></form><form>buzzer and led:<button type=\"submit\" name=\"ledstatus\" value=\"0\">off</button><button type=\"submit\" name=\"ledstatus\" value=\"1\">on</button></form><form>Wifi mode:<button type=\"submit\" name=\"wifiled\" value=\"0\">STA</button><button type=\"submit\" name=\"wifiled\" value=\"1\">STAAP</button></form></body></html>";
  String myhtml ="<html><head><style>font-size:20px;} .button {font-size: 16px;padding:15px 32px;border-radius: 16px;} </style><title>Smart Home</title></head><body><h1>Smart Home</h1><form>Led :<button class=\"button\" type=\"submit\" name=\"ledstatus\" value=\"0\">off</button><button class=\"button\" type=\"submit\" name=\"ledstatus\" value=\"1\">on</button></form></body></html>";

  myhtml += "Temperature: ";
  myhtml += temperature;
  myhtml += " C";
  myhtml += "<br>";
  myhtml += "Humidity: ";
  myhtml += humidity;
  myhtml += " %";
  myhtml += "<br>";
  myhtml += "Pressure: ";
  myhtml += dpresssure;
  myhtml += " Pa";
  myhtml += "<br>";
  myhtml += "Air: ";
  myhtml += airresult(); 

  server.send(200,"text/html",myhtml);

  if (server.arg("ledstatus")=="0") 
    {digitalWrite(wifiled,LOW);} 
  else if (server.arg("ledstatus")=="1") 
    {digitalWrite(wifiled,HIGH);}
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


// 用于数字两位显示，如果是一位数字，那么首位为0，
String toemon(int xxx) {
  // 对于 month 计算有偏差，+1 用于调整偏差
  if (xxx < 12){
    xxx = xxx + 1;
  }
  else{
    xxx = 1;
  }

  // 用0补位
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


void clearwifiinfor() {
  Serial.println("it will clear password!!!");
  WiFiManager wifiManager;
  wifiManager.resetSettings();
  Serial.println("it had cleared password!!!");
}



// Print the header for a display screen
void header(const char *string)
{
  tft.setTextSize(1);
  tft.setTextColor(TFT_DARKGREY, TFT_WHITE);
  tft.fillRect(0, 0, 320, 30, TFT_WHITE);
  tft.setTextDatum(TC_DATUM);
  tft.drawString(string, 20, 3, 4); // Font 4 for fast drawing with background
}





void setup() {

   // set pinmode
  pinMode(wifiled, OUTPUT);   // LED设置为输入
  pinMode(pinDHT11, OUTPUT); // 温度传感器设置为输入
  pinMode(wifibutton, INPUT); // wifi重置
  pinMode(ledPower,OUTPUT);  // 空气质量
  pinMode(dustPin, INPUT);

  // Connect to WiFi network
  WiFiManager wifiManager;
  wifiManager.autoConnect("dragonfly");
  
  // wifi 状态显示
  Serial.println();
  Serial.println("it needed connect wifi");
  while (WiFi.status() != WL_CONNECTED) {
    blink1000();
    Serial.print(F("."));
  }
  Serial.print("AP IP address :");
  Serial.println(WiFi.localIP());
  // wifi connected , led on.
  digitalWrite(wifiled,HIGH);

  // display setting
  tft.begin();
  tft.setRotation(0);
  Serial.begin(115200);
  Serial.println("=================================");

  // SPIFFS 初始化
  SPIFFS.begin();  

  // server route
  //server.on("/control",control);
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

  // 初始化 压强
  bme.begin();

}






void loop() {
  
  // 获取温度 湿度
  int err = SimpleDHTErrSuccess;
  digitalWrite(pinDHT11, 1);
  
  if ((err = dht11.read(&temperature, &humidity, NULL)) != SimpleDHTErrSuccess) {
    Serial.print("Read DHT11 failed, err="); Serial.println(err);delay(1000);
    return;
  }

//  Serial.print((int)temperature); Serial.print(" °C, "); 
//  Serial.print((int)humidity); Serial.println(" H");


// 压强
  dpresssure = bme.readPressure();



// 1111111111111111111111
  int xpos = 0;
  int ypos = 10;

  // >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
  // Select different fonts to draw on screen using the print class
  // >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

  // tft.fillScreen(TFT_DARKGREY); // Clear screen to navy background TFT_DARKGREY TFT_NAVY
  tft.fillScreen(random(0xFFFF));

  header("Hello Dragonfly!!!");
  
//  tft.setFreeFont(FSB12);       // Select Free Serif 15 point font

  // For comaptibility with Adafruit_GFX library the text background is not plotted when using the print class
  // even if we specify it.
  tft.setTextColor(TFT_WHITE, TFT_DARKGREY);
  tft.setCursor(xpos, ypos);    // Set cursor near top left corner of screen
 // tft.setTextFont(GLCD);     // Select the orginal small GLCD font by using NULL or GLCD
//  tft.println();             // Move cursor down a line

  // 显示时间
  tft.setTextColor(TFT_YELLOW); 
  tft.setTextFont(4);
  time_t now;
  struct tm * timeinfo;
  time(&now);
  timeinfo = localtime(&now); 
  //Serial.print(time(&now));
  tft.println();
  tft.print(timeinfo->tm_year + 1900);
  tft.print("-");
  tft.print(toemon(timeinfo->tm_mon));
  tft.print("-");
  tft.print(toe(timeinfo->tm_mday));
  tft.print(" ");
  tft.print(toe(timeinfo->tm_hour));
  tft.print(":");
  tft.print(toe(timeinfo->tm_min));
//  tft.print(":");
//  tft.print(toe(timeinfo->tm_sec));

  tft.println();
//  tft.print("星期");
  tft.print("Week ");
  tft.print(timeinfo->tm_wday);
//  tft.print("今年第");
  tft.print("; Days ");
  tft.print(timeinfo->tm_yday);
//  tft.print("天");


  // 显示温度 湿度  
  tft.println();                // Move cursor down a line
  tft.setTextColor(TFT_RED,TFT_BLACK);    
  tft.setTextFont(4);
  //tft.print("温度/湿度: ");
  tft.print("Temperature: ");
  tft.print((int)temperature);
  tft.print("°C");
  tft.println();                // Move cursor down a line
  tft.print("Humidity: ");
  tft.print((int)humidity);
  tft.print("%");


  // 显示 IP
  tft.setTextColor(TFT_GREEN,TFT_BLACK);
  tft.setTextFont(4);
  tft.println(); 
  tft.print("IP:");
  tft.print(WiFi.localIP());




  // 显示 压强
  tft.setTextColor(TFT_BLUE);    
  tft.setTextFont(4);
  tft.println();
//  tft.print("压强:");
//  u8g2.print(bme.readPressure());
  tft.print("Pressure: ");
  tft.print(dpresssure);
  tft.print(" Pa");
  tft.println();
//  tft.print("温度:");
  tft.print("Temperature:");
  tft.print(bme.readTemperature());
//  tft.print("°C");
  tft.print("C");


  // 显示空气质量
  digitalWrite(ledPower,LOW); 
  delayMicroseconds(delayTime);
  dustVal=analogRead(dustPin); 
  delayMicroseconds(delayTime2);
  digitalWrite(ledPower,HIGH); 
  delayMicroseconds(offTime);

  air = (float(dustVal/1024)-0.0356)*120000*0.035;
  //if (dustVal>36.455)
  // Serial.println((float(dustVal/1024)-0.0356)*120000*0.035);

  // 3000 + = 很差
  // 1050-3000 = 差
  // 300-1050 = 一般
  // 150-300 = 好
  // 75-150 = 很好
  // 0-75 = 非常好
  // <0 :等待 ---


  tft.setTextColor(TFT_YELLOW);    
  tft.setTextFont(4);
  tft.println();
  tft.print("Air: ");
  tft.print(airresult());
  



  // wait for http client
  server.handleClient();

  temp = digitalRead(wifibutton);
  if (temp == LOW) {
    clearwifiinfor();
  }

  delay(5000);
}

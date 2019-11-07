#include <SimpleDHT.h>
#include <Arduino.h>
#include <U8g2lib.h>
#include "pitches.h"
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif


// pin 使用说明：
// D0(16): led buzzer 信号
// D1(5): SCL
// D2(4): SDA
// D3(0): dht11 信号
// D4(2): button 信号
// D5(14): greed red only test 信号
// D6(12): WIFI MODE led (0: station; 1:stationap)
// D7(13): WIFI MODE button (0: station; 1:stationap)


U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, /* clock=*/ SCL, /* data=*/ SDA, /* reset=*/ U8X8_PIN_NONE);   // All Boards without Reset of the Display

// for DHT11, 
//      VCC: 5V or 3V
//      GND: GND
//      DATA: 2
// https://blog.bbskali.cn/index.php/archives/1337/

// u8g2: https://blog.csdn.net/dpjcn1990/article/details/92831760

// NTP: https://lastminuteengineers.com/esp8266-ntp-server-date-time-tutorial/  https://github.com/arduino-libraries/NTPClient


// notes in the melody:
int melody[] = {NOTE_C4, NOTE_G3, NOTE_G3, NOTE_A3, NOTE_G3, 0, NOTE_B3, NOTE_C4};
// note durations: 4 = quarter note, 8 = eighth note, etc.:
int noteDurations[] = {4, 8, 8, 4, 4, 4, 4, 4};

int led = 16;     // LED引脚
int button = 2; // 按键连接的引脚
int temp = 0;    // 用于读取按键状态的临时变量
int testled = 14;  // 用于测试
int wifimodeled = 12;     // wifimode LED引脚
int wifimodebutton = 13;     // wifimode LED引脚
int tempwifimode = 0; // 用于读取按键状态的临时变量

int pinDHT11 = 0;
SimpleDHT11 dht11(pinDHT11);


#ifndef STASSID
#define STASSID "CMCC-jRtE"
#define STAPSK  "gdky6zx6"
#endif

const char* ssid = STASSID;
const char* password = STAPSK;

// Create an instance of the server
// specify the port to listen on as an argument
ESP8266WebServer server;


// Define NTP Client to get time
const long utcOffsetInSeconds = 28800; // 3600 * 8
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);


  byte temperature = 0;
  byte humidity = 0;

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
  String myhtml ="<html><head><title>Test Smarthome</title></head><body><h1>Test Smarthome<h1><form>test led:<button type=\"submit\" name=\"testledstatus\"value=\"0\">off</button><button type=\"submit\" name=\"testledstatus\" value=\"1\">on</button></form><form>buzzer and led:<button type=\"submit\" name=\"ledstatus\" value=\"0\">off</button><button type=\"submit\" name=\"ledstatus\" value=\"1\">on</button></form><form>Wifi mode:<button type=\"submit\" name=\"wifiled\" value=\"0\">STA</button><button type=\"submit\" name=\"wifiled\" value=\"1\">STAAP</button></form></body></html>";
  myhtml += "Temperature: ";
  myhtml += temperature;
  myhtml += " C";
  myhtml += "<br>";
  myhtml += "Humidity: ";
  myhtml += humidity;
  myhtml += " %";

  server.send(200,"text/html",myhtml);

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

  if (server.arg("wifiled")=="0") {
    digitalWrite(wifimodeled,LOW);
  } else if (server.arg("wifiled")=="1") {
    digitalWrite(wifimodeled,HIGH);
  }
}


void blink(){
  digitalWrite(wifimodeled, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1000);                       // wait for a second
  digitalWrite(wifimodeled, LOW);    // turn the LED off by making the voltage LOW
  delay(1000);                       // wait for a second  
}




void setup() {

  // display setting
  u8g2.begin();
  u8g2.enableUTF8Print(); // enable UTF8 support for the print() function
  Serial.begin(9600);
  Serial.println("=================================");


  // Connect to WiFi network
  // Connect to WiFi network
  Serial.println();
  Serial.print(F("Connecting to "));
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(F("."));
  }
  Serial.println();
  Serial.println(F("WiFi connected"));

  // server route
  server.on("/control",control);
  server.on("/",control);
  
  // Start the server
  server.begin();
  Serial.println(WiFi.localIP());


  // connect ntp server
  timeClient.begin();


  // set pinmode
  pinMode(led, OUTPUT);   // LED设置为输入
  pinMode(button, INPUT); // 按键设置为输出
  pinMode(pinDHT11, OUTPUT); // 温度传感器设置为输入
  pinMode(testled, OUTPUT);   // LED设置为输入
  pinMode(wifimodeled, OUTPUT);   // wifimode 设置为输入
  pinMode(wifimodebutton, INPUT); // 按键设置为输出

}
















void loop() {
  
  // 获取温度 湿度

  int err = SimpleDHTErrSuccess;
  digitalWrite(pinDHT11, HIGH);

  
  if ((err = dht11.read(&temperature, &humidity, NULL)) != SimpleDHTErrSuccess) {
    Serial.print("Read DHT11 failed, err="); Serial.println(err);delay(1000);
    return;
  }

//  Serial.print((int)temperature); Serial.print(" °C, "); 
//  Serial.print((int)humidity); Serial.println(" H");

// 光强取值
//  int lightsensorValue = analogRead(A0); // 读取模拟引脚A0上的输入
//  float voltage = lightsensorValue * (5.0 / 1023.0); // 将模拟读数（从0 - 1023变为0）转换为电压（0 - 5V）
//  Serial.println(lightsensorValue); // 打印出模拟口读取的值

  // 获取时间
  timeClient.update();

//  Serial.print(daysOfTheWeek[timeClient.getDay()]);
//  Serial.print(", ");
//  Serial.print(timeClient.getHours());
//  Serial.print("：");
//  Serial.print(timeClient.getMinutes());
//  Serial.print("：");
//  Serial.println(timeClient.getSeconds());


  // 显示屏显示
  // 显示屏显示
  u8g2.clearBuffer();         // 清除内部缓冲区  
  u8g2.setFont(u8g2_font_wqy12_t_gb2312); // choose a suitable font u8g2_font_wqy12_t_gb2312 u8g2_font_ncenB08_tr

  // 显示温度 湿度
  u8g2.setCursor(0,10); // display temerature
  u8g2.print("温度: ");
  u8g2.print((int)temperature);
  u8g2.print("°C");
  
  u8g2.setCursor(0,25); // display humidity
  u8g2.print("湿度: ");
  u8g2.print((int)humidity);
  u8g2.print("%");

// 光强显示
//  u8g2.setCursor(0,60); // 光强
//  u8g2.print("光强: ");
//  u8g2.print(lightsensorValue);
//  u8g2.print(" xxx");

  // 显示 IP
  u8g2.setCursor(0,40); // IP
  u8g2.print("IP: ");
  u8g2.print(WiFi.localIP());

  // 显示时间
  u8g2.setCursor(0,55); // IP
  u8g2.print(daysOfTheWeek[timeClient.getDay()]);
  u8g2.print(" , ");
  u8g2.print(timeClient.getHours());
  u8g2.print(":");
  u8g2.print(timeClient.getMinutes());
  u8g2.print(":");
  u8g2.print(timeClient.getSeconds());

  // 发送显示数据
  u8g2.sendBuffer();          // transfer internal memory to the display


  // wait for http client
  server.handleClient();


  // button control buzzer
  temp = digitalRead(button);
  if (temp == LOW) {
      mytone();
  }


  // wifi mode change , default station (0); change to stationap (1), after setting it need to reset.
  tempwifimode = digitalRead(wifimodebutton);
  if (tempwifimode == LOW) {
      blink();
  } 
//  delay(2000);

}

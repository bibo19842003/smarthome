#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <FS.h>
#include <DNSServer.h>
#include <WiFiManager.h>


// pin 使用说明：
// D0(16): led buzzer 信号 待用
// D1(5): 信号灯 红
// D2(4): 信号灯 黄
// D3(0): 信号灯 绿
// D4(2): wifi 重置灯
// D5(14): 信号灯按钮 红
// D6(12): 信号灯按钮 黄
// D7(13): 信号灯按钮 绿
// D8(15): wifi 重置按钮


int button_r = 14;     // 信号灯按钮 红
int button_y = 12; // 信号灯按钮 黄
int button_g = 13;    // 信号灯按钮 绿
int button_wifi = 15;  // wifi 灯按钮

int led_r = 5;     // 信号灯 红
int led_y = 4; // 信号灯 黄
int led_g = 0;    // 信号灯 绿
int led_wifi = 2;  // wifi 灯

int temp_r = 0;
int temp_y = 0;
int temp_g = 0;
int temp_wifi = 0;

int old_r = 0;
int old_y = 0;
int old_g = 0;

unsigned long btn_time_s = 0;
unsigned long btn_time_e = 0;
unsigned long btn_time_dur = 0;
int wifi_status_old = 0;
int wifi_pin_lh = 0;


// Create an instance of the server
// specify the port to listen on as an argument
ESP8266WebServer server;
const char HTTP_HEADER_NEW[] PROGMEM          = "<!DOCTYPE html><html lang=\"en\"><head><meta charset=\"UTF-8\"><meta name=\"viewport\" content=\"width=device-width, initial-scale=1, user-scalable=no\"/><meta http-equiv=\"refresh\" content=\"5\"><title></title>";



//void blink(){
//  digitalWrite(wifimodeled, HIGH);   // turn the LED on (HIGH is the voltage level)
//  delay(1000);                       // wait for a second
////  digitalWrite(wifimodeled, LOW);    // turn the LED off by making the voltage LOW
//  delay(1000);                       // wait for a second  
//}

void blink_3s(){
  for (int i=0;i<3;i++){
    digitalWrite(led_r,0);
    digitalWrite(led_y,0);
    digitalWrite(led_g,0);
    delay(500);
    digitalWrite(led_r,1);
    digitalWrite(led_y,1);
    digitalWrite(led_g,1);
    delay(500);
  }
}


void all_red_light_3s(){
  digitalWrite(led_r,1);
  digitalWrite(led_y,1);
  digitalWrite(led_g,1);
  delay(3000);
}


void valsensor()
{
  // FPSTR from <WiFiManager.h>
  String myhtml = FPSTR(HTTP_HEADER_NEW);
  myhtml += FPSTR(HTTP_SCRIPT);
  myhtml += FPSTR(HTTP_STYLE);
  myhtml += FPSTR(HTTP_HEADER_END);
  myhtml += FPSTR(HTTP_END);
  
  
  server.send(200,"text/html",myhtml);
}


void nocss()
{
//  String myhtml ="<html><head><title>Smart Home</title></head><body><h1>Test Smarthome<h1><form>test led:<button type=\"submit\" name=\"testledstatus\"value=\"0\">off</button><button type=\"submit\" name=\"testledstatus\" value=\"1\">on</button></form><form>buzzer and led:<button type=\"submit\" name=\"ledstatus\" value=\"0\">off</button><button type=\"submit\" name=\"ledstatus\" value=\"1\">on</button></form><form>Wifi mode:<button type=\"submit\" name=\"wifiled\" value=\"0\">STA</button><button type=\"submit\" name=\"wifiled\" value=\"1\">STAAP</button></form></body></html>";
  String myhtml ="<html><head><title>Smart Home</title></head><body><h1>Smart Home<h1><form>Red led:<button type=\"submit\" name=\"rledstatus\" value=\"0\">off</button><button type=\"submit\" name=\"rledstatus\" value=\"1\">on</button></form></body></html>";

  myhtml += "<form>Yellow led:<button type=\"submit\" name=\"yledstatus\" value=\"0\">off</button><button type=\"submit\" name=\"yledstatus\" value=\"1\">on</button></form>";
  myhtml += "<form>Green led:<button type=\"submit\" name=\"gledstatus\" value=\"0\">off</button><button type=\"submit\" name=\"gledstatus\" value=\"1\">on</button></form>";
  myhtml += "<form>All led:<button type=\"submit\" name=\"allledstatus\" value=\"0\">off</button><button type=\"submit\" name=\"allledstatus\" value=\"1\">on</button></form>";

  server.send(200,"text/html",myhtml);

  if (server.arg("rledstatus")=="0") {
    old_r = 0;
    digitalWrite(led_r,old_r);
//    Serial.println("aaaaaaaaaaaaaaa");
  } else if (server.arg("rledstatus")=="1") {
    old_r = 1;
    digitalWrite(led_r,old_r);
//    Serial.println("bbbbbbbbbbbb");
  }

  if (server.arg("yledstatus")=="0") {
    old_y = 0;
    digitalWrite(led_y,old_y);
  } else if (server.arg("yledstatus")=="1") {
    old_y = 1;
    digitalWrite(led_y,old_y);
  }

  if (server.arg("gledstatus")=="0") {
    old_g = 0;
    digitalWrite(led_g,old_g);
  } else if (server.arg("gledstatus")=="1") {
    old_g = 1;
    digitalWrite(led_g,old_g);
  }

  if (server.arg("allledstatus")=="0") {
    old_r = 0;
    old_y = 0;
    old_g = 0;
  } else if (server.arg("allledstatus")=="1") {
    old_r = 1;
    old_y = 1;
    old_g = 1;
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


  // set pinmode
  pinMode(led_r, OUTPUT);   // LED设置为输出
  pinMode(led_y, OUTPUT);   // LED设置为输出
  pinMode(led_g, OUTPUT);   // LED设置为输出
  pinMode(led_wifi, OUTPUT);   // LED设置为输出
  pinMode(button_r, INPUT); // 按钮输入
  pinMode(button_y, INPUT); // 按钮输入
  pinMode(button_g, INPUT); // 按钮输入
  pinMode(button_wifi, INPUT); // 按钮输入

}





void loop() {

  // wait for http client
  server.handleClient();


  temp_r = digitalRead(button_r);
  temp_y = digitalRead(button_y);
  temp_g = digitalRead(button_g);
  temp_wifi = digitalRead(button_wifi);
  
  if (temp_r == HIGH) {
    old_r = !old_r;
    digitalWrite(led_r, old_r);
//    Serial.println("RED LED change ");
  }
  else {
    digitalWrite(led_r, old_r);
//    Serial.println("RED LED remain");
  }

  if (temp_y == HIGH) {
    old_y = !old_y;
    digitalWrite(led_y, old_y);
//    Serial.println("YELLOW LED change");
  }
  else {
    digitalWrite(led_y, old_y);
//    Serial.println("YELLOW LED remain");
  }

  if (temp_g == HIGH) {
    old_g = !old_g;
    digitalWrite(led_g, old_g);
//    Serial.println("GREEN LED change");
  }
  else {
    digitalWrite(led_g, old_g);
//    Serial.println("GREEN LED remain");
  }


// wifi 按钮超过5s清除密码
  if (temp_wifi == HIGH) {
    wifi_pin_lh = 1;
    if (wifi_status_old == 0){
      wifi_status_old =1;
      btn_time_s = millis();
    }
  }
  else {
    if (wifi_pin_lh ==1) {
      btn_time_e = millis();
      btn_time_dur = btn_time_e - btn_time_s;

      if (btn_time_dur > 5000) {
        Serial.println("it is clearing wifi passwd now !!!");
        Serial.println("it is clearing wifi passwd now !!!");
        Serial.println("it is clearing wifi passwd now !!!");
        Serial.println(btn_time_dur);
        blink_3s();
        clearwifipass();
        all_red_light_3s();
        //Serial.println("it will restart");
        //ESP.restart();
        //ESP.reset();
      }
    }
    wifi_status_old = 0;
    wifi_pin_lh =0;
  }

  delay(350);
}

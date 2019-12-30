
#include <ESP8266WiFi.h>
#include <WiFiManager.h>
#include <ESP8266mDNS.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>


// pin 使用说明：
// D0(16): 
// D1(5): m11
// D2(4): m12
// D3(0): m21
// D4(2):m22
// D5(14): m31
// D6(12): m32
// D7(13): m41
// D8(15): m42



// 各种变量定义
// 各种变量定义 - begin

// mdns name 定义
// String mdnsname = "dragonfly-" + String(ESP.getChipId()); //dragonfly-7914686.local
  String mdnsname = "car";

// http web page ,FPSTR from <WiFiManager.h>
const char HTTP_HEADER_NEW[] PROGMEM          = "<!DOCTYPE html><html lang=\"en\"><head><meta charset=\"UTF-8\"><meta name=\"viewport\" content=\"width=device-width, initial-scale=1, user-scalable=no\"/><title></title>";
const char HTTP_STYLE_NEW[] PROGMEM           = "<style>.c{text-align: center;} div,input{padding:5px;font-size:1em;} input{width:95%;} body{text-align: center;font-family:verdana;} button{border:0;border-radius:0.3rem;background-color:#1fa3ec;color:#fff;line-height:2.4rem;font-size:1.2rem;width:130px;} .q{float: right;width: 64px;text-align: right;} </style>";
const char HTTP_SCRIPT_NEW[] PROGMEM          = "<script>function c(l){document.getElementById('s').value=l.innerText||l.textContent;document.getElementById('p').focus();}</script>";
const char HTTP_HEADER_END_NEW[] PROGMEM        = "</head><body><div style='text-align:center;display:inline-block;min-width:260px;'>";
const char HTTP_END_NEW[] PROGMEM             = "</div></body></html>";


// 电机控制线
int m11=D1;
int m12=D2;
int m21=D3;
int m22=D4;
int m31=D5;
int m32=D6;
int m41=D7;
int m42=D8;

// 各种变量定义 - end


// ***********************************


// 各种服务定义
// 各种服务定义 - begin

// TCP server at port 80 will respond to HTTP requests
ESP8266WebServer server;



// 各种服务定义 - end

// ***********************************



// motor motor motor motor motor motor motor motor motor
// 单轮待机
void mwait(int in1, int in2){
  digitalWrite(in1,0);
  digitalWrite(in2,0);
}

// 单轮刹车
void mstop(int in1, int in2){
  digitalWrite(in1,1);
  digitalWrite(in2,1);
}

// 单轮正转最快
void mforword(int in1, int in2){
  digitalWrite(in1,1);
  digitalWrite(in2,0);
}

// 单轮反转最快
void mreversion(int in1, int in2){
  digitalWrite(in1,0);
  digitalWrite(in2,1);
}

// 单轮正转 - 加速
// s 模拟数值 0 - 1024
void mfa(int in1, int in2){
  int s;
  for(s=0;s<1024;s=s+10){
    analogWrite(in1,s);
    digitalWrite(in2,0);
    delay(50);
  }
}

// 单轮反转 - 加速
// s 模拟数值 0 - 1024
void mra(int in1, int in2){
  int s;
  for(s=1024;s>0;s=s-10){
    analogWrite(in1,0);
    digitalWrite(in2,s);
    delay(50);
  }
}



// car car car car car car car car car car car car car car car car car car car car
// car forword  up
void carforword(){
  carwait();
  delay(500);
  mforword(m11, m12);
  mforword(m21, m22);
  mforword(m31, m32);
  mforword(m41, m42);
}



// car reversion down
void carreversion(){
  carwait();
  delay(500);
  mreversion(m11, m12);
  mreversion(m21, m22);
  mreversion(m31, m32);
  mreversion(m41, m42);
}



// car forword turn left , left up
void carforwordleft(){
  carwait();
  delay(500);
  mforword(m11, m12);
  mforword(m21, m22);
  mreversion(m31, m32);
  mforword(m41, m42);

  delay(1000);
  mwait(m31, m32);
  mforword(m31, m32);
  
}



// car forword turn right, right up
void carforwordright(){
  carwait();
  delay(500);
  mreversion(m11, m12);
  mforword(m21, m22);
  mforword(m31, m32);
  mforword(m41, m42);

  delay(1000);
  mwait(m11, m12);
  mforword(m11, m12);
}


// car eversion turn left, left down
void carreversionleft(){
  carwait();
  delay(500);
  mreversion(m11, m12);
  mreversion(m21, m22);
  mreversion(m31, m32);
  mforword(m41, m42);

  delay(1000);
  mwait(m41, m42);
  mreversion(m41, m42);
}



// car eversion turn right, right down
void carreversionright(){
  carwait();
  delay(500);
  mreversion(m11, m12);
  mforword(m21, m22);
  mreversion(m31, m32);
  mreversion(m41, m42);

  delay(1000);
  mwait(m21, m22);
  mreversion(m21, m22);
}



// car stop
void carstop(){
  carwait();
  delay(500);
  mstop(m11, m12);
  mstop(m21, m22);
  mstop(m31, m32);
  mstop(m41, m42);
}



// car wait
void carwait(){
  mwait(m11, m12);
  mwait(m21, m22);
  mwait(m31, m32);
  mwait(m41, m42);
}




void test(){
  // 正转最快
//  mforword(m31, m32);
//  delay(3000);
  
// 待机
//  mwait(m31, m32);
//  delay(3000);

  // 刹车
//  mstop(m31, m32);
//  delay(3000);

// 反转最快
//  mreversion(m31, m32);
//  delay(3000);

  // 待机
 // mwait(m31, m32);
//  delay(3000);
  
// 刹车
//  mstop(m31, m32);
//  delay(3000);

}




void indexpage()
{
  
 // String myhtml ="<html><head><title>Smart Home</title></head><body><h1>Smart Home<h1><form>buzzer and led:<button type=\"submit\" name=\"ledstatus\" value=\"0\">off</button><button type=\"submit\" name=\"ledstatus\" value=\"1\">on</button></form></body></html>";

  String myhtml = FPSTR(HTTP_HEADER_NEW);
  myhtml += FPSTR(HTTP_SCRIPT_NEW);
  myhtml += FPSTR(HTTP_STYLE_NEW);
  myhtml += FPSTR(HTTP_HEADER_END_NEW);

  myhtml += "<form>";
  
  myhtml += "<br/><br/>";
  myhtml += "<button type=\"submit\" name=\"car\" value=\"3\">Left Up</button>";
  myhtml += "<button type=\"submit\" name=\"car\" value=\"1\">Up</button>";
  myhtml += "<button type=\"submit\" name=\"car\" value=\"4\">Right Up</button><br/><br/><br/><br/><br/><br/>";
  myhtml += "<button type=\"submit\" name=\"car\" value=\"0\">Stop</button><br/><br/><br/><br/><br/><br/>";
  myhtml += "<button type=\"submit\" name=\"car\" value=\"5\">Left Down</button>";
  myhtml += "<button type=\"submit\" name=\"car\" value=\"2\">Down</button>";
  myhtml += "<button type=\"submit\" name=\"car\" value=\"6\">Right Down</button><br/><br/>";
  
  myhtml += "</form>";

  myhtml += FPSTR(HTTP_END_NEW);  
  server.send(200,"text/html",myhtml);
  
  if (server.arg("car")=="1") {
    Serial.println("qqqqqqqqqqqqqq");
    carforword();
  } else if (server.arg("car")=="2") {
    Serial.println("hhhhhhhhhhhhhhhhh");
    carreversion();
  } else if (server.arg("car")=="3") {
    Serial.println("zuo qian");
    carforwordleft();
  } else if (server.arg("car")=="4") {
    Serial.println("you qian");
    carforwordright();
  } else if (server.arg("car")=="5") {
    Serial.println("zuo hou");
    carreversionleft();
  } else if (server.arg("car")=="6") {
    Serial.println("right hou");
    carreversionright();
  } else if (server.arg("car")=="0") {
    Serial.println("stop stop");
    carstop();
  } else if (server.arg("car")=="") {
    Serial.println("xxxxxxxxxxxxxxxxxxxxxx");
    carstop();
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




// Power on setup
void setup() {

  Serial.begin(115200);
  Serial.println("=================================");

  pinMode(m11,OUTPUT);
  pinMode(m12,OUTPUT);
  pinMode(m21,OUTPUT);
  pinMode(m22,OUTPUT);
  pinMode(m31,OUTPUT);
  pinMode(m32,OUTPUT);
  pinMode(m41,OUTPUT);
  pinMode(m42,OUTPUT);

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
  MDNS.addService("car", "tcp", 8080); 

  // server route
  server.on("/",indexpage);
  server.on("/indexpage",indexpage);
  server.on("/carforword",carforword);
  server.on("/carreversion",carreversion);
  server.on("/carforwordleft",carforwordleft);
  server.on("/carforwordright",carforwordright);
  server.on("/carreversionleft",carreversionleft);
  server.on("/carreversionright",carreversionright);
  server.on("/carstop",carstop);
  server.on("/mdns",mdnsresponder);
  server.onNotFound(handleNotFound);
  
  // Start the server
  server.begin();
  Serial.println(WiFi.localIP());
}




// Main Program Loop
void loop() {
  
  server.handleClient();

  delay(1000);  

}

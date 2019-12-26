
#include <ESP8266WiFi.h>
#include <WiFiManager.h>
#include <ESP8266mDNS.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>


// ESP8266 Motor shield
// pin 使用说明：
// D0: 
// D1: 点击A转速
// D2: 点击B转速
// D3: 点击A方向
// D4: 点击B方向
// D5: 
// D6: 
// D7: 
// D8: 
// VM: 电机电源输入 4.5V~36V,可单独供电;
// VIN: 控制电源输入 4.5V~9V(10VMAX),可单独供电
// 短路 VM 和 VIN),可以方便地使用一路电源(必须 4.5V~9V)同时完成电机的驱动与控制;

// 各种变量定义
// 各种变量定义 - begin

// mdns name 定义
// String mdnsname = "dragonfly-" + String(ESP.getChipId()); //dragonfly-7914686.local
  String mdnsname = "car-2w-master";

// http web page ,FPSTR from <WiFiManager.h>
const char HTTP_HEADER_NEW[] PROGMEM          = "<!DOCTYPE html><html lang=\"en\"><head><meta charset=\"UTF-8\"><meta name=\"viewport\" content=\"width=device-width, initial-scale=1, user-scalable=no\"/><title></title>";
const char HTTP_STYLE_NEW[] PROGMEM           = "<style>.c{text-align: center;} div,input{padding:5px;font-size:1em;} input{width:95%;} body{text-align: center;font-family:verdana;} button{border:0;border-radius:0.3rem;background-color:#1fa3ec;color:#fff;line-height:2.4rem;font-size:1.2rem;width:130px;} .q{float: right;width: 64px;text-align: right;} </style>";
const char HTTP_SCRIPT_NEW[] PROGMEM          = "<script>function c(l){document.getElementById('s').value=l.innerText||l.textContent;document.getElementById('p').focus();}</script>";
const char HTTP_HEADER_END_NEW[] PROGMEM        = "</head><body><div style='text-align:center;display:inline-block;min-width:260px;'>";
const char HTTP_END_NEW[] PROGMEM             = "</div></body></html>";


// 电机控制线
int mas=D1;
int mbs=D2;
int mad=D3;
int mbd=D4;

// 各种变量定义 - end


// ***********************************


// 各种服务定义
// 各种服务定义 - begin

// TCP server at port 80 will respond to HTTP requests
ESP8266WebServer server;



// 各种服务定义 - end

// ***********************************



// car car car car car car car car car car car car car car car car car car car car
// car carforword  up
void carforword(){
  // delay(500);
  digitalWrite(mad,1);
  digitalWrite(mbd,1);
  // analogWrite(mas,1024)
  digitalWrite(mas,1);
  digitalWrite(mbs,1);
}



// car carreversion down
void carreversion(){
  digitalWrite(mad,0);
  digitalWrite(mbd,0);
  digitalWrite(mas,1);
  digitalWrite(mbs,1);
}



// car counterclockwise
void counterclockwise(){
  digitalWrite(mad,0);
  digitalWrite(mbd,1);
  digitalWrite(mas,1);
  digitalWrite(mbs,1);
  delay(300);
  carforword();
}



// car clockwise
void clockwise(){
  digitalWrite(mad,1);
  digitalWrite(mbd,0);
  digitalWrite(mas,1);
  digitalWrite(mbs,1);
  delay(300);
  carforword();
}


// car carstop
void carstop(){
  digitalWrite(mas,0);
  digitalWrite(mbs,0);
}


// car carforword  up little
void carforwordlittle(){
  // delay(500);
  digitalWrite(mad,1);
  digitalWrite(mbd,1);
  digitalWrite(mas,1);
  digitalWrite(mbs,1);
  delay(100);
  carstop();
}



// car carreversion down little
void carreversionlittle(){
  digitalWrite(mad,0);
  digitalWrite(mbd,0);
  digitalWrite(mas,1);
  digitalWrite(mbs,1);
  delay(100);
  carstop();
}



// car counterclockwise little
void counterclockwiselittle(){
  digitalWrite(mad,0);
  digitalWrite(mbd,1);
  digitalWrite(mas,1);
  digitalWrite(mbs,1);
  delay(100);
  carstop();
}



// car clockwise little
void clockwiselittle(){
  digitalWrite(mad,1);
  digitalWrite(mbd,0);
  digitalWrite(mas,1);
  digitalWrite(mbs,1);
  delay(100);
  carstop();
}


// car counterclockwise little
void counterclockwiselittleforword(){
  digitalWrite(mad,0);
  digitalWrite(mbd,1);
  digitalWrite(mas,1);
  digitalWrite(mbs,1);
  delay(100);
  Serial.println("zuo zuo zuo and and and forword forword forword");
  carforword();
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
  myhtml += "<button type=\"submit\" name=\"car\" value=\"1\">carforword</button><br/><br/>";
  myhtml += "<button type=\"submit\" name=\"car\" value=\"3\">Left</button>";
  myhtml += "<button type=\"submit\" name=\"car\" value=\"5\">carstop</button>";
  myhtml += "<button type=\"submit\" name=\"car\" value=\"4\">Right</button><br/><br/>";
  myhtml += "<button type=\"submit\" name=\"car\" value=\"2\">carreversion</button>";

  myhtml += "<br/><br/><br/><br/>";
  myhtml += "<button type=\"submit\" name=\"car\" value=\"6\">carforword-L</button><br/><br/>";
  myhtml += "<button type=\"submit\" name=\"car\" value=\"8\">Left-L</button>";
  myhtml += "<button type=\"submit\" name=\"car\" value=\"5\">carstop</button>";
  myhtml += "<button type=\"submit\" name=\"car\" value=\"9\">Right-L</button><br/><br/>";
  myhtml += "<button type=\"submit\" name=\"car\" value=\"7\">carreversion-L</button>";

  myhtml += "<br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/>";
  myhtml += "<button type=\"submit\" name=\"car\" value=\"100\">counterclockwiselittleforword</button>";
  
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
    Serial.println("zuo zuo zuo");
    counterclockwise();
  } else if (server.arg("car")=="4") {
    Serial.println("you you you");
    clockwise();
  } else if (server.arg("car")=="5") {
    Serial.println("ttttttttttttttttt");
    carstop();
  } else if (server.arg("car")=="6") {
    Serial.println("qqqqqqqqqqqqqq -L-L-L");
    carforwordlittle();
  } else if (server.arg("car")=="7") {
    Serial.println("hhhhhhhhhhhhhhhhh -L-L-L");
    carreversionlittle();
  } else if (server.arg("car")=="8") {
    Serial.println("zuo zuo zuo -L-L-L");
    counterclockwiselittle();
  } else if (server.arg("car")=="9") {
    Serial.println("you you you -L-L-L");
    clockwiselittle();
  } else if (server.arg("car")=="100") {
    Serial.println("100   100   100");
    counterclockwiselittleforword();
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

  pinMode(mas,OUTPUT);
  pinMode(mbs,OUTPUT);
  pinMode(mad,OUTPUT);
  pinMode(mbd,OUTPUT);

  //pinMode(m31,OUTPUT);
  //pinMode(m32,OUTPUT);
  //pinMode(m41,OUTPUT);
  //pinMode(m42,OUTPUT);

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
  //MDNS.addService("car", "tcp", 8080); 

  // server route
  server.on("/",indexpage);
  server.on("/indexpage",indexpage);
  server.on("/carforword",carforword);
  server.on("/carreversion",carreversion);
  server.on("/counterclockwise",counterclockwise);
  server.on("/clockwise",clockwise);
  server.on("/carstop",carstop);

  server.on("/counterclockwiselittleforword",counterclockwiselittleforword);
  
  server.on("/mdns",mdnsresponder);
  server.onNotFound(handleNotFound);
  
  // Start the server
  server.begin();
  Serial.println(WiFi.localIP());
}




// Main Program Loop
void loop() {
  
  server.handleClient();

  MDNS.update();

  delay(1000);  

}

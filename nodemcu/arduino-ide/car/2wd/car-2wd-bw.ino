
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
// D5: 红外线 右
// D6: 红外线 左
// D7: 超声波 echo
// D8: 超声波 trig
// VM: 电机电源输入 4.5V~36V,可单独供电;
// VIN: 控制电源输入 4.5V~9V(10VMAX),可单独供电
// 短路 VM 和 VIN),可以方便地使用一路电源(必须 4.5V~9V)同时完成电机的驱动与控制;
//D
//G
//V

// IR 
// 黑色区域（距离内有障碍）为 0 ，白色区域（距离内无障碍）为 1
// ir1 0  ir2 1 左转
// ir1 0  ir2 0 直行
// ir1 1  ir2 0 右转
// ir1 1  ir2 1 停下


// 各种变量定义
// 各种变量定义 - begin

// mdns name 定义
// String mdnsname = "dragonfly-" + String(ESP.getChipId()); //dragonfly-7914686.local
  String mdnsname = "car-2w-bw";

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

// 电机模拟电量输入
int power=800;

// 红外
int ir1=D5;  // you
int ir2=D6;  // zuo

// 超声波
int trigPin = D8;
int echoPin = D7;

// 超声波定义变量
long duration;
int distance;

int carmodel=0; // 0 遥控; 1 寻线（黑线）; 2 超声波 10cm 停留，超过 10cm 前进，小于 10cm 后退。

int sir1 = 1;
int sir2 = 1;

// 各种变量定义 - end


// ***********************************


// 各种服务定义
// 各种服务定义 - begin

// TCP server at port 80 will respond to HTTP requests
ESP8266WebServer server;



// 各种服务定义 - end
// ***********************************



// car car car car car car car car car car car car car car car car car car car car
// analogWrite(mas,1024)
// digitalWrite(mad,1);
// car carforword  up
void carforword(int power){
  digitalWrite(mad,1);
  digitalWrite(mbd,1);  
  analogWrite(mas,power);
  analogWrite(mbs,power);
}



// car carreversion down
void carreversion(int power){
  digitalWrite(mad,0);
  digitalWrite(mbd,0);
  analogWrite(mas,power);
  analogWrite(mbs,power);
}



// car counterclockwise
void counterclockwise(int power){
  digitalWrite(mad,0);
  digitalWrite(mbd,1);
  analogWrite(mas,power);
  analogWrite(mbs,power);
  delay(300);
  carforword(power);
}



// car clockwise
void clockwise(int power){
  digitalWrite(mad,1);
  digitalWrite(mbd,0);
  analogWrite(mas,power);
  analogWrite(mbs,power);
  delay(300);
  carforword(power);
}


// car carstop
void carstop(){
  digitalWrite(mas,0);
  digitalWrite(mbs,0);
}


// car carforword  up little
void carforwordlittle(int power){
  // delay(500);
  digitalWrite(mad,1);
  digitalWrite(mbd,1);
  analogWrite(mas,power);
  analogWrite(mbs,power);
  delay(100);
  carstop();
}



// car carreversion down little
void carreversionlittle(int power){
  digitalWrite(mad,0);
  digitalWrite(mbd,0);
  analogWrite(mas,power);
  analogWrite(mbs,power);
  delay(100);
  carstop();
}



// car counterclockwise little
void counterclockwiselittle(int power){
  digitalWrite(mad,0);
  digitalWrite(mbd,1);
  analogWrite(mas,power);
  analogWrite(mbs,power);
  delay(100);
  carstop();
}



// car clockwise little
void clockwiselittle(int power){
  digitalWrite(mad,1);
  digitalWrite(mbd,0);
  analogWrite(mas,power);
  analogWrite(mbs,power);
  delay(100);
  carstop();
}



// xunxian
void xunxian(int power){
    sir1 = digitalRead(ir1);
    sir2 = digitalRead(ir2);

// ir1 0  ir2 1 左转
// ir1 0  ir2 0 直行
// ir1 1  ir2 0 右转
// ir1 1  ir2 1 停下    
    if ( sir1 == 0 and sir2 == 0){
      // ting
      Serial.println("ting ting");
      carstop();
    }
    else if ( sir1 == 0 and sir2 == 1){
      // zuo
      Serial.println("zuo zuo");
      counterclockwise(power);
    }
    else if ( sir1 == 1 and sir2 == 0){
      // you
      Serial.println("you you");
      clockwise(power);
    }
    else if ( sir1 == 1 and sir2 == 1){
      // qian
      Serial.println("qian qian");
      carforword(power);
    } 

}



// chaoshengbo
void chaoshengbo(int power) {
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);

//将trigPin设置为HIGH状态10微秒
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);

// 读取echoPin，以微秒为单位返回声波传播时间
    duration = pulseIn(echoPin, HIGH);

// 计算距离
    distance= duration*0.034/2;
// 打印距离在串行监视器
    Serial.print("Distance: ");
    Serial.print(distance);
    Serial.println(" cm");
    

    if (distance >20) {
      // qianjin
      Serial.println("qian qian");
      carforword(power);
    }
    else if (distance < 15){
      // houtui
      Serial.println("hou hou");
      carreversion(power);
    }
    else {
      // tingxia
      Serial.println("ting ting");
      carstop();
    }
    
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
  myhtml += "<input type=\"range\" name=\"power\" min=\"1\" max=\"1024\" />";

  myhtml += "<br/><br/>";
  myhtml += "<button type=\"submit\" name=\"car\" value=\"61\">Follow</button>";
  myhtml += "<button type=\"submit\" name=\"car\" value=\"62\">Echo</button>";

  myhtml += "<br/><br/><br/><br/>";
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

  myhtml += "</form>";

  myhtml += FPSTR(HTTP_END_NEW);  
  server.send(200,"text/html",myhtml);
  
  power = atoi(server.arg("power").c_str());

  if (server.arg("car")=="1") {
    Serial.println("qqqqqqqqqqqqqq");
    Serial.println(power);
    carforword(power);
  } else if (server.arg("car")=="2") {
    Serial.println("hhhhhhhhhhhhhhhhh");
    Serial.println(power);
    carreversion(power);
  } else if (server.arg("car")=="3") {
    Serial.println("zuo zuo zuo");
    Serial.println(power);
    counterclockwise(power);
  } else if (server.arg("car")=="4") {
    Serial.println("you you you");
    Serial.println(power);
    clockwise(power);
  } else if (server.arg("car")=="5") {
    Serial.println("ttttttttttttttttt");
    Serial.println(power);
    carstop();
    carmodel = 0;
  } else if (server.arg("car")=="6") {
    Serial.println("qqqqqqqqqqqqqq -L-L-L");
    Serial.println(power);
    carforwordlittle(power);
  } else if (server.arg("car")=="7") {
    Serial.println("hhhhhhhhhhhhhhhhh -L-L-L");
    Serial.println(power);
    carreversionlittle(power);
  } else if (server.arg("car")=="8") {
    Serial.println("zuo zuo zuo -L-L-L");
    Serial.println(power);
    counterclockwiselittle(power);
  } else if (server.arg("car")=="9") {
    Serial.println("you you you -L-L-L");
    Serial.println(power);
    clockwiselittle(power);
  } else if (server.arg("car")=="61") {
    Serial.println("xunxian xunxian xunxian");
    xunxian(power);
    carmodel = 1;
  } else if (server.arg("car")=="62") {
    Serial.println("chaoshengbo chaoshengbo chaoshengbo");
    chaoshengbo(power);
    carmodel = 2;
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



void setup() {

  Serial.begin(115200);
  Serial.println("=================================");

  pinMode(mas,OUTPUT);
  pinMode(mbs,OUTPUT);
  pinMode(mad,OUTPUT);
  pinMode(mbd,OUTPUT);

  // 超声波管角初始化
  pinMode(trigPin, OUTPUT); // 将trigPin设置为输出
  pinMode(echoPin, INPUT); // 将echoPin设置为输入

  // 红外线初始化
  pinMode(ir1,INPUT);
  pinMode(ir2,INPUT);

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

  if (carmodel == 1){
    xunxian(power);
  }
  else if (carmodel == 2){
    chaoshengbo(power);
  }

  delay(200);  

}

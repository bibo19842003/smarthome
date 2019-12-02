/**
 * Demo：
 *    演示ESP8266 mDNS responder功能
 * @author 单片机菜鸟
 * @date 2019/01/28
 */
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
 
//以下三个定义为调试定义
#define DebugBegin(baud_rate)    Serial.begin(baud_rate)
#define DebugPrintln(message)    Serial.println(message)
#define DebugPrint(message)    Serial.print(message)
 
const char* AP_SSID     = "TP-LINK_5344"; // XXXXXX -- 使用时请修改为当前你的 wifi ssid
const char* AP_PSK = "6206908you11011010";  // XXXXXX -- 使用时请修改为当前你的 wifi 密码
const unsigned long BAUD_RATE = 115200;// serial connection speed
 
//声明一下函数
void initBasic(void);
void initWifi(void);
void initWebServer(void);
void initmDNS(void);
 
ESP8266WebServer server(80);
 
/**
 * 处理根目录uri请求
 * uri:http://server_ip/
 */
void handleRoot() {
  DebugPrintln("handleRoot");
  server.send(200, "text/html", "Hello From ESP8266 mDNS demo");
}
 
/**
 * 处理无效uri
 * uri:http://server_ip/xxxx
 */
void handleNotFound() {
  DebugPrintln("handleNotFound");
  //打印无效uri的信息 包括请求方式 请求参数
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}
 
void setup(void) {

  initBasic();
  initWifi();
  initWebServer();
  initmDNS();
}
 
void loop(void) {
  server.handleClient();
}
 
/**
 * 初始化基础功能：波特率
 */
void initBasic(){
  DebugBegin(BAUD_RATE);
}
 
/**
 * 初始化wifi模块：工作模式 连接网络
 */
void initWifi(){
//  WiFi.mode(WIFI_STA);
//  WiFi.begin(AP_SSID, AP_PSK);
    WiFiManager wifiManager;
  wifiManager.autoConnect("dragonfly");
  DebugPrintln("");
 
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    DebugPrint(".");
  }
  DebugPrintln("");
  DebugPrint("Connected to ");
  DebugPrintln(AP_SSID);
  DebugPrint("IP address: ");
  DebugPrintln(WiFi.localIP());
}
 
/**
 * 初始化webserver
 */
void initWebServer(){
  //以下配置uri对应的handler
  server.on("/", handleRoot);
  server.on("/inline", []() {
    DebugPrintln("handleInline");
    server.send(200, "text/plain", "this works as well");
  });
  server.onNotFound(handleNotFound);
  //启动webserver
  server.begin();
  DebugPrintln("HTTP server started");
}
 
/**
 * 初始化mDNS
 */
void initmDNS(){
  if (!MDNS.begin("esp8266")) {
    DebugPrintln("Error setting up MDNS responder!");
    while (1) {
      delay(1000);
    }
  }
  DebugPrintln("mDNS responder started,please input http://esp8266.local/ in your browser after install Bonjour");
}



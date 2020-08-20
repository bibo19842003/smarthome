#include <SimpleDHT.h>
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <Wire.h> //IIC
#include <math.h> 
#include "pitches.h"
#include <WiFiManager.h>
#include <ESP8266mDNS.h>
#include <WiFiClient.h>
#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRutils.h>
#include <IRsend.h>
#include <Adafruit_NeoPixel.h>


// pin 使用说明：
// D0(16): wifiled
// D1(5): SCL
// D2(4): SDA
// D3(0): 
// D4(2): wifibutton 用于清除wifi密码
// D5(14): ws2812 in
// D6(12): dht11 信号
// D7(13): 红外线发送
// D8(15): speaker


// DHT11: https://blog.bbskali.cn/index.php/archives/1337/


// 各种变量定义
// 各种变量定义 - begin

// mdns name 定义
// String mdnsname = "dragonfly-" + String(ESP.getChipId()); //dragonfly-7914686.local
  String mdnsname = "speaker";

// http web page ,FPSTR from <WiFiManager.h>
const char HTTP_HEADER_NEW[] PROGMEM          = "<!DOCTYPE html><html lang=\"en\"><head><meta charset=\"UTF-8\"><meta name=\"viewport\" content=\"width=device-width, initial-scale=1, user-scalable=no\"/><meta http-equiv=\"refresh\" content=\"5\"><title></title>";
const char HTTP_STYLE_NEW[] PROGMEM           = "<style>.c{text-align: center;} div,input{padding:5px;font-size:1em;} input{width:95%;} body{text-align: center;font-family:verdana;} button{border:0;border-radius:0.3rem;background-color:#1fa3ec;color:#fff;line-height:2.4rem;font-size:1.2rem;width:100%;} .q{float: right;width: 64px;text-align: right;} </style>";
const char HTTP_SCRIPT_NEW[] PROGMEM          = "<script>function c(l){document.getElementById('s').value=l.innerText||l.textContent;document.getElementById('p').focus();}</script>";
const char HTTP_HEADER_END_NEW[] PROGMEM        = "</head><body><div style='text-align:center;display:inline-block;min-width:260px;'>";
const char HTTP_END_NEW[] PROGMEM             = "</div></body></html>";

const char HTTP_HEADER_NEW_IR[] PROGMEM          = "<!DOCTYPE html><html lang=\"en\"><head><meta charset=\"UTF-8\"><meta name=\"viewport\" content=\"width=device-width, initial-scale=1, user-scalable=no\"/><title></title>";
const char HTTP_STYLE_NEW_IR[] PROGMEM           = "<style>.c{text-align: center;} div,input{padding:5px;font-size:1em;} input{width:95%;} body{text-align: center;font-family:verdana;} button{border:0;border-radius:0.3rem;background-color:#1fa3ec;color:#fff;line-height:2.4rem;font-size:1.2rem;width:130px;} .q{float: right;width: 64px;text-align: right;} </style>";

// notes in the melody:
int melody[] = {NOTE_C4, NOTE_G3, NOTE_G3, NOTE_A3, NOTE_G3, 0, NOTE_B3, NOTE_C4};
// note durations: 4 = quarter note, 8 = eighth note, etc.:
int noteDurations[] = {4, 8, 8, 4, 4, 4, 4, 4};

int led = 16;     // LED引脚
int wifibutton = D4;  //wifibutton
int temp = 0;    // 用于读取按键状态的临时变量
int speakerpin = D8;  // 用于测试

int pinDHT11 = D6;
SimpleDHT11 dht11(pinDHT11);

// 温度 湿度
byte temperature = 0;
byte humidity = 0;

// 红外线 
const uint16_t kIrLed = D7; //发送
IRsend irsend(kIrLed);  // Set the GPIO to be used to sending the message.

//ws2812
#define LED_COUNT 8
#define LED_PIN 14
Adafruit_NeoPixel pixels(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

// 各种变量定义 - end


// ***********************************


// 各种服务定义
// 各种服务定义 - begin

// TCP server at port 80 will respond to HTTP requests
ESP8266WebServer server;



// 各种服务定义 - end

// ***********************************




void mytone() {
    Serial.println("LED Turned ON, buzzer on");

    // iterate over the notes of the melody:
    for (int thisNote = 0; thisNote < 8; thisNote++) {

      // to calculate the note duration, take one second divided by the note type.
      //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
      int noteDuration = 1000 / noteDurations[thisNote];
      tone(speakerpin, melody[thisNote], noteDuration);

      // to distinguish the notes, set a minimum time between them.
      // the note's duration + 30% seems to work well:
      int pauseBetweenNotes = noteDuration * 1.30;
      delay(pauseBetweenNotes);
      // stop the tone playing:
      noTone(8);  
    }
}


void valsensor()
{
  // FPSTR from <WiFiManager.h>
  String myhtml = FPSTR(HTTP_HEADER_NEW);
  myhtml += FPSTR(HTTP_SCRIPT_NEW);
  myhtml += FPSTR(HTTP_STYLE_NEW);
  myhtml += FPSTR(HTTP_HEADER_END_NEW);

  myhtml += "Temperature: ";
  myhtml += temperature;
  myhtml += " C";
  myhtml += "<br>";
  myhtml += "Humidity: ";
  myhtml += humidity;
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
  myhtml += temperature;
  myhtml += " C";
  myhtml += "<br>";
  myhtml += "Humidity: ";
  myhtml += humidity;
  myhtml += " %";
  myhtml += "<br>";
  server.send(200,"text/html",myhtml);

  if (server.arg("ledstatus")=="0") {
    digitalWrite(led,LOW);
  } else if (server.arg("ledstatus")=="1") {
    mytone();
    digitalWrite(led,HIGH);
  }

}


// tv - page
void tv()
{  
  String myhtml = FPSTR(HTTP_HEADER_NEW_IR);
  myhtml += FPSTR(HTTP_SCRIPT_NEW);
  myhtml += FPSTR(HTTP_STYLE_NEW_IR);
  myhtml += FPSTR(HTTP_HEADER_END_NEW);

  myhtml += "<form>";
 // myhtml += "<br/><br/>";
  myhtml += "<button type=\"submit\" name=\"tv\" value=\"32A650AF\">电源</button><br/><br/>";
  myhtml += "<button type=\"submit\" name=\"tv\" value=\"32A6C23D\">电视</button>";
  myhtml += "<button type=\"submit\" name=\"tv\" value=\"32A6827D\">HDMI</button><br/><br/>";
  myhtml += "<button type=\"submit\" name=\"tv\" value=\"32A6807F\">1</button>";
  myhtml += "<button type=\"submit\" name=\"tv\" value=\"32A640BF\">2</button>";
  myhtml += "<button type=\"submit\" name=\"tv\" value=\"32A6C03F\">3</button><br/><br/>";
  myhtml += "<button type=\"submit\" name=\"tv\" value=\"32A620DF\">4</button>";
  myhtml += "<button type=\"submit\" name=\"tv\" value=\"32A6A05F\">5</button>";
  myhtml += "<button type=\"submit\" name=\"tv\" value=\"32A6609F\">6</button><br/><br/>";
  myhtml += "<button type=\"submit\" name=\"tv\" value=\"32A6E01F\">7</button>";
  myhtml += "<button type=\"submit\" name=\"tv\" value=\"32A610EF\">8</button>";
  myhtml += "<button type=\"submit\" name=\"tv\" value=\"32A6906F\">9</button><br/><br/>";
  myhtml += "<button type=\"submit\" name=\"tv\" value=\"32A6A25D\">0</button><br/><br/>";
  myhtml += "<button type=\"submit\" name=\"tv\" value=\"32A6A857\">音量+</button>";
  myhtml += "<button type=\"submit\" name=\"tv\" value=\"32A6F00F\">静音</button>";
  myhtml += "<button type=\"submit\" name=\"tv\" value=\"32A6F807\">频道+</button><br/><br/>";
  myhtml += "<button type=\"submit\" name=\"tv\" value=\"32A638C7\">音量-</button>";
  myhtml += "<button type=\"submit\" name=\"tv\" value=\"32A600FF\">设置</button>";
  myhtml += "<button type=\"submit\" name=\"tv\" value=\"32A67887\">频道-</button><br/><br/>";
  myhtml += "<button type=\"submit\" name=\"tv\" value=\"32A602FD\">屏显</button>";
  myhtml += "<button type=\"submit\" name=\"tv\" value=\"32A630CF\">信号源</button><br/><br/>";
  myhtml += "<button type=\"submit\" name=\"tv\" value=\"32A6D02F\">上</button><br/><br/>";
  myhtml += "<button type=\"submit\" name=\"tv\" value=\"32A608F7\">左</button>";
  myhtml += "<button type=\"submit\" name=\"tv\" value=\"32A6B04F\">确定</button>";
  myhtml += "<button type=\"submit\" name=\"tv\" value=\"32A68877\">右</button><br/><br/>";
  myhtml += "<button type=\"submit\" name=\"tv\" value=\"32A6708F\">下</button><br/><br/>";
  myhtml += "<button type=\"submit\" name=\"tv\" value=\"32A6EC13\">菜单</button>";
  myhtml += "<button type=\"submit\" name=\"tv\" value=\"32A66897\">主页</button>";
  myhtml += "<button type=\"submit\" name=\"tv\" value=\"32A6B847\">返回</button><br/><br/>";
  myhtml += "</form>";

  myhtml += FPSTR(HTTP_END_NEW);  
  server.send(200,"text/html",myhtml);

  // irsend.sendNEC(0x32A650AF); 发送信号
  // irsend.sendNEC(atoi(("0x" + server.arg("tv")).c_str()));
  if (server.arg("tv")=="32A650AF") {
    irsend.sendNEC(0x32A650AF);
  } else if (server.arg("tv")=="32A6C23D") {
    irsend.sendNEC(0x32A6C23D);
  } else if (server.arg("tv")=="32A6827D") {
    irsend.sendNEC(0x32A6827D);
  } else if (server.arg("tv")=="32A6807F") {
    irsend.sendNEC(0x32A6807F);
  } else if (server.arg("tv")=="32A640BF") {
    irsend.sendNEC(0x32A640BF);
  } else if (server.arg("tv")=="32A6C03F") {
    irsend.sendNEC(0x32A6C03F);
  } else if (server.arg("tv")=="32A620DF") {
    irsend.sendNEC(0x32A620DF);
  } else if (server.arg("tv")=="32A6A05F") {
    irsend.sendNEC(0x32A6A05F);
  } else if (server.arg("tv")=="32A6609F") {
    irsend.sendNEC(0x32A6609F);
  } else if (server.arg("tv")=="32A6E01F") {
    irsend.sendNEC(0x32A6E01F);
  } else if (server.arg("tv")=="32A610EF") {
    irsend.sendNEC(0x32A610EF);
  } else if (server.arg("tv")=="32A6906F") {
    irsend.sendNEC(0x32A6906F);
  } else if (server.arg("tv")=="32A6A25D") {
    irsend.sendNEC(0x32A6A25D);
  } else if (server.arg("tv")=="32A6A857") {
    irsend.sendNEC(0x32A6A857);
  } else if (server.arg("tv")=="32A6F00F") {
    irsend.sendNEC(0x32A6F00F);
  } else if (server.arg("tv")=="32A6F807") {
    irsend.sendNEC(0x32A6F807);
  } else if (server.arg("tv")=="32A638C7") {
    irsend.sendNEC(0x32A638C7);
  } else if (server.arg("tv")=="32A600FF") {
    irsend.sendNEC(0x32A600FF);
  } else if (server.arg("tv")=="32A67887") {
    irsend.sendNEC(0x32A67887);
  }  else if (server.arg("tv")=="32A602FD") {
    irsend.sendNEC(0x32A602FD);
  } else if (server.arg("tv")=="32A630CF") {
    irsend.sendNEC(0x32A630CF);
  } else if (server.arg("tv")=="32A6D02F") {
    irsend.sendNEC(0x32A6D02F);
  } else if (server.arg("tv")=="32A608F7") {
    irsend.sendNEC(0x32A608F7);
  } else if (server.arg("tv")=="32A6B04F") {
    irsend.sendNEC(0x32A6B04F);
  } else if (server.arg("tv")=="32A68877") {
    irsend.sendNEC(0x32A68877);
  } else if (server.arg("tv")=="32A6708F") {
    irsend.sendNEC(0x32A6708F);
  } else if (server.arg("tv")=="32A6EC13") {
    irsend.sendNEC(0x32A6EC13);
  } else if (server.arg("tv")=="32A66897") {
    irsend.sendNEC(0x32A66897);
  } else if (server.arg("tv")=="32A6B847") {
    irsend.sendNEC(0x32A6B847);
  } 
}


// led page
void ledled()
{  
//  String myhtml = FPSTR(HTTP_HEADER_NEW_IR);
//  myhtml += FPSTR(HTTP_SCRIPT_NEW);
//  myhtml += FPSTR(HTTP_STYLE_NEW_IR);
//  myhtml += FPSTR(HTTP_HEADER_END_NEW);

  String myhtml ="<!DOCTYPE html><html lang=\"en\"><head><meta charset=\"utf-8\"><title>LED COLOR</title></head><body>";

  myhtml += "<form>";

  myhtml += "Brightness: <br/>";
  myhtml += "<input id=\"powernum\" value=\"powernum\" type=\"range\" name=\"power\" min=\"0\" max=\"255\"><br/><br/><br/>";

  myhtml += "Led Color: <br/>";
  myhtml += "<input type=\"radio\" name=\"ledcolor\" value=\"RED\">红色";  //red
  myhtml += "<input type=\"radio\" name=\"ledcolor\" value=\"YELLOW\">黄色";  //yellow
  myhtml += "<input type=\"radio\" name=\"ledcolor\" value=\"BLUE\">蓝色";  //blue
  myhtml += "<input type=\"radio\" name=\"ledcolor\" value=\"GREEN\">绿色";  //green
  myhtml += "<input type=\"radio\" name=\"ledcolor\" value=\"PURPLE\">紫色";  //purple
  myhtml += "<input type=\"radio\" name=\"ledcolor\" value=\"ORANGE\">橙色";  //oragne
  myhtml += "<input type=\"radio\" name=\"ledcolor\" value=\"WHITE\">白色";  //white
  myhtml += "<input type=\"radio\" name=\"ledcolor\" value=\"PINK\">粉色";  //pink
  myhtml += "<input type=\"radio\" name=\"ledcolor\" value=\"CYAN\">青色";  //cyan
  myhtml += "<input type=\"radio\" name=\"ledcolor\" value=\"MAGENTA\">品红";  //magenta
  myhtml += "<input type=\"radio\" name=\"ledcolor\" value=\"GRAY\">灰色<br/><br/><br/>";  //gray

  myhtml += "Led Status: <br/>";
  myhtml += "<input type=\"radio\" name=\"ledstatus\" value=\"0\">一直亮";  //FX_MODE_STATIC
  myhtml += "<input type=\"radio\" name=\"ledstatus\" value=\"2\">呼吸";  //FX_MODE_BREATH
  myhtml += "<input type=\"radio\" name=\"ledstatus\" value=\"1\">闪烁";  //FX_MODE_BLINK
  myhtml += "<input type=\"radio\" name=\"ledstatus\" value=\"40\">跑马灯";  //FX_MODE_RUNNING_COLOR
  myhtml += "<input type=\"radio\" name=\"ledstatus\" value=\"12\">彩虹";  //FX_MODE_RAINBOW_CYCLE
  myhtml += "<input type=\"radio\" name=\"ledstatus\" value=\"88\">彩虹 run<br/><br/><br/>";  //自定义

  myhtml += "<button type=\"submit\" name=\"ledled\" value=\"lightlight\">开灯</button>";
  myhtml += "<button type=\"submit\" name=\"ledled\" value=\"nolight\">关灯</button><br/><br/><br/>";

  myhtml += "</form>";

//  myhtml += FPSTR(HTTP_END_NEW);
  myhtml += "</body></html>";
  server.send(200,"text/html",myhtml);

  if (server.arg("ledled")=="lightlight") {
    String ledcolor=server.arg("ledcolor");
    String ledbrightness=server.arg("power");
    String ledledstatus=server.arg("ledstatus");

    int a;
    int b;
    int c;

    // ws2812 set color
    if (server.arg("ledcolor")=="RED") {
      a=255;
      b=0;
      c=0;
    } else if (server.arg("ledcolor")=="YELLOW") {
      a=255;
      b=255;
      c=0;
    } else if (server.arg("ledcolor")=="BLUE") {
      a=0;
      b=0;
      c=255;
    } else if (server.arg("ledcolor")=="GREEN") {
      a=0;
      b=255;
      c=0;
    } else if (server.arg("ledcolor")=="PURPLE") {
      a=40;
      b=0;
      c=80;
    } else if (server.arg("ledcolor")=="ORANGE") {
      a=255;
      b=30;
      c=0;
    } else if (server.arg("ledcolor")=="WHITE") {
      a=255;
      b=255;
      c=255;
    } else if (server.arg("ledcolor")=="PINK") {
      a=255;
      b=14;
      c=93;
    } else if (server.arg("ledcolor")=="CYAN") {
      a=0;
      b=255;
      c=255;
    } else if (server.arg("ledcolor")=="MAGENTA") {
      a=255;
      b=0;
      c=255;
    } else if (server.arg("ledcolor")=="GRAY") {
      a=10;
      b=10;
      c=10;
    }

    // 一直亮 0
    if (server.arg("ledstatus")=="0") {
      pixels.clear();
      pixels.setBrightness(ledbrightness.toInt());
      for(int i=0; i<LED_COUNT; i++) { // For each pixel...
      // pixels.Color() takes RGB values, from 0,0,0 up to 255,255,255
      // Here we're using a moderately bright green color:
        pixels.setPixelColor(i, pixels.Color(a, b, c));
      }
      pixels.show();   // Send the updated pixel colors to the hardware.
    }

    // 呼吸 2
    if (server.arg("ledstatus")=="2") {
      pixels.clear();
      for(int i=0; i<LED_COUNT; i++) {
        pixels.setPixelColor(i, pixels.Color(a, b, c));
      }

      for(int k=0; k<20; k++){
        for(int j=5; j<250; j=j+10) {
          pixels.setBrightness(j);
          pixels.show();
          delay(100);
        }
        for(int j=250; j>5; j=j-10) {
          pixels.setBrightness(j);
          pixels.show();
          delay(100);
        }
      }
    }

    // 闪烁 1
    if (server.arg("ledstatus")=="1") {
      pixels.clear();
      pixels.setBrightness(ledbrightness.toInt());
      for(int k=0; k<20; k++){
        for(int i=0; i<LED_COUNT; i++) {
          pixels.setPixelColor(i, pixels.Color(a, b, c));
        }
        pixels.show();
        delay(300);

        for(int i=0; i<LED_COUNT; i++) {
          pixels.setPixelColor(i, pixels.Color(0,0,0));
        }
        pixels.show();
        delay(300);
      }
    }

    // 跑马灯 40
    if (server.arg("ledstatus")=="40") {
      pixels.clear();
      pixels.setBrightness(ledbrightness.toInt());
      for(int k=0; k<20; k++){
        for(int i=0; i<LED_COUNT; i++) {
          pixels.clear();
          pixels.setPixelColor(i, pixels.Color(a, b, c));
          pixels.show();
          delay(200);
        }
      }
    }

    // 彩虹 12
    if (server.arg("ledstatus")=="12") {
      pixels.clear();
      pixels.setBrightness(ledbrightness.toInt());
      for(long firstPixelHue = 0; firstPixelHue < 5*65536; firstPixelHue += 256) {
        for(int i=0; i<pixels.numPixels(); i++) { 
          int pixelHue = firstPixelHue + (i * 65536L / pixels.numPixels());
          pixels.setPixelColor(i, pixels.gamma32(pixels.ColorHSV(pixelHue)));
        }
        pixels.show();
      }
    }

    // 彩虹 88
    if (server.arg("ledstatus")=="88") {
      pixels.clear();
      pixels.setBrightness(ledbrightness.toInt());
      
      int firstPixelHue = 0;     // First pixel starts at red (hue 0)
      for(int a=0; a<50; a++) {  // Repeat 30 times...
        for(int b=0; b<3; b++) { //  'b' counts from 0 to 2...
          pixels.clear();
          for(int c=b; c<pixels.numPixels(); c += 3) {
            int hue = firstPixelHue + c * 65536L / pixels.numPixels();
            uint32_t color = pixels.gamma32(pixels.ColorHSV(hue)); // hue -> RGB
            pixels.setPixelColor(c, color); // Set pixel 'c' to value 'color'
          }
          pixels.show();                // Update strip with new contents
          delay(100);                 // Pause for a moment
          firstPixelHue += 65536 / 90; // One cycle of color wheel over 90 frames
        }
      }
    }

    
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


void clearwifiinfor() {
  Serial.println("it will clear password!!!");
  WiFiManager wifiManager;
  wifiManager.resetSettings();
  Serial.println("it had cleared password!!!");
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

  // 红外线发送初始化
  irsend.begin();

  // set pinmode
  pinMode(led, OUTPUT);   // LED设置为输入
  pinMode(wifibutton, INPUT); // 按键设置为输出
  pinMode(pinDHT11, OUTPUT); // 温度传感器设置为输入
  pinMode(speakerpin, OUTPUT);   // LED设置为输入

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
  MDNS.addService("speaker", "tcp", 8080); 

  // server route
  server.on("/",nocss);
  server.on("/sensor",valsensor);
  server.on("/nocss",nocss);
  server.on("/mdns",mdnsresponder);
  server.on("/tv",tv);
  server.on("/ledled",ledled);
  server.onNotFound(handleNotFound);
  
  // Start the server
  server.begin();
  Serial.println(WiFi.localIP());

  // ws2812 led
  pixels.begin();  //ws2812初始化
}




void loop() {
  
  MDNS.update();

  // 获取温度 湿度
//  int err = SimpleDHTErrSuccess;
//  digitalWrite(pinDHT11, HIGH);  
//  err = dht11.read(&temperature, &humidity, NULL);

  // wait for http client
  server.handleClient();

  // wifibutton
  temp = digitalRead(wifibutton);
  if (temp == LOW) {
    clearwifiinfor();
  }

  delay(1000);
}

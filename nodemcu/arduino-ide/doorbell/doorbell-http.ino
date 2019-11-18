#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
 
#ifndef STASSID
#define STASSID "CMCC-jRtE"
#define STAPSK  "gdky6zx6"
#endif

const char* ssid = STASSID;
const char* password = STAPSK;

HTTPClient http;
String GetUrl;


// D4(2): button 信号
int button = 2; // 按键连接的引脚

int temp = 0;



void kaimen() {
  GetUrl = "http://192.168.1.8/?ledstatus=1";
  http.begin(GetUrl);
  int httpCode = http.GET();
  if (httpCode > 0) {
      Serial.printf("[HTTP] GET... code: %d\n", httpCode);
      //判断请求是否成功
      if (httpCode == HTTP_CODE_OK) {
        Serial.println("ok ok ok");
        }
      }
  else {
      Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
  }
  http.end();
}


void setup() {
  Serial.begin(115200);
  Serial.println("=================================");
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
  
  pinMode(button, INPUT); // 按键设置为输出
}
 
void loop() {

  // button control doorbell
  temp = digitalRead(button);
  if (temp == LOW) {
      kaimen();
  }

  //delay(1000);//每1s调用一次 

  Serial.println("test test test");

// 添加低功耗模式，modem模式不需要设置触发类型；GPIO_PIN_INTR_LOLEVEL/GPIO_PIN_INTR_HILEVEL
// wifi_set_sleep_type(MODEM_SLEEP_T);
  wifi_set_sleep_type(LIGHT_SLEEP_T);
  gpio_pin_wakeup_enable(2, GPIO_PIN_INTR_LOLEVEL);
  
}
  

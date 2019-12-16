#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiManager.h>
#include <ESP8266mDNS.h>


HTTPClient http;
String GetUrl;

String mdnsname = "doorbell";

// n 为 mdns service host 数量
int n;

// D4(2): button 信号
int button = 2; // 按键连接的引脚

int temp = 0;


/*
    for (int i = 0; i < n; ++i) {
      // 打印查找到的服务具体信息
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.print(MDNS.hostname(i));
      Serial.print(" (");
      Serial.print(MDNS.IP(i));
      Serial.print(":");
      Serial.print(MDNS.port(i));
      Serial.println(")");
    }
*/

void kaimen() {

  for (int i = 0; i < n; ++i) {

    IPAddress ip = MDNS.IP(i);
    String ipStr = String(ip[0]) + '.' + String(ip[1]) + '.' + String(ip[2]) + '.' + String(ip[3]);
    GetUrl = "http://" + ipStr + "/?ledstatus=1";
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
  
}


void setup() {
  Serial.begin(115200);
  Serial.println("=================================");

  // Connect to WiFi network
  WiFiManager wifiManager;
  wifiManager.autoConnect("dragonfly");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(F("."));
  }
  Serial.println();
  Serial.println(WiFi.localIP());
  Serial.println(F("WiFi connected"));

  pinMode(button, INPUT); // 按键设置为输入
  
  if (!MDNS.begin(mdnsname)) {
    Serial.println("Error setting up MDNS responder!");
  }
  Serial.println("mDNS responder started");
  
  //查找服务
  // int n = MDNS.queryService("speaker", "tcp"); // Send out query for esp tcp services
  n = MDNS.queryService("speaker", "tcp");
  Serial.println("mDNS query done");
  if (n == 0) {
    Serial.println("no services found");
  } else {
    Serial.print(n);
    Serial.println(" service(s) found");
    for (int i = 0; i < n; ++i) {
      // 打印查找到的服务具体信息
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.print(MDNS.hostname(i));
      Serial.print(" (");
      Serial.print(MDNS.IP(i));
      Serial.print(":");
      Serial.print(MDNS.port(i));
      Serial.println(")");
    }
  }
}
 
void loop() {

  // 添加低功耗模式，modem模式不需要设置触发类型；GPIO_PIN_INTR_LOLEVEL/GPIO_PIN_INTR_HILEVEL
  // wifi_set_sleep_type(MODEM_SLEEP_T);
  wifi_set_sleep_type(LIGHT_SLEEP_T);
  //gpio_pin_wakeup_enable(D4, GPIO_PIN_INTR_LOLEVEL);
  gpio_pin_wakeup_enable(2, GPIO_PIN_INTR_LOLEVEL);

  MDNS.update();
  
  Serial.println("test test test");

  // button control doorbell
  temp = digitalRead(button);
  if (temp == LOW) {
      kaimen();
  }

  delay(1000);//每1s调用一次 

}

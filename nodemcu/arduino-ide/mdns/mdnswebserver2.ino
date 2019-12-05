/*
  演示ESP8266 mDNS 发现服务功能

  注意:
  - 输入你的 WiFi SSID 和 password.
  - 烧写到两块 ESP8266  板子 
*/

#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>

const char* ssid     = "...";
const char* password = "...";
char hostString[16] = {0};

void setup() {
  Serial.begin(115200);
  delay(100);
  Serial.println("\r\nsetup()");

  sprintf(hostString, "ESP_%06X", ESP.getChipId());
  Serial.print("Hostname: ");
  Serial.println(hostString);
  WiFi.hostname(hostString);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(250);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (!MDNS.begin(hostString)) {
    Serial.println("Error setting up MDNS responder!");
  }
  Serial.println("mDNS responder started");
  //往mDNS里面注册服务
  MDNS.addService("esp", "tcp", 8080); 

  Serial.println("Sending mDNS query");
  //查找服务
  int n = MDNS.queryService("esp", "tcp"); // Send out query for esp tcp services
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
  Serial.println();
  Serial.println("loop() next");
}

void loop() {
  MDNS.update();
}


#include <ESP8266WiFi.h>
#include <time.h>

const char* ssid = "";
const char* password = "";
int timezone = 8;
int dst = 0;



String toe(int xxx) {
  if (xxx < 10) {
    return "0"+ String(xxx);
    }
  else {
    return String(xxx);
    }
}




void setup() {
     Serial.begin(115200);
     Serial.setDebugOutput(true);
     WiFi.mode(WIFI_STA);
     WiFi.begin(ssid, password);
     Serial.println("\nConnecting to WiFi");
     while (WiFi.status() != WL_CONNECTED) {
       Serial.print(".");
       delay(1000);
     }
     configTime(timezone * 3600, dst * 0, "pool.ntp.org");
     Serial.println("\nWaiting for time");
     while (!time(nullptr)) {
       Serial.print(".");
       delay(1000);
     }
     Serial.println("");
}

void loop() {
// 
     time_t now;
     struct tm * timeinfo;
     time(&now);
     timeinfo = localtime(&now); 
     Serial.print(timeinfo->tm_year + 1900);
     Serial.print("-");
     Serial.print(timeinfo->tm_mon);
     Serial.print("-");
     Serial.print(timeinfo->tm_mday);
     Serial.print("  ");
//     Serial.print(timeinfo->tm_hour);
     Serial.print(toe(timeinfo->tm_hour));
     Serial.print(":"); 
//     Serial.print(timeinfo->tm_min);
     Serial.print(toe(timeinfo->tm_min));
     Serial.print(":");
//     Serial.print(timeinfo->tm_sec);
     Serial.print(toe(timeinfo->tm_sec));
     Serial.print("  ");
     Serial.print(timeinfo->tm_wday);
     Serial.print("  ");
     Serial.println(timeinfo->tm_yday);
          
     delay(1000);
//






}

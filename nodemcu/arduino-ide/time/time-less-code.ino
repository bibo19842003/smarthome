/*
  NTP-TZ-DST
  NetWork Time Protocol - Time Zone - Daylight Saving Time

  This example shows how to read and set time,
  and how to use NTP (set NTP0_OR_LOCAL1 to 0 below)
  or an external RTC (set NTP0_OR_LOCAL1 to 1 below)

  TZ and DST below have to be manually set
  according to your local settings.

  This example code is in the public domain.
*/

#include <ESP8266WiFi.h>
#include <time.h>                       // time() ctime()
#include <sys/time.h>                   // struct timeval
#include <coredecls.h>                  // settimeofday_cb()

////////////////////////////////////////////////////////

#ifndef STASSID
#define STASSID "CMCC-jRtE"
#define STAPSK  "gdky6zx6"
#endif

#define SSID            STASSID
#define SSIDPWD         STAPSK
#define TZ              7       // (utc+) TZ in hours
#define DST_MN          60      // use 60mn for summer time in some countries

#define RTC_TEST     1510592825 // 1510592825 = Monday 13 November 2017 17:07:05 UTC

////////////////////////////////////////////////////////

#define TZ_MN           ((TZ)*60)
#define TZ_SEC          ((TZ)*3600)
#define DST_SEC         ((DST_MN)*60)

time_t now;

timeval cbtime;			// time set in callback
bool cbtime_set = false;

void time_is_set(void) {
  gettimeofday(&cbtime, NULL);
  cbtime_set = true;
  Serial.println("------------------ settimeofday() was called ------------------");
}


void setup() {
  Serial.begin(115200);
  settimeofday_cb(time_is_set);

// ntp
  configTime(TZ_SEC, DST_SEC, "pool.ntp.org");
  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID, SSIDPWD);
 
}


void loop() {

  now = time(nullptr);

  // human readable
  Serial.print(ctime(&now));

  // simple drifting loop
  delay(2000);
}

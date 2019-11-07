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

#define NTP0_OR_LOCAL1  0       // 0:use NTP  1:fake external RTC
#define RTC_TEST     1510592825 // 1510592825 = Monday 13 November 2017 17:07:05 UTC

////////////////////////////////////////////////////////

#define TZ_MN           ((TZ)*60)
#define TZ_SEC          ((TZ)*3600)
#define DST_SEC         ((DST_MN)*60)

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

#if NTP0_OR_LOCAL1
  // local

  ESP.eraseConfig();
  time_t rtc = RTC_TEST;
  timeval tv = { rtc, 0 };
  timezone tz = { TZ_MN + DST_MN, 0 };
  settimeofday(&tv, &tz);

#else // ntp

  configTime(TZ_SEC, DST_SEC, "pool.ntp.org");
  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID, SSIDPWD);
  // don't wait, observe time changing when ntp timestamp is received

#endif // ntp
}

// for testing purpose:
extern "C" int clock_gettime(clockid_t unused, struct timespec *tp);


timeval tv;
timespec tp;
time_t now;
uint32_t now_ms, now_us;

void loop() {

  gettimeofday(&tv, nullptr);
  clock_gettime(0, &tp);
  now = time(nullptr);
  now_ms = millis();
  now_us = micros();

  // localtime / gmtime every second change
  static time_t lastv = 0;
  if (lastv != tv.tv_sec) {
    Serial.println();
  }

  // human readable
  Serial.print(ctime(&now));

  // simple drifting loop
  delay(2000);
}

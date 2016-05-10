// DS3231_Test.ino
// Nipun Gunawardena
// Tests DS3231, with squarewave on and interrupts
// Modified from DS3231 example in RTClib
// Todo: Try sleep modes

#include <Wire.h>
#include "RTClib.h"
#include "./DS3231_Alarm1.h"

#define LEDPIN 13


RTC_DS3231 rtc;
DS3231_Alarm1 rtcAlarm1;
volatile bool rtcFlag = false;
const uint8_t deltaT = 10;  // Seconds




void setup () {

#ifndef ESP8266
  while (!Serial); // for Leonardo/Micro/Zero
#endif

  pinMode(LEDPIN, OUTPUT);

  Serial.begin(9600);
  delay(3000);  // Wait for console opening
  Serial.println("Starting Sketch");

  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }

  if (rtc.lostPower()) {
    Serial.println("RTC lost power, lets set the time!");
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }

  // Uncomment following line if force time set is desired
//  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));


  // Attach interrupt. Until sleep modes figured out, use while loop to emulate sleep mode
  attachInterrupt(12, rtcISR, FALLING);

  // Wait for even interval of deltaT to start measurements
  // If alarm set time is <1 second away, wait for next even value
  Serial.print("Set Alarm to ");
  Serial.print(deltaT);
  Serial.println(" second evenly divisible start point");
  DateTime now = rtc.now();
  uint8_t setSec;
  uint8_t rem = now.second() % deltaT;
  uint8_t lef = deltaT - rem;
  if (lef == 1) {
    setSec = 1 + deltaT;
  } else {
    setSec = lef;
  }
  rtcAlarm1.alarmSecondsSet(now, setSec);

  rtcAlarm1.alarmOn();
  Serial.print("Current Seconds: ");
  Serial.print(now.second());
  Serial.print(" || Setting alarm to ");
  Serial.print(setSec);
  Serial.println(" seconds from now. Starting Loop...");
}




void loop () {
  digitalWrite(LEDPIN, rtcFlag);
  while (!rtcFlag);
  digitalWrite(LEDPIN, rtcFlag);

  DateTime now = rtc.now();
  Serial.print(now.year(), DEC);
  Serial.print('/');
  Serial.print(now.month(), DEC);
  Serial.print('/');
  Serial.print(now.day(), DEC);
  Serial.print(" ");
  Serial.print(now.hour(), DEC);
  Serial.print(':');
  Serial.print(now.minute(), DEC);
  Serial.print(':');
  Serial.print(now.second(), DEC);
  Serial.println();
  rtcAlarm1.alarmSecondsSet(now, deltaT);
  rtcFlag = false;
}




void rtcISR(void) {
  rtcFlag = true;
}


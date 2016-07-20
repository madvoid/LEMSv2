// DS3231_Test.ino
// Nipun Gunawardena
// Tests DS3231 Alarm1
// Modified from DS3231 example in RTClib
// Todo: Try sleep modes

#include <Wire.h>
#include "RTClib.h"
#include "./DS3231_Alarm1.h"

#define LEDPIN 13


RTC_DS3231 rtc;
DS3231_Alarm1 rtcAlarm1;
volatile bool rtcFlag = false;
bool ledFlag = true;
const uint8_t deltaT = 10;  // Seconds




void setup () {

  //#ifndef ESP8266
  //  while (!Serial); // for Leonardo/Micro/Zero
  //#endif

  pinMode(LEDPIN, OUTPUT);

  SerialUSB.begin(9600);
  delay(3000);  // Wait for console opening
  SerialUSB.println("Starting Sketch");

  if (! rtc.begin()) {
    SerialUSB.println("Couldn't find RTC");
    while (1);
  }

  if (rtc.lostPower()) {
    SerialUSB.println("RTC lost power, lets set the time!");
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
  SerialUSB.print("Set Alarm to ");
  SerialUSB.print(deltaT);
  SerialUSB.println(" second evenly divisible start point");
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
  SerialUSB.print("Current Seconds: ");
  SerialUSB.print(now.second());
  SerialUSB.print(" || Setting alarm to ");
  SerialUSB.print(setSec);
  SerialUSB.println(" seconds from now. Starting Loop...");
}




void loop () {
  //  digitalWrite(LEDPIN, rtcFlag);
  while (!rtcFlag);
  //    idleSleep(0x02);
  //  digitalWrite(LEDPIN, rtcFlag);
  digitalWrite(LEDPIN, ledFlag);

  DateTime now = rtc.now();
  SerialUSB.print(now.year(), DEC);
  SerialUSB.print('/');
  SerialUSB.print(now.month(), DEC);
  SerialUSB.print('/');
  SerialUSB.print(now.day(), DEC);
  SerialUSB.print(" ");
  SerialUSB.print(now.hour(), DEC);
  SerialUSB.print(':');
  SerialUSB.print(now.minute(), DEC);
  SerialUSB.print(':');
  SerialUSB.print(now.second(), DEC);
  SerialUSB.println();
  rtcAlarm1.alarmSecondsSet(now, deltaT);
  rtcFlag = false;
  ledFlag = !ledFlag;
}




void rtcISR(void) {
  rtcFlag = true;
}


// Sleep function. Variable idleMode can take values from 0x00 to 0x02,
// where 0x00 is "lightest" sleep and 0x02 is the deepest idle sleep
void idleSleep(uint8_t idleMode) {
  if (idleMode > 0x02) {
    idleMode = 0x02;
  }
  SCB->SCR &= ~SCB_SCR_SLEEPDEEP_Msk;
  PM->SLEEP.reg = idleMode;
  __DSB();
  __WFI();
}

// See https://forum.arduino.cc/index.php?topic=337289.0 for help
void standbySleep(void) {
  SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;
  __WFI();
}


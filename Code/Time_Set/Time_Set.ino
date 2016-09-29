// Time_Set.ino
// Nipun Gunawardena
// Adjust the time for the DS3231 real time clock on LEMSv2.
// Designed to be used with Sparkfun SAMD21 Dev Board
// Modified from the RTCLib DS3231 example

// Date and time functions using a DS3231 RTC connected via I2C and Wire lib
#include <Wire.h>
#include "RTClib.h"

RTC_DS3231 rtc;

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

void setup () {

#ifndef ESP8266
  while (!SerialUSB); // for Leonardo/Micro/Zero
#endif

  SerialUSB.begin(9600);

  // Wait for console open
  delay(3000);

  // Begin RTC
  if (! rtc.begin()) {
    SerialUSB.println("Couldn't find RTC");
    while (1);
  }

  // Set time to compile time
  SerialUSB.println("Setting time to compile time!");
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

}

void loop () {

  // Get time
  DateTime now = rtc.now();

  // Print time
  SerialUSB.print(now.year(), DEC);
  SerialUSB.print('/');
  SerialUSB.print(now.month(), DEC);
  SerialUSB.print('/');
  SerialUSB.print(now.day(), DEC);
  SerialUSB.print(" (");
  SerialUSB.print(daysOfTheWeek[now.dayOfTheWeek()]);
  SerialUSB.print(") ");
  SerialUSB.print(now.hour(), DEC);
  SerialUSB.print(':');
  SerialUSB.print(now.minute(), DEC);
  SerialUSB.print(':');
  SerialUSB.print(now.second(), DEC);
  SerialUSB.println();

  // Pause temporarily
  SerialUSB.println();
  delay(3000);
}

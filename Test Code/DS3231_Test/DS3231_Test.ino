// DS3231_Test.ino
// Nipun Gunawardena
// Tests DS3231 Alarm1
// Modified from DS3231 example in RTClib
// Todo: Try different sleep modes than just standby
//       Make standby more efficient by only leaving important clocks on
// Thanks to @sandeepmistry at https://github.com/arduino/ArduinoCore-samd/issues/142 for help with sleep code

#include <Wire.h>
#include "RTClib.h"
#include "./DS3231_Alarm1.h"


#define LED_PIN 13
#define RTC_PIN 9

#define DEBUG 0


RTC_DS3231 rtc;
DS3231_Alarm1 rtcAlarm1;
volatile bool rtcFlag = false;
const uint8_t deltaT = 10;  // Seconds




void setup () {

  //#ifndef ESP8266
  //  while (!Serial); // for Leonardo/Micro/Zero
  //#endif

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);

  // Configure the regulator to run in normal mode when in standby mode
  // Otherwise it defaults to low power mode and can only supply 50 uA
  SYSCTRL->VREG.bit.RUNSTDBY = 1;

  // Enable the DFLL48M clock in standby mode
  SYSCTRL->DFLLCTRL.bit.RUNSTDBY = 1;

  // Disable the USB device, this avoids USB interrupts
  // mainly the SOF every 1ms.
  // Note: you'll have to double tap the reset button to load new sketches
  // This is only needed when connected via native USB
#if DEBUG
  USBDevice.detach();
#endif

  // Print welcome
  delay(3000);  // Wait for console opening
#if DEBUG
  SerialUSB.begin(9600);
  SerialUSB.println("Starting Sketch");
#endif

  // Start RTC
  if (! rtc.begin()) {
#if DEBUG
    SerialUSB.println("Couldn't find RTC");
#endif
    while (1);
  }

  // Adjust time if needed
  if (rtc.lostPower()) {
#if DEBUG
    SerialUSB.println("RTC lost power, lets set the time!");
#endif
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }

  // Uncomment following line if force time set is desired
  // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

  // Attach interrupt. Until sleep modes figured out, use while loop to emulate sleep mode
  attachInterrupt(RTC_PIN, rtcISR, FALLING);

  // Wait for even interval of deltaT to start measurements
  // If alarm set time is <1 second away, wait for next even value
#if DEBUG
  SerialUSB.print("Set Alarm to ");
  SerialUSB.print(deltaT);
  SerialUSB.println(" second evenly divisible start point");
#endif
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

  // Turn on Alarm and enter loop
  rtcAlarm1.alarmOn();
#if DEBUG
  SerialUSB.print("Current Seconds: ");
  SerialUSB.print(now.second());
  SerialUSB.print(" || Setting alarm to ");
  SerialUSB.print(setSec);
  SerialUSB.println(" seconds from now. Starting Loop...");
#endif
}




void loop () {

  // Turn LED off and sleep
  digitalWrite(LED_PIN, rtcFlag);
  standbySleep();

  // Upon Wake, turn on LED and print time
  digitalWrite(LED_PIN, rtcFlag);
  DateTime now = rtc.now();
#if DEBUG
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
#endif

  // Set alarm for future, prepare for sleep
  rtcAlarm1.alarmSecondsSet(now, deltaT);
  rtcFlag = false;

  // Delay just to have LED on for some time
  delay(100);
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


void standbySleep(void) {
  SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;
  __WFI();
}


// ------------------------------------------------------------------------------------------------
// Filename:
//		LEMSv2.ino
//
// Author:
//		Nipun Gunawardena
//
// Description:
//		LEMSv2 Arduino Code
//
// Credits:
//		Thanks to all the 3rd party libraries, especially to Adafruit for the excellent
//		libraries and hardware. More specifics to be added as needed
//
// Requirements:
//		- LEMS Hardware
//		- Libraries in libraries list
//
// Notes:
//		- As far as I know, extra files have to be in the same folder as the sketch. Therefore,
//      the DS3231 alarm files have to be copied from their test folder. This means there are
//      possibilities for code divergence. Be aware.
//
// Todo:
//		- SD Card
//		- All sensors
//		- Wifi
//		- Low Power Modes
//    - LEDs - Add LEDs, green -> 5; red -> 6
//    - Preprocessor defs for different sensors
//
// ------------------------------------------------------------------------------------------------




// Defines ----------------------------------------------------------------------------------------
#define DEBUG 1
#define GREEN_LED 13
#define RED_LED 6
#define CARDSELECT 5



// Libraries --------------------------------------------------------------------------------------
#include <SD.h>			        // SD Card Library
#include <SPI.h>		        // SPI Library
#include <Wire.h>		        // I2C Library
#include <RTClib.h>		      // RTC Library
#include "DS3231_Alarm1.h"  // RTC Alarm Files



// Initialize Variables----------------------------------------------------------------------------

// Real Time Clock
RTC_DS3231 rtc;                 // Real Time Clock Class
DS3231_Alarm1 rtcAlarm1;        // RTC Alarm
volatile bool rtcFlag = false;  // Used for ISR to know when to take measurements
const uint8_t deltaT = 10;      // Sampling time - Seconds

// SD Card
File logfile;
char filename[] = "LEMXX_00.CSV";






// Setup ------------------------------------------------------------------------------------------
// ************************************************************************************************
void setup() {

#if DEBUG
  while (!Serial);
  Serial.begin(9600);
  delay(3000);
  Serial.println("Starting Sketch");
#endif

  // Set pins
  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(CARDSELECT, OUTPUT);

  // RTC Setup
  if (!rtc.begin()) {
    error("Couldn't Find RTC!");
  }

  // Attach Interrupt - Interrupts from RTC used instead of delay()
  attachInterrupt(12, rtcISR, FALLING);

  // Check for Card Present
  if (!SD.begin(CARDSELECT)) {
    error("Card Init. Failed!");
  }

  // Check for new filename
  for (uint8_t i = 0; i < 100; i++) {
    filename[6] = '0' + i / 10;
    filename[7] = '0' + i % 10;
    if (! SD.exists(filename)) {
      break;
    }
  }

  // Open file
  logfile = SD.open(filename, FILE_WRITE);
  if (!logfile) {
    error("Couldn't Create File!");
  }
#if DEBUG
  Serial.print("Writing to ");
  Serial.println(filename);
#endif

  // Write Header - Debug header at end of setup()
  logfile.print("Year,Month,Date,Hour,Minute,Second");
  logfile.println();

  // Delay and indicate start
  delay(2000);
  digitalWrite(GREEN_LED, HIGH);
  delay(1500);
  digitalWrite(GREEN_LED, LOW);

  // Wait for next time with evenly divisible deltaT, then set alarm
  // If alarm set time is <1 second away, wait for next even value
  DateTime now = rtc.now();
  uint8_t setSec;                           // Seconds value to set alarm to
  uint8_t rem = now.second() % deltaT;      // Seconds since last set point
  rem = deltaT - rem;                       // Calculate seconds until next set point
  setSec = ((rem == 1) ? 1 + deltaT : rem); // Set to next *convenient* set point
  rtcAlarm1.alarmSecondsSet(now, setSec);
  rtcAlarm1.alarmOn();
  rtcFlag = false;
#if DEBUG
  Serial.print("Set RTC alarm to ");
  Serial.print(deltaT);
  Serial.println(" second evenly divisible start point");
  Serial.print("Current Seconds: ");
  Serial.print(now.second());
  Serial.print(" || Setting alarm to ");
  Serial.print(setSec);
  Serial.println(" seconds from now. Starting Loop...");
  Serial.print("Year,Month,Date,Hour,Minute,Second");
  Serial.println();
#endif

} // End setup()






// Loop -------------------------------------------------------------------------------------------
// ************************************************************************************************
void loop() {
  // Wait for interrupt
  digitalWrite(GREEN_LED, rtcFlag);    // Turn LED off
  while (!rtcFlag);
  digitalWrite(GREEN_LED, rtcFlag);    // Turn LED on

  // Gather Measurements
  DateTime now = rtc.now();

  // Log Data
  logfile.print(now.year(), DEC);
  logfile.print(",");
  logfile.print(now.month(), DEC);
  logfile.print(",");
  logfile.print(now.day(), DEC);
  logfile.print(",");
  logfile.print(now.hour(), DEC);
  logfile.print(",");
  logfile.print(now.minute(), DEC);
  logfile.print(",");
  logfile.print(now.second(), DEC);
  logfile.println();
  logfile.flush();


  // Debug Info
#if DEBUG
  Serial.print(now.year(), DEC);
  Serial.print(",");
  Serial.print(now.month(), DEC);
  Serial.print(",");
  Serial.print(now.day(), DEC);
  Serial.print(",");
  Serial.print(now.hour(), DEC);
  Serial.print(",");
  Serial.print(now.minute(), DEC);
  Serial.print(",");
  Serial.print(now.second(), DEC);
  Serial.println();
#endif

  // Turn alarm on for next measurement
  rtcAlarm1.alarmSecondsSet(now, deltaT);
  rtcFlag = false;

} // End loop()






// Functions --------------------------------------------------------------------------------------
// ************************************************************************************************

// Turn red LED on and loop. Used in case of errors
void error(String errorMsg) {
#if DEBUG
  Serial.println(errorMsg);
#endif

  digitalWrite(RED_LED, HIGH);
  while (true);
}


// ISR for RTC alarm. Used to break out of while loop used instead of delay
void rtcISR(void) {
  rtcFlag = true;
}

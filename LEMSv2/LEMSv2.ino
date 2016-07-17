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
//    - Sleep code taken from https://github.com/rocketscream/Low-Power/blob/master/LowPower.cpp
//      and from https://github.com/pubnub/samd21/blob/master/examples/Code/FirmwareUpgrade/ASF/sam0/drivers/system/system.h
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
//		- All sensors
//    - Make ADC generalizable. Replace all 4096
//		- Low Power Modes - Implement standby mode?
//    - LEDs - Add LEDs, change pins
//    - Line 196, print everything no more dynamic header?
//
// ------------------------------------------------------------------------------------------------



// Sensor Defines: Plugged in sensors should be defined as 1 --------------------------------------
#define TEMPRH 1
#define IR 1
#define UPPERSOIL 1
#define LOWERSOIL 1
#define SUNLIGHT 1
#define WIND 1
#define PRESSURE 1



// Other Defines ----------------------------------------------------------------------------------
#define DEBUG 0



// Pin Defines ------------------------------------------------------------------------------------
#define GREEN_LED_PIN 6     // Green LED pin - Change LED Pins in future!!
#define RED_LED_PIN 7       // Red LED pin - Change LED Pins in future!!
#define CARDSELECT 5        // Sd card chip select pin
#define RTC_ALARM_PIN 9     // DS3231 Alarm pin
#define SUN_PIN A5          // Li200 solar radiation pin
#define WDIR_PIN A4         // Davis wind direction pin
#define WSPD_PIN 8          // Davis wind speed pin
#define USOIL_POW_PIN 2     // 5TM Power Pin, Upper
#define LSOIL_POW_PIN 38    // 5TM Power Pin, Lower
#define USOIL_SER Serial1   // 5TM Serial Port, Upper. RX = D0, TX = D1
#define LSOIL_SER Serial    // 5TM Serial Port, Lower. RX = D31, TX = D30





// Libraries --------------------------------------------------------------------------------------
#include <SD.h>			            // SD Card Library
#include <SPI.h>		            // SPI Library
#include <math.h>               // Math library - https://www.arduino.cc/en/Math/H
#include <Wire.h>		            // I2C Library
#include <RTClib.h>		          // RTC Library - https://github.com/adafruit/RTClib
#include "DS3231_Alarm1.h"      // RTC Alarm Files
#include <Adafruit_SHT31.h>     // SHT31 - https://github.com/adafruit/Adafruit_SHT31
#include <Adafruit_Sensor.h>    // Necessary for BMP280 Code
#include <Adafruit_BMP280.h>    // BMP280 - https://github.com/adafruit/Adafruit_BMP280_Library
#include <Adafruit_MLX90614.h>  // MLX90614 Library - https://github.com/adafruit/Adafruit-MLX90614-Library



// Initialize Variables----------------------------------------------------------------------------

// Real Time Clock
RTC_DS3231 rtc;                 // Real Time Clock Class
DS3231_Alarm1 rtcAlarm1;        // RTC Alarm
volatile bool rtcFlag = false;  // Used for ISR to know when to take measurements
const uint8_t deltaT = 10;      // Sampling time - Seconds

// SD Card
File logfile;                       // File object
char filename[] = "LEMXX_00.CSV";   // Initial filename

// MLX90614
#if IR
float mlxIR;                                   // IR values from MLX90614
float mlxAmb;                                  // Ambient temp values from MLX90614
Adafruit_MLX90614 mlx = Adafruit_MLX90614();   // MLX90614 class
#endif

// Upper soil
#if UPPERSOIL
float upperTemp;            // Upper soil temp
float upperMois;            // Upper soil mois
#endif

// Lower soil
#if LOWERSOIL
float lowerTemp;            // Lower soil temp
float lowerMois;            // Lower soil mois
#endif

// BMP280
#if PRESSURE
Adafruit_BMP280 bmp;        // Initialize BMP280 class
float pressure;             // Barometric pressure
float bmpAmb;               // Temperature from BMP280
#endif

// Wind
#if WIND
unsigned long startTime;            // Used to measure averaging period for wind speed
volatile unsigned long windCount;   // Counts to convert to speed
float wDir;                         // Wind direction
float wSpd;                         // Wind speed
#endif

// SHT21
#if TEMPRH
Adafruit_SHT31 sht = Adafruit_SHT31();
float shtAmb;               // Ambient temp values from SHT21
float shtHum;               // Relative humidity values from SHT21
#endif






// Setup ------------------------------------------------------------------------------------------
// ************************************************************************************************
void setup() {

#if DEBUG
  while (!SerialUSB);
  SerialUSB.begin(9600);
  delay(3000);
  SerialUSB.println("Starting Sketch");
#endif

  // Set pins
  pinMode(GREEN_LED_PIN, OUTPUT);
  pinMode(RED_LED_PIN, OUTPUT);
  pinMode(CARDSELECT, OUTPUT);
  analogReadResolution(12);

  // RTC Setup
  if (!rtc.begin()) {
    error("Couldn't Find RTC!");
  }

  // Attach Interrupt - Interrupts from RTC used instead of delay()
  attachInterrupt(RTC_ALARM_PIN, rtcISR, FALLING);

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
  SerialUSB.print("Writing to ");
  SerialUSB.println(filename);
#endif

  // Write Header - Debug header at end of setup()
  // Sensors also initiated here
  logfile.print("Year,Month,Date,Hour,Minute,Second");
#if IR
  mlx.begin();
  logfile.print(",MLX_IR_C,MLX_Amb_C");
#endif
#if UPPERSOIL
  USOIL_SER.begin(1200);
  pinMode(USOIL_POW_PIN, OUTPUT);
  digitalWrite(USOIL_POW_PIN, LOW);
  logfile.print(",Upper_Soil_Temp,Upper_Soil_Mois");
#endif
#if LOWERSOIL
  LSOIL_SER.begin(1200);
  pinMode(LSOIL_POW_PIN, OUTPUT);
  digitalWrite(LSOIL_POW_PIN, LOW);
  logfile.print(",Lower_Soil_Temp,Lower_Soil_Mois");
#endif
#if PRESSURE
  bmp.begin();
  logfile.print(",Pressure,BMP_Amb");
#endif
#if WIND
  logfile.print(",Wind_Dir,Wind_Spd");
  pinMode(WSPD_PIN, INPUT_PULLUP);
#endif
#if TEMPRH
  sht.begin(0x44);    // Set to 0x45 for alt. i2c address
  logfile.print(",SHT_Amb_C,SHT_Hum_Pct");
#endif
  logfile.println();

  // Delay and indicate start
  delay(2000);
  digitalWrite(GREEN_LED_PIN, HIGH);
  delay(1500);
  digitalWrite(GREEN_LED_PIN, LOW);

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
  SerialUSB.print("Setting RTC alarm to ");
  SerialUSB.print(deltaT);
  SerialUSB.println(" second evenly divisible start point");
  SerialUSB.print("Current Seconds: ");
  SerialUSB.print(now.second());
  SerialUSB.print(" || Setting alarm to ");
  SerialUSB.print(setSec);
  SerialUSB.println(" seconds from now. Entering Loop...");
  //  SerialUSB.print("Year,Month,Date,Hour,Minute,Second");
  SerialUSB.println();
#endif

} // End setup()






// Loop -------------------------------------------------------------------------------------------
// ************************************************************************************************
void loop() {
  // Wait for interrupt

  digitalWrite(GREEN_LED_PIN, rtcFlag);    // Turn LED off
  while (!rtcFlag);
  digitalWrite(GREEN_LED_PIN, rtcFlag);    // Turn LED on

  // Gather Measurements
  DateTime now = rtc.now();
#if IR
  mlxIR = mlx.readObjectTempC();
  mlxAmb = mlx.readAmbientTempC();
#endif
#if UPPERSOIL

#endif
#if LOWERSOIL

#endif
#if PRESSURE
  bmpAmb = bmp.readTemperature();
  pressure = bmp.readPressure();
#endif
#if TEMPRH
  shtAmb = sht.readTemperature();
  shtHum = sht.readHumidity();
#endif
#if WIND
  wDir = (float)analogRead(WDIR_PIN) * 360.0 / 4096;  // Map from analog count to 0-360, with 360=North
  startTime = millis();
  attachInterrupt(WSPD_PIN, windCounter, RISING);     // Attach interrupt for wind speed pin
  while (millis() - startTime < 2250);                // Measure number of counts in 2.5 sec
  detachInterrupt(WSPD_PIN);
  wSpd = windCount;                                   // mph = (counts)*(2.25)/(sample period in seconds). If period is 2.25 seconds, mph = counts
  wSpd = 0.447 * wSpd;                                // Convert to m/s
#endif


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
#if IR
  logfile.print(",");
  logfile.print(mlxIR);
  logfile.print(",");
  logfile.print(mlxAmb);
#endif
#if UPPERSOIL
  logfile.print(",");
  logfile.print(upperTemp);
  logfile.print(",");
  logfile.print(upperMois);
#endif
#if LOWERSOIL
  logfile.print(",");
  logfile.print(lowerTemp);
  logfile.print(",");
  logfile.print(lowerMois);
#endif
#if PRESSURE
  logfile.print(",");
  logfile.print(pressure);
  logfile.print(",");
  logfile.print(bmpAmb);
#endif
#if WIND
  logfile.print(",");
  logfile.print(wDir);
  logfile.print(",");
  logfile.print(wSpd);
#endif
#if TEMPRH
  logfile.print(", ");
  logfile.print(shtAmb);
  logfile.print(", ");
  logfile.print(shtHum);
#endif
  logfile.println();
  logfile.flush();


  // Debug Info
#if DEBUG
  SerialUSB.print(now.year(), DEC);
  SerialUSB.print(", ");
  SerialUSB.print(now.month(), DEC);
  SerialUSB.print(", ");
  SerialUSB.print(now.day(), DEC);
  SerialUSB.print(", ");
  SerialUSB.print(now.hour(), DEC);
  SerialUSB.print(", ");
  SerialUSB.print(now.minute(), DEC);
  SerialUSB.print(", ");
  SerialUSB.print(now.second(), DEC);
#if IR
  SerialUSB.print(", ");
  SerialUSB.print(mlxIR);
  SerialUSB.print(", ");
  SerialUSB.print(mlxAmb);
#endif
#if UPPERSOIL
  SerialUSB.print(", ");
  SerialUSB.print(upperTemp);
  SerialUSB.print(", ");
  SerialUSB.print(upperMois);
#endif
#if LOWERSOIL
  SerialUSB.print(", ");
  SerialUSB.print(lowerTemp);
  SerialUSB.print(", ");
  SerialUSB.print(lowerMois);
#endif
#if PRESSURE
  SerialUSB.print(",");
  SerialUSB.print(pressure);
  SerialUSB.print(",");
  SerialUSB.print(bmpAmb);
#endif
  SerialUSB.print(",");
  SerialUSB.print(wDir);
  SerialUSB.print(",");
  SerialUSB.print(wSpd);
#if TEMPRH
  SerialUSB.print(", ");
  SerialUSB.print(shtAmb);
  SerialUSB.print(", ");
  SerialUSB.print(shtHum);
#endif
  SerialUSB.println();
#endif

  // Reset any variables and turn alarm on for next measurement
  windCount = 0;
  rtcAlarm1.alarmSecondsSet(now, deltaT);
  rtcFlag = false;


} // End loop()






// Functions --------------------------------------------------------------------------------------
// ************************************************************************************************

// Turn red LED on and loop. Used in case of errors
void error(String errorMsg) {
#if DEBUG
  SerialUSB.println(errorMsg);
#endif

  digitalWrite(RED_LED_PIN, HIGH);
  while (true);
}


// ISR for RTC alarm. Used to break out of while loop used instead of delay
void rtcISR(void) {
  rtcFlag = true;
}


// ISR for Davis Anemometer. Increments wind counter with every revolution
void windCounter() {
  windCount++;
}


// Function to parse packet received from 5TM
void parse5TM(char input[], double &mois, double &temp) {
  int values[3] = {0, 0, 0};                                                    // Moisture, place holder, temperature
  int fieldIndex = 0;                                                           // Values index count
  int check = 0;                                                                // Stop byte position

  // Parse input string, store into values
  for (int j = 0; j < strlen(input); j++) {                                     
    if (input[j] >= '0' && input[j] <= '9' && fieldIndex < 3) {
      values[fieldIndex] = (values[fieldIndex] * 10) + (input[j] - '0');
    }
    else if (input[j] == ' ') {                                                 // If character is space, increment values place
      fieldIndex++;
    }
    else {                                                                      // Otherwise store length of measurement string
      check = j;
      break;
    }
  }
  values[1] = int(input[strlen(input) - 3]);                                    // Store sensor side checksum in place holder part of values, save space

  // Calculate arduino side checksum...
  char crc = 0;                                                                 
  for (int j = 0; j < check; j++) {
    crc += input[j];
  }
  crc = (crc + 0xD + 'x') % 64 + 32;

  // Calculate moisture and temperature
  mois = 0.0000043 * pow(double(values[0]) / 50.0, 3) - 0.00055 * pow(double(values[0]) / 50.0, 2) + 0.0292 * (double(values[0]) / 50.0) - 0.053; // Calculate moisture with topp equation
  if (values[2] <= 900) {                       // Calculate temperature...
    temp = (double(values[2]) - 400.0) / 10.0;
  }
  if (values[2] > 900) {
    temp = 900.0 + 5.0 * (double(values[2]) - 900.0);
    temp = (temp - 400.0) / 10.0;
  }
  if ( values[1] != crc) {
    mois = 0.0;
    temp = -273.15;
  }

  // No return. Reference to multiple variables passed in
} // end parse5TM()




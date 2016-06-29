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
//		- Wifi
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
#define GREEN_LED_PIN 13    // Green LED pin - Change LED Pins in future!!
#define RED_LED_PIN 6       // Red LED pin - Change LED Pins in future!!
#define CARDSELECT 5        // Sd card chip select pin
#define LSOILT_PIN A0       // Lower soil temperature pin
#define USOILT_PIN A1       // Upper soil temperature pin
#define LSOILM_PIN A2       // Lower soil moisture pin
#define USOILM_PIN A3       // Upper soil moisture pin
#define WDIR_PIN A4         // Davis wind direction pin
#define SUN_PIN A5          // Li200 solar radiation pin
#define RTC_ALARM_PIN 12    // DS3231 Alarm pin
#define SOIL_POW_PIN 11     // Soil transistor pin, gives power to soil sensors
#define WSPD_PIN 10         // Davis wind speed pin



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
#include <Adafruit_ADS1015.h>   // ADS1015 - https://github.com/adafruit/Adafruit_ADS1X15 
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
  while (!Serial);
  Serial.begin(9600);
  delay(3000);
  Serial.println("Starting Sketch");
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
  Serial.print("Writing to ");
  Serial.println(filename);
#endif

  // Write Header - Debug header at end of setup()
  // Sensors also initiated here
  logfile.print("Year,Month,Date,Hour,Minute,Second");
#if IR
  mlx.begin();
  logfile.print(",MLX_IR_C,MLX_Amb_C");
#endif
#if UPPERSOIL
  logfile.print(",Upper_Soil_Temp,Upper_Soil_Mois");
#endif
#if LOWERSOIL
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
  Serial.print("Setting RTC alarm to ");
  Serial.print(deltaT);
  Serial.println(" second evenly divisible start point");
  Serial.print("Current Seconds: ");
  Serial.print(now.second());
  Serial.print(" || Setting alarm to ");
  Serial.print(setSec);
  Serial.println(" seconds from now. Entering Loop...");
  //  Serial.print("Year,Month,Date,Hour,Minute,Second");
  Serial.println();
#endif

} // End setup()






// Loop -------------------------------------------------------------------------------------------
// ************************************************************************************************
void loop() {
  // Wait for interrupt

  digitalWrite(GREEN_LED_PIN, rtcFlag);    // Turn LED off - This also deactivates the soil transistor
  while (!rtcFlag);
  digitalWrite(GREEN_LED_PIN, rtcFlag);    // Turn LED on - This also activates the soil transistor

  // Gather Measurements
  DateTime now = rtc.now();
#if IR
  mlxIR = mlx.readObjectTempC();
  mlxAmb = mlx.readAmbientTempC();
#endif
#if UPPERSOIL
  upperTemp = soilTemp(analogRead(USOILT_PIN));
  upperMois = soilMois(analogRead(USOILM_PIN));
#endif
#if LOWERSOIL
  lowerTemp = soilTemp(analogRead(LSOILT_PIN));
  lowerMois = soilMois(analogRead(LSOILM_PIN));
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
  Serial.print(now.year(), DEC);
  Serial.print(", ");
  Serial.print(now.month(), DEC);
  Serial.print(", ");
  Serial.print(now.day(), DEC);
  Serial.print(", ");
  Serial.print(now.hour(), DEC);
  Serial.print(", ");
  Serial.print(now.minute(), DEC);
  Serial.print(", ");
  Serial.print(now.second(), DEC);
#if IR
  Serial.print(", ");
  Serial.print(mlxIR);
  Serial.print(", ");
  Serial.print(mlxAmb);
#endif
#if UPPERSOIL
  Serial.print(", ");
  Serial.print(upperTemp);
  Serial.print(", ");
  Serial.print(upperMois);
#endif
#if LOWERSOIL
  Serial.print(", ");
  Serial.print(lowerTemp);
  Serial.print(", ");
  Serial.print(lowerMois);
#endif
#if PRESSURE
  Serial.print(",");
  Serial.print(pressure);
  Serial.print(",");
  Serial.print(bmpAmb);
#endif
  Serial.print(",");
  Serial.print(wDir);
  Serial.print(",");
  Serial.print(wSpd);
#if TEMPRH
  Serial.print(", ");
  Serial.print(shtAmb);
  Serial.print(", ");
  Serial.print(shtHum);
#endif
  Serial.println();
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
  Serial.println(errorMsg);
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


// Function to convert analog read of RT-1 to temperature in C
// See RT-1 datahseet for information
double soilTemp(int aVal) {
  double v = (aVal / 4096.0) * 3.3; // Convert adc result to voltage - 12-bit adc and 3.3v supply
  double x = log((3.3 / v) - 1.0);
  double T = -0.1087 * pow(x, 3) + 1.6066 * pow(x, 2) - 22.801 * x + 25.0;
  return T;
}


// Function to convert analog read of EC-5 to moisture in m^3/m^3
double soilMois(int aVal) {
  double mv = (aVal / 4096.0) * 3.3 * 1000.0; // Convert adc result to mV - 12-bit adc and 3.3v supply
  double epsilon = 1.0 / (-3.3326e-9 * pow(mv, 3) + 7.01218e-6 * pow(mv, 2) - 5.11647e-3 * mv + 1.30746);
  double vwc = 4.3e-6 * pow(epsilon, 3) - 5.5e-4 * pow(epsilon, 2) + 2.92e-2 * epsilon - 5.3e-2;
}

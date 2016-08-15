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
//      Thanks to @sandeepmistry at https://github.com/arduino/ArduinoCore-samd/issues/142 for further help
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
//		- Change 3.3V to actual VCC reading from ADC
//    - Change error red light to blinking red light to save power
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
#define DEBUG 1



// Pin Defines ------------------------------------------------------------------------------------
#define GREEN_LED_PIN 6     // Green LED pin
#define RED_LED_PIN 7       // Red LED pin
#define CARDSELECT 5        // SD  card chip select pin
#define RTC_ALARM_PIN 9     // DS3231 Alarm pin
#define SUN_PIN A5          // Li200 solar radiation pin
#define WDIR_PIN A4         // Davis wind direction pin
#define BAT_PIN A3          // Battery resistor div. pin
#define WSPD_PIN 8          // Davis wind speed pin
#define USOIL_POW_PIN 2     // 5TM Power Pin, Upper - White 5TM Wire
#define LSOIL_POW_PIN 38    // 5TM Power Pin, Lower - White 5TM Wire
#define USOIL_SER Serial1   // 5TM Serial Port, Upper RX = D0, TX = D1; Red 5TM Wire
#define LSOIL_SER Serial    // 5TM Serial Port, Lower RX = D31, TX = D30; Red 5TM Wire
#define FAN_PIN 11          // Pin for cooling fan
#define ADC_RES 12          // Number of bits ADC has





// Libraries --------------------------------------------------------------------------------------
#include <SD.h>			            // SD Card Library
#include <SPI.h>		            // SPI Library
#include "d5TM.h"               // 5TM Library
#include <math.h>               // Math Library - https://www.arduino.cc/en/Math/H
#include <Wire.h>		            // I2C Library
#include <RTClib.h>		          // RTC Library - https://github.com/adafruit/RTClib
#include "DS3231_Alarm1.h"      // RTC Alarm Library Files
#include <Adafruit_SHT31.h>     // SHT31 - https://github.com/adafruit/Adafruit_SHT31
#include <Adafruit_Sensor.h>    // Necessary for BMP280 Code - https://github.com/adafruit/Adafruit_Sensor
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
double mlxIR;                                   // IR values from MLX90614
double mlxAmb;                                  // Ambient temp values from MLX90614
Adafruit_MLX90614 mlx = Adafruit_MLX90614();   // MLX90614 class
#endif

// Upper soil
#if UPPERSOIL
d5TM upperSoil(USOIL_SER, 1200, USOIL_POW_PIN);
#endif

// Lower soil
#if LOWERSOIL
d5TM lowerSoil(LSOIL_SER, 1200, LSOIL_POW_PIN);
#endif

// BMP280
#if PRESSURE
Adafruit_BMP280 bmp;        // Initialize BMP280 class
double pressure;             // Barometric pressure
double bmpAmb;               // Temperature from BMP280
#endif

// Wind
#if WIND
unsigned long startTime;            // Used to measure averaging period for wind speed
volatile unsigned long windCount;   // Counts to convert to speed
double wDir;                         // Wind direction
double wSpd;                         // Wind speed
#endif

#if SUNLIGHT
unsigned int rawSun = 0;                // Word to hold 12 bit sunlight values
const double liConst = 92.54E-6 / 1000;  // Licor Calibration Constant. Units of (Amps/(W/m^2))
const double ampResistor = 31000;        // Exact Resistor Value used by Op-Amp
double sunlight = 0.0;                   // Converted Value
#endif

// SHT21
#if TEMPRH
Adafruit_SHT31 sht = Adafruit_SHT31();
double shtAmb;               // Ambient temp values from SHT21
double shtHum;               // Relative humidity values from SHT21
#endif






// Setup ------------------------------------------------------------------------------------------
// ************************************************************************************************
void setup() {

#if DEBUG
  while (!SerialUSB);
  SerialUSB.begin(9600);
  SerialUSB.println("Starting Sketch");
#endif
  delay(3000);                                  // Allows to unplug before main code starts

  // Set pins
  pinMode(GREEN_LED_PIN, OUTPUT);
  pinMode(RED_LED_PIN, OUTPUT);
  pinMode(CARDSELECT, OUTPUT);
  analogReadResolution(ADC_RES);                // Set resolution to 12 bit

  // Set fan low in case it's plugged in but not wanting to be used
  pinMode(FAN_PIN, OUTPUT);
  digitalWrite(FAN_PIN, LOW);

  // Turn on LED during setup process
  digitalWrite(GREEN_LED_PIN, HIGH);

  // Setup registers so interrupts wake board from sleep
  SYSCTRL->VREG.bit.RUNSTDBY = 1;              // Configure regulator to run normally in standby mode, so not limited to 50uA
  SYSCTRL->DFLLCTRL.bit.RUNSTDBY = 1;          // Enable DFLL48MHz clock in standby mode
  //#if DEBUG
  //  USBDevice.detach();                          // Disable USB device to avoid USB interrupts. Necessitates double press of reset button to upload though
  //#endif                                         // Only needed when connected via native USB

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
  upperSoil.begin();
  logfile.print(",Upper_Soil_Temp,Upper_Soil_Mois");
#endif
#if LOWERSOIL
  lowerSoil.begin();
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
#if SUNLIGHT
  pinMode(SUN_PIN, INPUT);      // Make sure ADC pin in correct state
  digitalWrite(SUN_PIN, LOW);
  logfile.print(",Sunlight");
#endif
#if TEMPRH
  sht.begin(0x44);    // Set to 0x45 for alt. i2c address
  logfile.print(",SHT_Amb_C,SHT_Hum_Pct");
#endif
  logfile.println();

  // Delay and indicate start
  delay(3500);
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
#if DEBUG
  while (!rtcFlag);                        // Use while loop to avoid sleep states or debugging
#else
  standbySleep();                          // Use standbySleep() to reduce power consumption
#endif

  // Gather Measurements
  DateTime now = rtc.now();
#if IR
  mlxIR = mlx.readObjectTempC();
  mlxAmb = mlx.readAmbientTempC();
#endif
#if UPPERSOIL
  upperSoil.getMeasurements();
#endif
#if LOWERSOIL
  lowerSoil.getMeasurements();
#endif
#if PRESSURE
  bmpAmb = bmp.readTemperature();
  pressure = bmp.readPressure();
#endif
#if SUNLIGHT
  rawSun = analogRead(SUN_PIN);
  sunlight = double(rawSun) * (3.3 / double(pow(2, ADC_RES))) * (1.0 / ampResistor) * (1.0 / liConst); // Convert to W/m^2
#endif
#if TEMPRH
  shtAmb = sht.readTemperature();
  shtHum = sht.readHumidity();
#endif
#if WIND
  wDir = (double)analogRead(WDIR_PIN) * 360.0 / pow(2, ADC_RES);  // Map from analog count to 0-360, with 360=North
  startTime = millis();
  attachInterrupt(WSPD_PIN, windCounter, RISING);                 // Attach interrupt for wind speed pin
  while (millis() - startTime < 2250);                            // Measure number of counts in 2.5 sec
  detachInterrupt(WSPD_PIN);
  wSpd = windCount;                                               // mph = (counts)*(2.25)/(sample period in seconds). If period is 2.25 seconds, mph = counts
  wSpd = 0.447 * wSpd;                                            // Convert to m/s
#endif


  // Log Data
  digitalWrite(GREEN_LED_PIN, rtcFlag);    // Turn LED on
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
  logfile.print(upperSoil.temperature);
  logfile.print(",");
  logfile.print(upperSoil.moisture);
#endif
#if LOWERSOIL
  logfile.print(",");
  logfile.print(lowerSoil.temperature);
  logfile.print(",");
  logfile.print(lowerSoil.moisture);
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
#if SUNLIGHT
  logfile.print(",");
  logfile.print(sunlight);
#endif
#if TEMPRH
  logfile.print(",");
  logfile.print(shtAmb);
  logfile.print(",");
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
  SerialUSB.print(upperSoil.temperature);
  SerialUSB.print(", ");
  SerialUSB.print(upperSoil.moisture);
#endif
#if LOWERSOIL
  SerialUSB.print(", ");
  SerialUSB.print(lowerSoil.temperature);
  SerialUSB.print(", ");
  SerialUSB.print(lowerSoil.moisture);
#endif
#if PRESSURE
  SerialUSB.print(", ");
  SerialUSB.print(pressure);
  SerialUSB.print(", ");
  SerialUSB.print(bmpAmb);
#endif
#if WIND
  SerialUSB.print(", ");
  SerialUSB.print(wDir);
  SerialUSB.print(", ");
  SerialUSB.print(wSpd);
#endif
#if SUNLIGHT
SerialUSB.print(", ");
SerialUSB.print(sunlight);
#endif
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
#if WIND
void windCounter() {
  windCount++;
}
#endif


// Function to put SAMD21 into standby sleep
void standbySleep(void) {
  SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;
  __WFI();
}



// Get Apogee SL-610 working

// Libraries 
#include <math.h>
#include <Wire.h>
#include <Adafruit_BMP280.h>    // BMP280 used to get initial temp to calculate thermistor temp
#include <Adafruit_ADS1015.h>

// Defines
#define ADC_RES 12          // Number of bits Arduino ADC has
#define ADS_VCC_PIN 1       // VCC pin on ADS1115
#define THERM_PIN A3        // SL-610 Thermistor output
#define THERM_POW 3         // Thermistor power pin
// Positive thermopile pin is in AIN2, negative in AIN3. Serial Number: SL-610-SS_1034 !! USES NEW WIRING COLORS !!

// BMP280
Adafruit_BMP280 bmp;      // Initialize BMP280 class
double bmpAmb;            // Temperature from BMP280
double approxAlt;         // Approximate altitude from BMP

// Generic variables
Adafruit_ADS1115 ads;     // Use this for the 16-bit version
double vcc;               // VCC 

// Thermistor variables
double thermV;            // Voltage reading from arduino ADC for thermistor (volts)
double Rt;                // Thermistor resistance (ohm)
double thermTemp;         // Temperature as measured by thermistor (Kelvin)
double Agz = 9.32793534266128e-4;  // Greater than 0 C thermistor constants
double Bgz = 2.21450736014070e-4;  // "
double Cgz = 1.26232582309837e-7;  // "
double Alz = 9.32959957496852e-4;  // Less than 0 C thermistor constants
double Blz = 2.21423593265217e-4;  // "
double Clz = 1.26328669787011e-7;  // "
unsigned int rawTemp = 0;          // Analog read from thermistor

// Thermopile Constants & variables
//double k1 = 9.712;     // Wm^-2 per mV - Serial number 1034
//double k2 = 1.028;     // Unitless - Serial number 1034
double k1 = 10.023;      // Serial number 1038
double k2 = 1.020;       // "
double pilemV;         // Voltage from thermopile
double LWi;            // Incoming longwave radiation

  


void setup(void) {
  SerialUSB.begin(9600);
  SerialUSB.println("Starting Test");

  analogReadResolution(ADC_RES);                // Set arduino resolution to 12 bit - only works on ARM arduino boards

  pinMode(THERM_POW, OUTPUT);
  digitalWrite(THERM_POW, LOW);

  bmp.begin();                  // Start BMP280

  ads.setGain(GAIN_SIXTEEN);    // 16x gain  +/- 0.256V  0.0078125mV/bit
  ads.begin();
}




void loop(void) {

  // Read misc variables
  ads.setGain(GAIN_ONE);
  vcc = ads.readADC_SingleEnded_V(ADS_VCC_PIN);       // VCC connected to AIN1
  ads.setGain(GAIN_SIXTEEN);
  bmpAmb = bmp.readTemperature();                     // Temp from BMP280
  approxAlt = bmp.readAltitude(1013.25);              // Technically should be adjusted to local sea level pressure

  // Read thermistor
  digitalWrite(THERM_POW, HIGH);
  rawTemp = analogRead(THERM_PIN);
  digitalWrite(THERM_POW, LOW);
  thermV = double(rawTemp) / (pow(2,ADC_RES)-1) * vcc;
  Rt = 24900.0 * thermV / (vcc - thermV);
  if(bmpAmb >= 0){
    thermTemp = 1.0 / (Agz + Bgz*log(Rt) + Cgz*pow(log(Rt), 3));    // Steinhart-hart Equation
  } else {
    thermTemp = 1.0 / (Alz + Blz*log(Rt) + Clz*pow(log(Rt), 3));   
  }

  // Read thermopile
  pilemV = ads.readADC_Differential_2_3_V()*1000.0;   // Positive thermopile pin is in AIN2, negative in AIN3.
  LWi = k1*pilemV + k2*5.6704e-8*pow(thermTemp, 4);

  // Print
  SerialUSB.print("Approximate Altitude (m): ");
  SerialUSB.println(approxAlt);
  SerialUSB.print("VCC: ");
  SerialUSB.println(vcc);
  SerialUSB.print("Thermistor temperature (C): ");
  SerialUSB.println(thermTemp - 273.15);
  SerialUSB.print("Thermopile voltage (mV): ");
  SerialUSB.println(pilemV, 6);
  SerialUSB.print("Longwave Radiation (Wm^-2): ");
  SerialUSB.println(LWi);
  SerialUSB.println();

  delay(2000);
}

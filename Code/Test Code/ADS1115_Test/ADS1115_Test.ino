#include <Wire.h>
#include <Adafruit_ADS1015.h>

Adafruit_ADS1115 ads;  /* Use this for the 16-bit version */
const float vDiv = 0.000125;   // Volts/div when gain = 1

void setup(void)
{
  SerialUSB.begin(9600);
  SerialUSB.println("Starting Test");
  SerialUSB.println("Getting single-ended readings from AIN0..3");

  //  Serial.begin(9600);
  //  Serial.println("Starting Test");
  //  Serial.println("Getting single-ended readings from AIN0..3");

  // The ADC input range (or gain) can be changed via the following
  // functions, but be careful never to exceed VDD +0.3V max, or to
  // exceed the upper and lower limits if you adjust the input range!
  // Setting these values incorrectly may destroy your ADC!
  //                                                                ADS1015  ADS1115
  //                                                                -------  -------
  // ads.setGain(GAIN_TWOTHIRDS);  // 2/3x gain +/- 6.144V  1 bit = 3mV      0.1875mV (default)
  ads.setGain(GAIN_ONE);        // 1x gain   +/- 4.096V  1 bit = 2mV      0.125mV
  // ads.setGain(GAIN_TWO);        // 2x gain   +/- 2.048V  1 bit = 1mV      0.0625mV
  // ads.setGain(GAIN_FOUR);       // 4x gain   +/- 1.024V  1 bit = 0.5mV    0.03125mV
  // ads.setGain(GAIN_EIGHT);      // 8x gain   +/- 0.512V  1 bit = 0.25mV   0.015625mV
  // ads.setGain(GAIN_SIXTEEN);    // 16x gain  +/- 0.256V  1 bit = 0.125mV  0.0078125mV
  ads.begin();
}

void loop(void)
{
  uint16_t adc0, adc1, adc2, adc3;

  adc0 = ads.readADC_SingleEnded(0);
  adc1 = ads.readADC_SingleEnded(1);
  adc2 = ads.readADC_SingleEnded(2);
  adc3 = ads.readADC_SingleEnded(3);

  SerialUSB.print("AIN0: "); SerialUSB.print(adc0);
  SerialUSB.print(" || "); SerialUSB.println(float(adc0)*vDiv);

  SerialUSB.print("AIN1: "); SerialUSB.print(adc1);
  SerialUSB.print(" || "); SerialUSB.println(float(adc1)*vDiv);

  SerialUSB.print("AIN2: "); SerialUSB.print(adc2);
  SerialUSB.print(" || "); SerialUSB.println(float(adc2)*vDiv);

  SerialUSB.print("AIN3: "); SerialUSB.print(adc3);
  SerialUSB.print(" || "); SerialUSB.println(float(adc3)*vDiv);
  SerialUSB.println(" ");

  //  Serial.print("AIN0: "); Serial.print(adc0);
  //  Serial.print(" || "); Serial.println(float(adc0)*vDiv);
  //
  //  Serial.print("AIN1: "); Serial.print(adc1);
  //  Serial.print(" || "); Serial.println(float(adc1)*vDiv);
  //
  //  Serial.print("AIN2: "); Serial.print(adc2);
  //  Serial.print(" || "); Serial.println(float(adc2)*vDiv);
  //
  //  Serial.print("AIN3: "); Serial.print(adc3);
  //  Serial.print(" || "); Serial.println(float(adc3)*vDiv);
  //  Serial.println(" ");

  delay(2000);
}

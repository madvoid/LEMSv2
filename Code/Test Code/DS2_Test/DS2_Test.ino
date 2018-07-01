// DS2_Test.ino
// Nipun Gunawardena
// Test DS2 and DS2 Library
// Todo:

#include "DS2.h"

#define DATA_PIN 10

DS2 anem(DATA_PIN, '0');

void setup() {
  delay(3000);  // "Warm up" time, gives time for port to open
  SerialUSB.begin(9600);
  SerialUSB.println("Initializing Wind Class");
  anem.begin();
  SerialUSB.println("Press enter to continue...");
  while (!SerialUSB.available());
  SerialUSB.read();
}

void loop() {
  digitalWrite(13, HIGH);
  anem.getMeasurements();
  digitalWrite(13, LOW);
  SerialUSB.print("Raw: ");
  SerialUSB.println(anem.dsResponse);
  SerialUSB.print("Wind Speed: ");
  SerialUSB.println(anem.wSpd);
  SerialUSB.print("Wind Direction: ");
  SerialUSB.println(anem.wDir);
  SerialUSB.print("Wind Gust: ");
  SerialUSB.println(anem.wGst);
  SerialUSB.print("Wind Temperature: ");
  SerialUSB.println(anem.wTmp);
  SerialUSB.println();

  delay(10000);
}

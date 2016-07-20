// Test_5TM.ino
// Nipun Gunawardena
// Test 5TM and 5TM library
// Todo:

#include "d5TM.h"

d5TM soil(Serial, 1200);

void setup() {
  pinMode(13, OUTPUT);
  SerialUSB.begin(9600);
  SerialUSB.println("Initializing Soil Class");
  soil.begin(38);
}



void loop() {
  digitalWrite(13, HIGH);
  soil.getMeasurements();
  digitalWrite(13, LOW);
  SerialUSB.print("Temperature: ");
  SerialUSB.print(soil.temperature);
  SerialUSB.print(" :|: Moisture: ");
  SerialUSB.println(soil.moisture);
  SerialUSB.println();

  delay(5000);
}


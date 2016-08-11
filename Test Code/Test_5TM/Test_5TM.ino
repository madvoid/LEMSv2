// Test_5TM.ino
// Nipun Gunawardena
// Test 5TM and 5TM library
// Todo:

#include "d5TM.h"

d5TM soil(Serial, 1200, 38);
d5TM soil1(Serial1, 1200, 2);

void setup() {
  pinMode(13, OUTPUT);
  SerialUSB.begin(9600);
  SerialUSB.println("Initializing Soil Class");
  //  Serial.begin(9600);
  //  Serial.println("Initializing Soil Class");
  soil.begin();
  soil1.begin();
}



void loop() {
  digitalWrite(13, HIGH);
  soil.getMeasurements();
  soil1.getMeasurements();
  digitalWrite(13, LOW);
  SerialUSB.print("Serial2 :: ");
  SerialUSB.print("Temperature: ");
  SerialUSB.print(soil.temperature);
  SerialUSB.print(" :|: Moisture: ");
  SerialUSB.println(soil.moisture);
  SerialUSB.print("Serial1 :: ");
  SerialUSB.print("Temperature: ");
  SerialUSB.print(soil1.temperature);
  SerialUSB.print(" :|: Moisture: ");
  SerialUSB.println(soil1.moisture);
  SerialUSB.println();
  //  Serial.print("Serial2 :: ");
  //  Serial.print("Temperature: ");
  //  Serial.print(soil.temperature);
  //  Serial.print(" :|: Moisture: ");
  //  Serial.println(soil.moisture);
  //  Serial.print("Serial1 :: ");
  //  Serial.print("Temperature: ");
  //  Serial.print(soil1.temperature);
  //  Serial.print(" :|: Moisture: ");
  //  Serial.println(soil1.moisture);
  //  Serial.println();

  delay(5000);
}


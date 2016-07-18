// Test_5TM.ino
// Nipun Gunawardena
// Test 5TM and 5TM library
// Todo:

#include "d5TM.h"

d5TM soil(Serial1, 1200);

void setup(){
  SerialUSB.begin(9600);
  SerialUSB.println("Initializing Soil Class");
  soil.begin(2);  
}



void loop(){
  soil.getMeasurements();
  SerialUSB.print("Temperature: ");
  SerialUSB.print(soil.temperature);
  SerialUSB.print(" :|: Moisture: ");
  SerialUSB.print(soil.moisture);
  SerialUSB.println();
  
  delay(5000);
}


// soil_test.ino
// Nipun Gunawardena
// Test the soil temperature sensor

#include <math.h>

unsigned int aVal = 0;
double volts = 0.0;
double x = 0.0;
double temp = 0.0;

void setup() {
  analogReadResolution(12);
  Serial.begin(9600);
  Serial.println("Starting Loop...");
}

void loop() {
  aVal = analogRead(A0);
  Serial.print("Raw: ");
  Serial.println(aVal);

  volts = (aVal / 4096.0) * 3.3;
  Serial.print("Volts: ");
  Serial.println(volts);

  x = log((3.3 / volts) - 1.0);
  Serial.print("x: ");
  Serial.println(x);

  temp = -0.1087 * pow(x, 3) + 1.6066 * pow(x, 2) - 22.801 * x + 25.0;
  Serial.print("Temp: ");
  Serial.println(temp);
  Serial.println();
  
  delay(2000);
}

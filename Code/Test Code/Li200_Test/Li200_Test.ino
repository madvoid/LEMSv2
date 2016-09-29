// Li200_Test.ino
// Nipun Gunawardena
// Test Li200 Sensor

#define SUN_PIN A5
#define ADC_RES 12

unsigned int rawSun = 0;                  // Word to hold 12 bit sunlight values
const double liConst = 92.54E-6 / 1000;  // Licor Calibration Constant. Units of (Amps/(W/m^2))
const double ampResistor = 31000;        // Exact Resistor Value used by Op-Amp
double sunlight = 0.0;                   // Converted Value



void setup() {
  while (!SerialUSB);
  SerialUSB.begin(9600);
  analogReadResolution(ADC_RES);                // Set resolution to 12 bit
  SerialUSB.println("Starting loop");
  pinMode(SUN_PIN, INPUT);
  digitalWrite(SUN_PIN, LOW);
}



void loop() {
  rawSun = analogRead(SUN_PIN);
  double volts = double(rawSun)*(3.3/double(pow(2, ADC_RES)));
  double amps = volts / ampResistor;
  sunlight =  amps / liConst; // Convert to W/m^2
  SerialUSB.print("Raw (Bits): ");
  SerialUSB.println(rawSun);
  SerialUSB.print("Voltage Read (V): ");
  SerialUSB.println(volts, 4);
  SerialUSB.print("Sensor Current (uA): ");
  SerialUSB.println(amps*1e6, 9);
  SerialUSB.print("Sunlight (W/m^2): ");
  SerialUSB.println(sunlight, 4);  

  SerialUSB.println();
  delay(1000);
}

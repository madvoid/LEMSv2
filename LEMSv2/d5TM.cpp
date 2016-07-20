// ------------------------------------------------------------------------------------------------
// Filename:
//    d5TM.cpp
//
// Author:
//    Nipun Gunawardena
//
// Description:
//    Simple library to read and parse Decagon 5TM
//
// Credits:
//
// Requirements:
//    - Decagon 5TM sensor
//    - d5TM.cpp
//
// Notes:
//    None for now
//
// Todo:
//    -
//
// ------------------------------------------------------------------------------------------------






// Includes and Defines - Boilerplate from Adafruit Libraries -------------------------------------

#include "d5TM.h"

#ifdef __AVR__
#include <avr/pgmspace.h>
#elif defined(ESP8266)
#include <pgmspace.h>
#elif defined(ARDUINO_ARCH_SAMD)
// nothing special needed
#elif defined(ARDUINO_SAM_DUE)
#define PROGMEM
#define pgm_read_byte(addr) (*(const unsigned char *)(addr))
#define Wire Wire1
#endif

#if (ARDUINO >= 100)
#include <Arduino.h> // capital A so it is error prone on case-sensitive filesystems
// Macro to deal with the difference in I2C write functions from old and new Arduino versions.
#define _I2C_WRITE write
#define _I2C_READ  read
#else
#include <WProgram.h>
#define _I2C_WRITE send
#define _I2C_READ  receive
#endif






// Functions --------------------------------------------------------------------------------------






// Class Functions --------------------------------------------------------------------------------
d5TM::d5TM(HardwareSerial &port, int bdRate): serPort(port) {
  baudRate = bdRate;
}



void d5TM::begin(int powPin) {
  powerPin = powPin;
  pinMode(powerPin, OUTPUT);
  digitalWrite(powerPin, LOW);
  serPort.begin(baudRate);
}



int d5TM::getMeasurements(void) {
  // Power sensor and wait 200ms for reading
  digitalWrite(powerPin, HIGH);
  delay(200);

  // Initialize variables
  int i = 0;                        // Position counter

  // Read sensor output
  if (serPort.peek() == 0x00) {
    serPort.read();
  }
  char input[serPort.available()];    // Array to store sensor output
  while (serPort.available() > 0) {
    input[i] = serPort.read();
    i++;                              
  }
  // After this point, i is used as the length of the string sent

  // Power off sensor
  digitalWrite(powerPin, LOW);      // Turn 5TM off

  // Initialize variables used to parse
  int values[3] = {0, 0, 0};       // Moisture, checksum, temperature
  int fieldIndex = 0;              // Values index count
  int check = 0;                   // Stop byte position

  // Parse input string according to datasheet specs
  for (int j = 0; j < i; j++) {
    if (input[j] >= '0' && input[j] <= '9' && fieldIndex < 3) {
      values[fieldIndex] = (values[fieldIndex] * 10) + (input[j] - '0');
    }
    else if (input[j] == ' ') {
      fieldIndex++;
    }
    else {
      check = j;
      break;
    }
  }

  // Calculate checksum
  values[1] = int(input[i - 3]);        // Store sensor side checksum
  char crc = 0;                                     // Calculate arduino side checksum...
  for (int j = 0; j < check; j++) {
    crc += input[j];
  }
  crc = (crc + 0xD + 'x') % 64 + 32;

  // Calculate soil moisture using Topp Equation
  // TODO: Make more modular - Add ability to pass soil moisture function in
  moisture = 0.0000043 * pow(double(values[0]) / 50.0, 3) - 0.00055 * pow(double(values[0]) / 50.0, 2) + 0.0292 * (double(values[0]) / 50.0) - 0.053; // Calculate moisture with topp equation

  // Calculate soil temperature using datasheet equation
  if (values[2] <= 900) {
    temperature = (double(values[2]) - 400.0) / 10.0;
  }
  if (values[2] > 900) {
    temperature = 900.0 + 5.0 * (double(values[2]) - 900.0);
    temperature = (temperature - 400.0) / 10.0;
  }

  // If Checksum doesn't match, return bad numbers
  if ( values[1] != crc) {
    moisture = -1.0;
    temperature = -273.15;
  }

  return 0;
}


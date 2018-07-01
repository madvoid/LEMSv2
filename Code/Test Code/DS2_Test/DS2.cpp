// ------------------------------------------------------------------------------------------------
// Filename:
//    DS2.cpp
//
// Author:
//    Nipun Gunawardena
//
// Description:
//    Simple library to read and parse Decagon DS2
//
// Credits:
//
// Requirements:
//    - Decagon DS2 Anemometer
//    - DS2.h
//
// Notes:
//    None for now
//
// Todo:
//
// ------------------------------------------------------------------------------------------------






// Includes and Defines - Boilerplate from Adafruit Libraries -------------------------------------

#include "DS2.h"

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
DS2::DS2(uint8_t datPin, char addr): ds(datPin) {
  dataPin = datPin;
  address = address + addr;
}



void DS2::begin(void) {
  ds.begin();
  getVersion();
}



void DS2::sendCommand(unsigned int delayTime) {
  // Clear variables for use
  dsResponse = "";
  ds.flush();

  // Send command and wait for response
  ds.sendCommand(command);
  delay(delayTime);
  ds.forceHold();

  // Get bytes from sensor
  while (ds.available()) {
    char c = ds.read();
    if ( (c != '\n') && (c != '\r') ) {
      dsResponse += c;
      delay(5);
    }
  }
}



void DS2::getVersion(void) {
  // Make command to send
  command = address + "I!";

  // Send command
  sendCommand(380);   // Don't know recommended time, assume it's immediate but have buffer

  // Trim whitespace
  dsResponse.trim();

  // Check manufacturer first letter
  versionCode = dsResponse.substring(3, 5);
}



int DS2::getMeasurements(void) {
  // Make command to send
  command = address + "R0!";

  // Send command
  sendCommand(380);   // 380 ms is recommended wait time

  // Parse string
  parseDataString();

  return 0;
}



int DS2::parseDataString(void) {
  // Ensure whitespace is removed
  dsResponse.trim();

  // Simple error check - add more checks?
  if (dsResponse.length() < 3) {
    wSpd = -1.0;
    wDir = 1000.0;
    wTmp = -273.15;
    return 1;
  }

  // Initialize variables
  char c;
  uint8_t j = 0;

  // Decagon version
  if (versionCode == "DE") {

    // Iterate through string and get signs and start indices
    for (int i = 0; i < dsResponse.length(); i++) {
      c = dsResponse[i];
      if ( (c == '+') || (c == '-')) {
        idx[j] = i;
        if (c == '+') {
          pos[j] = 1;
        } else {
          pos[j] = -1;
        }
        j++;
      } else {
        continue;
      }
    }

    // Convert to individual values
    String wSpdStr = dsResponse.substring(idx[0], idx[1]);
    String wDirStr = dsResponse.substring(idx[1], idx[2]);
    String wTmpStr = dsResponse.substring(idx[2]);
    wSpd = wSpdStr.toFloat();
    wDir = wDirStr.toFloat();
    wGst = 0.0/0.0;
    wTmp = wTmpStr.toFloat();

  } else if (versionCode == "ME") {   // Meter version

    // Iterate through string and get signs and start indices
    for (int i = 0; i < dsResponse.length(); i++) {
      c = dsResponse[i];
      if ( (c == '+') || (c == '-')) {
        idx[j] = i;
        if (c == '+') {
          pos[j] = 1;
        } else {
          pos[j] = -1;
        }
        j++;
        if (j > 4) {  // Need next separator to end temp substring
          break;
        }
      } else {
        continue;
      }
    }

    // Convert to individual values - Different order for Atmos 22 than DS2
    String wSpdStr = dsResponse.substring(idx[0], idx[1]);
    String wDirStr = dsResponse.substring(idx[1], idx[2]);
    String wGstStr = dsResponse.substring(idx[2], idx[3]);
    String wTmpStr = dsResponse.substring(idx[3], idx[4]);
    wSpd = wSpdStr.toFloat();
    wDir = wDirStr.toFloat();
    wGst = wGstStr.toFloat();
    wTmp = wTmpStr.toFloat();


  } else {
    wSpd = -2.0;
    wDir = 2000.0;
    wTmp = -300.15;
    return 1;
  }


  // Add more checks?

  return 0;
}


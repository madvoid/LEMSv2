// ------------------------------------------------------------------------------------------------
// Filename:
//    DS3231_Alarm1.cpp
//
// Author:
//    Nipun Gunawardena
//
// Description:
//    Simple library to set and manipulate alarm1 on DS3231
//
// Credits:
//    Thanks to Adafruit's RTClib. Many components of this library are heavily borrowed from that
//    one
//
// Requirements:
//    - DS3231 Hardware
//    - DS3231_Alarm.h
//
// Notes:
//    None for now
//
// Todo:
//    -
//
// ------------------------------------------------------------------------------------------------






// Includes and Defines - Boilerplate from Adafruit Libraries -------------------------------------
#include <Wire.h>
#include "DS3231_Alarm1.h"
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
// read/write_i2c_register taken from adafruit libraries
static uint8_t read_i2c_register(uint8_t addr, uint8_t reg) {
  Wire.beginTransmission(addr);
  Wire._I2C_WRITE((byte)reg);
  Wire.endTransmission();

  Wire.requestFrom(addr, (byte)1);
  return Wire._I2C_READ();
}

static void write_i2c_register(uint8_t addr, uint8_t reg, uint8_t val) {
  Wire.beginTransmission(addr);
  Wire._I2C_WRITE((byte)reg);
  Wire._I2C_WRITE((byte)val);
  Wire.endTransmission();
}

// Also taken from adafruit libraries
static uint8_t bcd2bin (uint8_t val) { return val - 6 * (val >> 4); }
static uint8_t bin2bcd (uint8_t val) { return val + 6 * (val / 10); }






// Class Functions --------------------------------------------------------------------------------
DS3231_Alarm1::DS3231_Alarm1(void) {
  // Class constructor. As of now, does nothing
}


void DS3231_Alarm1::alarmOn(void) {
  // Clear Alarm1 bits
  uint8_t statReg = read_i2c_register(DS3231_ADDRESS, DS3231_REG_STATUS_CTL);
  statReg &= ~B00000001;      // Set A1F off
  write_i2c_register(DS3231_ADDRESS, DS3231_REG_STATUS_CTL, statReg);

  // Turn DS3231 Alarm1 On
  uint8_t ctrlReg = read_i2c_register(DS3231_ADDRESS, DS3231_REG_CONTROL);
  ctrlReg |= B00000101;      // Set INTCN and A1IE on
  write_i2c_register(DS3231_ADDRESS, DS3231_REG_CONTROL, ctrlReg);
}


void DS3231_Alarm1::alarmOff(void) {
  // Clear Alarm1 bits
  uint8_t statReg = read_i2c_register(DS3231_ADDRESS, DS3231_REG_STATUS_CTL);
  statReg &= ~B00000001;      // Set A1F off
  write_i2c_register(DS3231_ADDRESS, DS3231_REG_STATUS_CTL, statReg);

  // Turn DS3231 Alarm1 Off
  uint8_t ctrlReg = read_i2c_register(DS3231_ADDRESS, DS3231_REG_CONTROL);
  ctrlReg &= ~B00000001;      // Set A1IE off
  write_i2c_register(DS3231_ADDRESS, DS3231_REG_CONTROL, ctrlReg);
}


void DS3231_Alarm1::alarmSecondsSet(DateTime currentTime, int32_t delSeconds) {
  // Set alarm 1 to match seconds only
  DateTime setTime(currentTime + TimeSpan(delSeconds));
  uint8_t setSeconds = bin2bcd(setTime.second());

  // Send seconds data
  Wire.beginTransmission(DS3231_ADDRESS);
  Wire._I2C_WRITE((byte)DS3231_REG_A1SECONDS);
  Wire._I2C_WRITE((byte)(setSeconds & 0x7F));   // Send seconds
  Wire._I2C_WRITE((byte)0x80);                  // Send masks so alarm rings on seconds
  Wire._I2C_WRITE((byte)0x80);
  Wire._I2C_WRITE((byte)0x80);
  Wire.endTransmission();

  // Clear Alarm1 bits
  uint8_t statReg = read_i2c_register(DS3231_ADDRESS, DS3231_REG_STATUS_CTL);
  statReg &= ~B00000001;      // Set A1F off
  write_i2c_register(DS3231_ADDRESS, DS3231_REG_STATUS_CTL, statReg);
}


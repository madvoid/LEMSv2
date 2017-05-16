// ------------------------------------------------------------------------------------------------
// Filename:
//    DS2.h
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
//    - DS2.cpp
//
// Notes:
//    None for now
//
// Todo:
//
// ------------------------------------------------------------------------------------------------


#ifndef _DS2_H_
#define _DS2_H_


// Includes ---------------------------------------------------------------------------------------
#include <Arduino.h>
#include "SDI12_Zero.h"



// Defines ----------------------------------------------------------------------------------------



// Classes ----------------------------------------------------------------------------------------
class DS2 {
    uint8_t dataPin;
    String address;
    SDI12 ds;
    int parseDataString(void);
    uint8_t idx[3];
    int pos[3]; 
  public:
    DS2(uint8_t datPin, char addr = '0');
    void begin(void);
    int getMeasurements(void);
    double wSpd, wDir, wTmp;
    String dsResponse;
    String command;
  protected:
};


#endif // _DS2_H_

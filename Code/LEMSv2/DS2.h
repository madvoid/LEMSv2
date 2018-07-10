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
//    There are two models, DS2 and Atmos22. These are the old and new version of the same anemometer, respectively
//    The code for the DS2 is not compatible with the Atmos 22, hence the if/else statements
//
// Todo:
//
// ------------------------------------------------------------------------------------------------


#ifndef _DS2_H_
#define _DS2_H_


// Includes ---------------------------------------------------------------------------------------
#include <Arduino.h>
#include <SDI12.h>
//#include "SDI12_Zero.h"



// Defines ----------------------------------------------------------------------------------------



// Classes ----------------------------------------------------------------------------------------
class DS2 {
    String address;
    
    SDI12 ds;
    
    int parseDataString(void);

    uint8_t idx[5];
    uint8_t dataPin;
    
    void sendCommand(unsigned int delayTime);
    void getVersion(void);
    
  public:
    DS2(uint8_t datPin, char addr = '0');
    
    void begin(void);
    
    int getMeasurements(void);
    
    double wSpd;  // Wind speed
    double wDir;  // Wind direction
    double wTmp;  // Sonic temp
    double wGst;  // Wind gust - only on Atmos 22, not DS2
    
    String dsResponse;
    String command;
    String versionCode;
    
  protected:
};


#endif // _DS2_H_

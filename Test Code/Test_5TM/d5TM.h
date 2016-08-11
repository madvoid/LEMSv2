// ------------------------------------------------------------------------------------------------
// Filename:
//    d5TM.h
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
//    - Previously done with functions. Library simplifies
//
// Todo:
//
// ------------------------------------------------------------------------------------------------


#ifndef _d5TM_H_
#define _d5TM_H_


// Includes ---------------------------------------------------------------------------------------
#include <Arduino.h>



// Defines ----------------------------------------------------------------------------------------



// Classes ----------------------------------------------------------------------------------------
class d5TM {
    int baudRate;
    int powerPin;
    HardwareSerial &serPort;
  public:
    d5TM(HardwareSerial &port, int bdRate, int powPin);
    void begin(void);
    int getMeasurements(void);
    double moisture, temperature;
  protected:
};


#endif // _d5TM_H_

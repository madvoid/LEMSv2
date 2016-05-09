// ------------------------------------------------------------------------------------------------
// Filename:
//    DS3231_Alarm1.h
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
//    - DS3231_Alarm.cpp
//
// Notes:
//    Changes made to DS3231_Alarm.h need to be repeated in DS3231_Alarm.cpp file
//    The only reason a class was made was for future expandability. Could easily be done with functions
//
// Todo:
//    - For "library" to be complete, class should be expanded and functions should be generalized
//
// ------------------------------------------------------------------------------------------------


#ifndef _DS3231_ALARM1_H_
#define _DS3231_ALARM1_H_


// Includes ---------------------------------------------------------------------------------------
#include <Arduino.h>
#ifndef _RTCLIB_H_
#include <RTClib.h>
#endif



// Defines ----------------------------------------------------------------------------------------
// Some are already defined in RTClib
#define DS3231_REG_SECONDS    0x00
#define DS3231_REG_MINUTES    0x01
#define DS3231_REG_HOURS      0x02
#define DS3231_REG_DAYOFWEEK  0x03
#define DS3231_REG_DAYOFMONTH 0x04
#define DS3231_REG_MONTH      0x05
#define DS3231_REG_YEAR       0x06
#define DS3231_REG_A1SECONDS  0x07
#define DS3231_REG_A1MINUTES  0x08
#define DS3231_REG_A1HOURS    0x09
#define DS3231_REG_A1DAYDATE  0x0A
#define DS3231_REG_A2MINUTES  0x0B
#define DS3231_REG_A2HOURS    0x0C
#define DS3231_REG_A2DAYDATE  0x0D
#define DS3231_REG_CONTROL    0x0E
#define DS3231_REG_STATUS_CTL 0x0F
#define DS3231_REG_AGING      0x10
#define DS3231_REG_TEMP_MSB   0x11
#define DS3231_REG_TEMP_LSB   0x12
#define DS3231_TIME_LENGTH    0x07



// Classes ----------------------------------------------------------------------------------------
class DS3231_Alarm1 {
  public:
    DS3231_Alarm1(void);
    void alarmOn(void);
    void alarmOff(void);
    void alarmSecondsSet(DateTime currentTime, int32_t delSeconds);

  protected:
};


#endif // _DS3231_ALARM1_H_

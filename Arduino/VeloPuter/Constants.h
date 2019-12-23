#include <Arduino.h>
#include <U8glib.h>

#include "VeloPuter_Config.h"

// now via VeloPuter_Config.h
//#define ICB_DF   // Configuration management: STRADA QUILTJE QUATRO and ICB_DF
//#define BATTERY_LIFEPO4 // Battry type BATTERY_LIPO BATTERY_LIFEPO4

#define Ug82

/* Arduino Micro pin configuration/ capabilities

    See also: https://www.arduino.cc/en/Main/ArduinoBoardMicro

  20 Digital input/outputs
  Intrupts: on 5 lines.
  PWM: on 7 lines
  Analog in: 12 lines

  0          INT     (Serial RX)
  1          INT     (Serial TX)
  2          INT     (TWI SDA)
  3      PWM INT     (TWI SCL)
  4/A6           ANA
  5      PWM
  6/A7   PWM     ANA
  7          INT
  8/A8           ANA
  9/A9   PWM     ANA     // Note that Timer 1 does break the pwm in this pin
  10/A10  PWM     ANA     // Note that Timer 1 does break the pwm in this pin
  11      PWM
  12/A11          ANA
  13      PWM
  A0              ANA
  A1              ANA
  A2              ANA
  A3              ANA
  A4              ANA
  A5              ANA



*/

const byte switchCadPin =       0;      // NOTE: 0 and 1 are the RX and TX pins.
const byte switchSpdPin =       1;      // NOTE: 0 and 1 are the RX and TX pins.
const byte OLED_SDA =           2;      // used for the oled display through u8glib
const byte OLED_SCL =           3;      // used for the oled display through u8glib
const byte switchConfigPin =    4;
const byte ledHeadPin =         5;
const byte ledAuxPin =          6;
const byte switchBrakePin =     7;
const byte switchAlarmPin =     8;
const byte ledRightPin =        9;
const byte ledLeftPin =         10;
const byte ledRearPin =         11;
const byte UNUSED3 =            12;
const byte speakerPin =         13;
const byte voltagePin =         A0;

const byte switchRightPin =     A1;
const byte switchLeftPin =      A2;
const byte switchHeadDownPin =  A3;
const byte switchHeadUpPin =    A4;
const byte UNUSED4 =            A5;


/*
   Other constants which are used throughout the programm:
*/
const long tSleep_ms = 300000;              // Timeout until sleep when cadence and speed sensor are active
const long tSleepNoCadSpd_ms = 1800000;     // Timeout until sleep when cadence and speed sensor are NOT active
const int  tPeriodBlink_ms = 333;           // 1.5 Hz Note that it actually is have a period.
const int  tFogFlashHigh_ms = 100;           // 1.5 Hz Note that it actually is have a period.
const int  tFogFlashLow_ms = 4000 - tFogFlashHigh_ms;           // 1.5 Hz Note that it actually is have a period.
const byte numTimesToBlink = VP_BLINK_COUNT;             // 7 times high, 6 times low, = 13 = 4.3 s
const long tDurationBrakeLight_ms = 15000;   // 

// now via VeloPuter_Config.h
/* 
#if defined(STRADA)
const int  wheelCircumference_mm = 1590;    // 406-50 wheel
const int  rearWheelCircumference_mm = 2075; // 559-50 wheel
#elif defined(QUILTJE)
const int  wheelCircumference_mm = 1450;    // 406-28 wheel
const int  rearWheelCircumference_mm = 1990; // 559-35 wheel
#elif defined(QUATRO)
const int  wheelCircumference_mm = 1540;   // 406-40 wheel
const int  rearWheelCircumference_mm = wheelCircumference_mm; // identical
#elif defined(ICB_DF)
const int  wheelCircumference_mm = 1450;    // 406-28 wheel
const int  rearWheelCircumference_mm = 1990; // 559-35 wheel
#endif
 */

const int  wheelCircumference_mm = VP_FRONTWHEEL;
const int  rearWheelCircumference_mm = VP_REARWHEEL;

const float gearOnCassette_scaling = ((float)rearWheelCircumference_mm/(float)wheelCircumference_mm);

const byte speakerVolume = 50;

// not used
// const byte setBrakeMaxTimeOn_s =  15; // max time for a brakelight to be on.

byte setOledIntensity = 0;

// now via VeloPuter_Config.h
/* 
#if defined(ICB_DF)
const byte setTeethOnCainring = 53; // MaartenS: 53
#elif defined(QUATRO)
const byte setTeethOnCainring = 70; //Gert: 70, STefan: 75 MaartenS: 53
#else
const byte setTeethOnCainring = 53; // MaartenS: 53
#endif
 */

const byte setTeethOnCainring = VP_CHAINRING;

//#if defined(QUATRO)
//const float setTeethOnCassette[20] = {11,  13,   15,   17,   19, 22,   25, 28, 32, 36,
//                                     255, 255, 37.5, 42.5, 47.5, 55, 62.5, 70, 80, 90
//                                     }; // Quatro Note that I put two of the gears to 100 time the number to avoid confusion with the normal low gearing (36 etc). S11 and S13 will not be displayed.
// const String setTeethOnCassette_string[20] = {"11", "13", "15", "17", "19", "22", "25", "28", "32", "36",
//                                               "s11", "s13", "s15", "s17", "s19", "s22", "s25", "s28", "s32", "s36"
//                                              };
//#else
//const byte setTeethOnCassette[VP_CASSETTE_LENGHT] = {VP_CASSETTE_VALUES};
//#endif


/*
  Defaullt intensiteiten
*/
#if !defined(VP_DIMMED_INTENSITY)
/* This makes the setting accessible through the python configuration script */
#define VP_DIMMED_INTENSITY 96
#endif

const int leftLedOffIntensity = 0; // Note that currently pwm is disabled for the blinker pins. See of this has anything to do with the failing display/.
const int leftLedLowIntensity = 255;
const int leftLedMediumIntensity = 255;
const int leftLedHighIntensity = 64;
const int leftLedMaxIntensity = 255;

const int rightLedOffIntensity = 0;
const int rightLedLowIntensity = 255;
const int rightLedMediumIntensity = 255;
const int rightLedHighIntensity = 64;
const int rightLedMaxIntensity = 255;

const int rearLedOffIntensity = 8;
const int rearLedLowIntensity = 32;
const int rearLedMediumIntensity = 64;
const int rearLedHighIntensity = 64; // Note that this stops the up/down!
const int rearLedMaxIntensity = 255;

#if defined(QUATRO)
const int headLedOffIntensity = 0;
const int headLedLowIntensity = 32;
const int headLedMediumIntensity = VP_DIMMED_INTENSITY;
const int headLedHighIntensity = 255;
const int headLedMaxIntensity = 255;

const int auxLedOffIntensity = 0; // aux is the brakelight
const int auxLedLowIntensity = 0; // remember that the brakelight comes back into low.
const int auxLedMediumIntensity = 255;
const int auxLedHighIntensity = 255;
const int auxLedMaxIntensity = 255;

#elif defined(ICB_DF)
const int headLedOffIntensity = 0;
const int headLedLowIntensity = 32;
const int headLedMediumIntensity = VP_DIMMED_INTENSITY;
const int headLedHighIntensity = 255;
const int headLedMaxIntensity = 255;

const int auxLedOffIntensity = 0; // aux is not connected in the ICB_DF
const int auxLedLowIntensity = 0;
const int auxLedMediumIntensity = 0;
const int auxLedHighIntensity = 0;
const int auxLedMaxIntensity = 0;

#endif

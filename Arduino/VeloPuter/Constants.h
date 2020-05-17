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
//const byte ledAuxPin =          6;   // brake light in the hood
const byte ledAuxPin =          12;   // brake light in the hood
const byte switchBrakePin =     7;
const byte switchAlarmPin =     8;
const byte ledRightPin =        9;
const byte ledLeftPin =         10;
const byte ledRearPin =         11;
//const byte ledAux2Pin =         12; // wide beam inside headlight
const byte ledAux2Pin =         6; // wide beam inside headlight
const byte speakerPin =         13;
const byte voltagePin =         A0;

const byte switchRightPin =     A1;
const byte switchLeftPin =      A2;
const byte switchHeadDownPin =  A3;
const byte switchHeadUpPin =    A4;
const byte powerOnOffPin =      A5;


/*
   Other constants which are used throughout the programm:
*/
const long tSleep_min = 5;                // Timeout until sleep when cadence and speed sensor are active. In minutes.
const long tSleepNoCadSpd_min = 30;       // Timeout until sleep when cadence and speed sensor are NOT active. In minutes.
const int tDelayBatteryCheck_s = 15;      // How long until the battery management starts. This is the time window after power on in which the battery management van be disabled even when the battery voltage is complety off.
const long tDurationBrakeLight_s = 15;    // The maximun duration the brake light is on for one continuous brake action. 

const int  tPeriodBlink_ms = 333;           // 1.5 Hz Note that it actually is have a period.
const int  tFogFlashHigh_ms = 100;           // 1.5 Hz Note that it actually is have a period.
const int  tFogFlashLow_ms = 4000 - tFogFlashHigh_ms;           // 1.5 Hz Note that it actually is have a period.
const byte numTimesToBlink = VP_BLINK_COUNT;             // 7 times high, 6 times low, = 13 = 4.3 s

const int  wheelCircumference_mm = VP_FRONTWHEEL;
const int  rearWheelCircumference_mm = VP_REARWHEEL;

const float gearOnCassette_scaling = ((float)rearWheelCircumference_mm/(float)wheelCircumference_mm);

const byte speakerVolume = 50;

byte setOledIntensity = 0;

const byte setTeethOnCainring = VP_CHAINRING;

/*
  Defaullt intensities
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
const int headLedOffIntensity = 255;
const int headLedLowIntensity = 255-32;
const int headLedMediumIntensity = 255-VP_DIMMED_INTENSITY;
const int headLedHighIntensity = 255-255;
const int headLedMaxIntensity = 255-  255;

const int aux2LedOffIntensity = 0; // aux is the 2nd headlight
const int aux2LedLowIntensity = 16; // remember that the brakelight comes back into low.Note: the lamp normally has 150 mA current. We up this to 175 (128 of 255 and over two lights)
const int aux2LedMediumIntensity = 48; // be carefull making this number higher
const int aux2LedHighIntensity = 128;
const int aux2LedMaxIntensity = 128;

const int auxLedOffIntensity = 0; // aux2 is the brakelight
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

#include <Arduino.h>
#include <U8glib.h>

#include "VeloPuter_Config.h"

enum {LIPO, LIFEPO4};
const byte setBatteryType = LIPO;

#define Ug82

/* Arduino Micro pin configuration/ capabilities

    See also: https://www.arduino.cc/en/Main/ArduinoBoardMicro

  20 Digital input/outputs
  Intrupts: on 5 lines.
  PWM: on 7 lines
  Analog in: 12 lines

  0           INT     (Serial RX)
  1           INT     (Serial TX)
  2           INT     (TWI SDA)
  3       PWM INT     (TWI SCL)
  4/A6            ANA
  5       PWM
  6/A7    PWM     ANA
  7          INT
  8/A8            ANA
  9/A9    PWM     ANA     // Note that Timer 1 does break the pwm in this pin
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
const byte ledHeadPin =         5;      // PWM
const byte switchBrakePin =     7;
const byte ledAuxPin =          6;      // PWM Aux is configured as a second head light.
const byte switchAlarmPin =     8;
const byte ledRightPin =        9;      // PWM
const byte ledLeftPin =         10;     // PWM
const byte ledRearPin =         11;     // PWM
const byte ledBrakePin =        12;     // On/Off (No intensity variation)
const byte speakerPin =         13;     // PWM
const byte voltagePin =         A0;
const byte switchRightPin =     A1;
const byte switchLeftPin =      A2;
const byte switchHeadDownPin =  A3;
const byte switchHeadUpPin =    A4;
const byte powerOnOffPin =      A5;


/*
   Other constants which are used throughout the programm:
*/
const byte tSleepCadSpd_min = 5;                // Timeout until sleep when cadence and speed sensor are active. In minutes.
const byte tSleepNoCadSpd_min = 30;       // Timeout until sleep when cadence and speed sensor are NOT active. In minutes.
//const int tDelayBatteryCheck_s = 15;      // How long until the battery management starts. This is the time window after power on in which the battery management van be disabled even when the battery voltage is complety off.
const int tDurationBrakeLight_s = 15;    // The maximun duration the brake light is on for one continuous brake action.

//const int  tDelayBatteryStatusChange_s = 15;
const int  tPeriodBlink_ms = 333;            // 1.5 Hz Note that the time listed actually is half a period.
const int  tFogFlashHigh_ms = 100;           // Just a short flash.
const int  tFogFlashLow_ms = 4000 - tFogFlashHigh_ms;           // Once every 4 seconds.
const byte numTimesToBlink = VP_BLINK_COUNT;             // 7 times high, 6 times low, = 13 = 4.3 s

const int  wheelCircumference_mm = VP_FRONTWHEEL;
const int  rearWheelCircumference_mm = VP_REARWHEEL;

const float gearOnCassette_scaling = ((float)rearWheelCircumference_mm / (float)wheelCircumference_mm);

const byte speakerVolume = 50;

const byte setTeethOnCainring = VP_CHAINRING;

/*
  Defaullt intensities
*/
#if !defined(VP_DIMMED_INTENSITY)
/* This makes the setting accessible through the python configuration script */
#define VP_DIMMED_INTENSITY 96
#endif
// NOTE: -1 indicated end of setting or non-used values.
// Note these are the normal intensities.

// 1ste position is reserved for the black level and sleep (ie: turn it all the way off)
// 2nd number is the lowest setting. 0 for more. A bit for the rear lights
// last number is -1 to indicate the end of the array.
// An array cannot be longer than 8 because of the definition in Led.cpp
//

#if defined(QUATRO)
const int leftLedIntensities[8]  = {0, 0, 64,  128, 255,  -1, -1, -1}; // Indicators left
const int rightLedIntensities[8] = {0, 0, 64,  128, 255,  -1, -1, -1}; // Indicators right
const int headLedIntensities[8]  = {0, 0, 32,  96,  255,  -1, -1, -1};  // Recom: {255, 255, 255-32, 255-VP_DIMMED_INTENSITY, 0}; // Head lights
const int auxLedIntensities[8]  =  {0, 0, 32,  96,  255,  -1, -1, -1}; // Doubles as the head light
const int brakeLedIntensities[8] = {0, 0, 64,  128, 255,  -1, -1, -1}; // Brake light in the hood
const int rearLedIntensities[8]  = {0, 8, 32,  64,  128, 255, -1, -1}; // Rear light. Note that it cannot be turned off. 

#elif defined(ICB_DF)
// *TODO: fix lengths
const int leftLedIntensities[6]  = {0, 0, 64, 128, 255, -1}; // Indicators left
const int rightLedIntensities[6] = {0, 0, 64, 128, 255, -1}; // Indicators right
const int brakeLedIntensities[6] = {0, 0, -1}; // Brake light
const int headLedIntensities[6]  = {0, 0, 32, 96,  255, -1};  // Recom: {255, 255, 255-32, 255-VP_DIMMED_INTENSITY, 0}; // Head lights
const int rearLedIntensities[6]  = {0, 8, 32, 64,  255, -1}; // Rear light
const int auxLedIntensities[3]  =  {0, 0, -1}; // Extra unused pin Set to zero
#endif

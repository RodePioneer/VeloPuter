#include <Arduino.h> 
#include <U8glib.h>

/* Arduino Micro pin configuration/ capabilities
 *  
 *  See also: https://www.arduino.cc/en/Main/ArduinoBoardMicro
 
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
 9/A9   PWM     ANA 
10/A10  PWM     ANA 
11      PWM
12/A11          ANA 
13      PWM
A0              ANA 
A1              ANA   
A2              ANA 
A3              ANA 
A4              ANA 
A5              ANA 

 *
 *
 */

const byte switchCadPin =       0; // NOTE: 0 and 1 are the RX and TX pins. 
const byte switchSpdPin =       1; // NOTE: 0 and 1 are the RX and TX pins. 
// const byte OLED_SDA =        2; // used for the oled display through u8glib
// const byte OLED_SCL =        3; // used for the oled display through u8glib
// const byte UNUSED =          4;
const byte ledRightPin =        5;
const byte ledLeftPin =         6;
// const byte UNUSED =          7; // currently unused but in the next gen it will be the brake switch because it has an interupt!
// const byte UNUSED =          8; 
const byte ledRearPin =         9;
const byte ledHeadPin =         10; 
const byte ledHead2Pin =        11; 
// const byte UNUSED =          12; 
const int speakerPin =          13; 
const byte voltagePin =         A0;    
const byte switchHeadUpPin =    A1;    
const byte switchHeadDownPin =  A2;
const byte switchRightPin =     A3;    
const byte switchLeftPin =      A4;  
const byte switchBrakePin =     A5; // the number of the brake switch -> to be open in the next gen.

/*
 * Other constants which are used throughout the programm:
 * 
 */
const long tSleep_ms = 6000;     // 10 min
const int tPeriodBlink_ms = 333;  // 1.5 Hz
const byte numTimesToBlink = 10;  // 10 times high, 9 times low, = 19 * 6.3 s
const int wheelCircumvention_mm = 1590;  // 406-50 wheel



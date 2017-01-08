
/*********************************************************************

  Velomobiel led driver board by Gert Beumer

**********************************************************************

  2016, 2017
  by G.M. Beumer

*/

#include <U8glib.h>
#include "Switch.cpp"
#include "Led.cpp"
#include "Draw_Icons.cpp"
#include "Constants.h"
#include <TimerOne.h>
#include <avr/interrupt.h>
#include <avr/power.h>
#include <avr/sleep.h>

U8GLIB_SH1106_128X64 u8g(U8G_I2C_OPT_NONE);   // The proper contructor for the display currently used.

Led leftLed, rightLed, rearLed, headLed, auxLed;
Switch leftSwitch, rightSwitch, upSwitch, downSwitch, brakeSwitch, speedSwitch, cadenceSwitch;

int batteryPercentage_pct = 99;
float cellVoltage_v = 4.2;
int speed_kmh = 0;
int cadence_rpm = 0;

enum {BATTERY_GREEN, BATTERY_ORANGE, BATTERY_RED};
byte statusBattery = BATTERY_GREEN;
volatile byte statusPowerDown = false;
byte stateAlarmBlinkersOn = false;


/**********************************************************************************************
   The Loop the engine of the system. This is the slow train.
 **********************************************************************************************

  The are all the non-essential functions. It is not critical if they are slow or delayed.

*/
void loop ()
{
  updateBattery(); // Read out and calculate the acutual battery status
  updateSpeed();   // Check the speed based on the interupts which have been.
  updateCadence(); // Check the cadence based on the interupts which have been.
  updateSleep();   // See is we need to powerdown the Arduino
  drawScreen();    // Write all the information to the display.
}

/**********************************************************************************************
   Interupt functions
 **********************************************************************************************

  These are the functions which are done at a fixed interval Note that the processin time of these functions
  MAY NEVER be more that the interupt time. Keep in mind that they themselves are interuped by hardware interupts.

*/
void interruptServiceRoutinePinsAndLEDs ()
{
  if (statusPowerDown == false) // we want to avoid that lights are set to ON when goinig down. 
  {
    interrupts(); // make sure the speed and cadence interupts can go through.
    // Note 1: the blinkers are out due to timing issues and crashes.
    // Note 2: the brake will be coupled to the D7 pin with interupt. Perhaps we will not use the timed interupt then.
    // Note 3: the interupts screw up some of the PWM timers causing the lights to behave weirdly.
    updateHead();
    updateRear();
    updateBlinkers();// Update the blinkers
    noInterrupts(); // end that interupts can go through.
  }
}

/**********************************************************************************************
   Interupt functions: Hardware interupts. The deepest level.
 **********************************************************************************************

  Because interupt handling can only be done by a function and not a class member we have a
  few functions here which make that call.

*/
void interruptServiceRoutineSpeed()
{
  speedSwitch.Interupt();
}

void interruptServiceRoutineCadence()
{
  cadenceSwitch.Interupt();
}
























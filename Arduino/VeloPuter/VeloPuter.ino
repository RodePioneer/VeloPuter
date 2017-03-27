
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
#include <avr/interrupt.h>
#include <avr/power.h>
#include <avr/sleep.h>


U8GLIB_SH1106_128X64 u8g(U8G_I2C_OPT_NONE);   // The proper contructor for the display currently used.

Led leftLed, rightLed, rearLed, headLed, auxLed;
Switch leftSwitch, rightSwitch, upSwitch, downSwitch, brakeSwitch, speedSwitch, cadenceSwitch, alarmSwitch, configSwitch;

int batteryPercentage_pct = 99;
int cellVoltage_mv = 4200;
int speed_kmh = 0;
int cadence_rpm = 0;

// 
long DEBUGTlast = 0;
long DEBUGTnow = 0;

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
  u8g.firstPage();
  do {
    
    updateHead();    // change headlight intensity
    updateRear();    // update rear lights. This includes the brakelight when applicable.
    updateBlinkers();// Update the blinkers

    updateBattery(); // Read out and calculate the acutual battery status
    updateSpeed();   // Check the speed based on the interupts which have been.
    updateCadence(); // Check the cadence based on the interupts which have been.
    updateSleep();   // See is we need to powerdown the Arduino
    updateConfig();  // Updte the config. For now: only the 
    
    drawScreen();    // Write all the information to the display.
  } while ( u8g.nextPage() );
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
























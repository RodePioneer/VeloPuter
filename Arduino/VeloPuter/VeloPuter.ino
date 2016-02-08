/*********************************************************************
*
* Velomobiel led driver board by Gert Beumer
*
**********************************************************************
*
* 2016
* by G.M. Beumer
*
*/

#include <U8glib.h>
#include "Switch.cpp"
#include "Led.cpp"
#include "Draw_Icons.cpp"
#include "State.cpp"
#include "Constants.cpp"

enum { 
  LEFT, RIGHT, OFF, LOW_INTENSITY, DEFAULT_INTENSITY, HIGH_INTENSITY, FOG_INTENSITY, BRAKE_INTENSITY, BLINK_WAIT, BLINK_LEFT, BLINK_RIGHT, BLINK_ALARM  };

//**************************************************
// Change this constructor to match your display!!!
//Adafruit_SSD1306 display(NULL); <= alternative display unit.
U8GLIB_SH1106_128X64 u8g(U8G_I2C_OPT_NONE);   // I2C / TWI ACK <= WORKS
//U8GLIB_SH1106_128X64 u8g(U8G_I2C_OPT_DEV_0|U8G_I2C_OPT_FAST); // Dev 0, Fast I2C / TWI  <= WORKS
//U8GLIB_SH1106_128X64 u8g(U8G_I2C_OPT_NO_ACK); // Display which does not send ACK <= WORKS

//**************************************************

Led leftLed, rightLed, rearLed, headLed, head2Led;
Switch leftSwitch, rightSwitch, upSwitch, downSwitch, brakeSwitch, spdSwitch, cadSwitch;
State rearState, headState, blinkerState, loopState, sleepState;
int batteryStatus = 0;
float batteryVoltage_mv = 0;
int speed_kmh = 0;
int cadence_rpm = 0;

void setup()   { 
  analogReference(DEFAULT);  // use the 5V pin as ref

  // Define the mode of  the pints
  pinMode(ledRearPin, OUTPUT);      
  pinMode(switchBrakePin, INPUT_PULLUP);  

  pinMode(ledHeadPin, OUTPUT);      
  pinMode(switchHeadUpPin, INPUT_PULLUP);  
  pinMode(switchHeadDownPin, INPUT_PULLUP);  

  pinMode(ledLeftPin, OUTPUT);      
  pinMode(switchLeftPin, INPUT_PULLUP);  

  pinMode(ledRightPin, OUTPUT);      
  pinMode(switchRightPin, INPUT_PULLUP);  

  pinMode(voltagePin, INPUT);

  pinMode(switchSpdPin, INPUT_PULLUP);      
  pinMode(switchCadPin, INPUT_PULLUP);  
  attachInterrupt(digitalPinToInterrupt(switchSpdPin), intSpd, FALLING); // 0 = interupt on pin 2
  attachInterrupt(digitalPinToInterrupt(switchCadPin), intCad, FALLING); // 1 = interupt on pin 3
  //attachInterrupt(digitalPinToInterrupt(switchBrakePin), intBrake, FALLING); // 0 = interupt on pin 7 

  pinMode(speakerPin, OUTPUT);      

  // define serial if we want to sent serial information to the serial monitor
  Serial.begin(9600);
  
  u8g.setColorIndex(1); // Instructs the display to draw with a pixel on.
  u8g.setContrast(16);

    drawSplash ();

}

/***********************************************
 * 
 * The state machine: the engine of the system.
 *
 ***********************************************
 */
void loop ()
{
  enum {
    INIT, UPDATE_SWITCHES, GO_TO_SLEEP, SLEEP, WAKE_UP,  UPDATE_FRONT, UPDATE_REAR, UPDATE_BLINKERS, UPDATE_SPD, UPDATE_CAD, UPDATE_DISPLAY, BATTERY, ACTIVE_LOOP       };

  switch (loopState.getState()) {
  case UPDATE_SWITCHES:
    updatePinStates();
    if ( (sleepState.getTimeInState() > tSleep_ms) && (blinkerState.getState() != BLINK_ALARM)) 
    {
      sleepState.setState(true);
    }

    // Check if we are sleeping. If zo -> sleep the loop. 
    if ( sleepState.getState() ) // true = sleep mode started.
    {
      loopState.setState(GO_TO_SLEEP);
    }
    else
    {
      loopState.setState(ACTIVE_LOOP);
    }
    break;

  case ACTIVE_LOOP:
    updateHead();
    updateRear();
    updateBlinkers();
    updateBattery();
    updateSpeed();
    updateCadence();
    writeScreen();
    loopState.setState(UPDATE_SWITCHES);
    break;

  case GO_TO_SLEEP:
    clearScreen ();
    goToSleep();
    loopState.setState(SLEEP);
    break;

  case SLEEP:
    updatePinStates();
    if (sleepState.getState() == false) // sleepstate gets ste in the update pins. 
    {
         loopState.setState(WAKE_UP);
    }
    break;

  case WAKE_UP:
    updatePinStates();
    // continue with Init!
    
  case INIT:
    Init();
    loopState.setState(UPDATE_SWITCHES);
    break;
  }
}

/**********************************************************************************************
 * Init: after return from sleep or at bootup.
 **********************************************************************************************/

void Init () {
  // Connect all the pins.
  leftSwitch.setPin(switchLeftPin);
  rightSwitch.setPin(switchRightPin);
  upSwitch.setPin(switchHeadUpPin);
  downSwitch.setPin(switchHeadDownPin);
  brakeSwitch.setPin(switchBrakePin);

  leftLed.setPin(ledLeftPin);
  rightLed.setPin(ledRightPin);
  leftLed.lowIntensity = 0;
  rightLed.lowIntensity = 0;
  leftLed.setLedLow(); // do this because otherwise the LED would remain on.
  rightLed.setLedLow();

  // reset all the intensities and states. 
  rearLed.setPin(ledRearPin);
  rearLed.lowIntensity = 16; // it might have been set to FOG of off

  headLed.setPin(ledHeadPin);
  headLed.setLedIntensity(16); // set initial intensity of the frontlight
  head2Led.setPin(ledHead2Pin);
  head2Led.setLedIntensity(16);
  
  // Initialise all the in
  headState.setState(DEFAULT_INTENSITY);
  rearState.setState(DEFAULT_INTENSITY);
  blinkerState.setState(DEFAULT_INTENSITY);
  loopState.setState(0);

  drawSplash ();
  
  // TODO: a loop to set the output of the LED pins so that the LDD modules know that there is a PWM signal.
}

/**********************************************************************************************
 * Functions for the sleep loop
 **********************************************************************************************/
void goToSleep()
{
  // turn off all illumination.. Never mind the states: later we will reset them.
  leftLed.setLedIntensity(0);
  rightLed.setLedIntensity(0);
  rearLed.setLedIntensity(0);
  headLed.setLedIntensity(0);
  head2Led.setLedIntensity(0);
  
  // and sleep....
  sleepState.setState(true); 
}

/**********************************************************************************************
 * Interupt functions
 **********************************************************************************************

 Because interupt handling can only be done by a function and not a class member we have a 
 few functions here which make that call. 
 
 */

void intSpd()
{
  spdSwitch.Interupt();
}

void intCad()
{
  cadSwitch.Interupt();
}

void intBrake() 
{
  brakeSwitch.Interupt();
}



























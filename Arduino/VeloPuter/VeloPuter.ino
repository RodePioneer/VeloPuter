/*********************************************************************

  Velomobiel led driver board by Gert Beumer

**********************************************************************

  2016
  by G.M. Beumer

*/

#include <U8glib.h>
#include "Switch.cpp"
#include "Led.cpp"
#include "Draw_Icons.cpp"
#include "State.cpp"
#include "Constants.cpp"
#include <TimerOne.h>

enum {
  LEFT, RIGHT, OFF, LOW_INTENSITY, DEFAULT_INTENSITY, HIGH_INTENSITY, FOG_INTENSITY, BRAKE_INTENSITY, BLINK_WAIT, BLINK_LEFT, BLINK_RIGHT, BLINK_ALARM
};

//**************************************************
// Change this constructor to match your display!!!
//Adafruit_SSD1306 display(NULL); <= alternative display unit.
U8GLIB_SH1106_128X64 u8g(U8G_I2C_OPT_NONE);   // I2C / TWI ACK <= WORKS
//U8GLIB_SH1106_128X64 u8g(U8G_I2C_OPT_DEV_0|U8G_I2C_OPT_FAST); // Dev 0, Fast I2C / TWI  <= WORKS
//U8GLIB_SH1106_128X64 u8g(U8G_I2C_OPT_NO_ACK); // Display which does not send ACK <= WORKS

//**************************************************

Led leftLed, rightLed, rearLed, headLed, head2Led;
Switch leftSwitch, rightSwitch, upSwitch, downSwitch, brakeSwitch, speedSwitch, cadenceSwitch;
State rearState, headState, blinkerState, loopState, sleepState;
int batteryStatus = 0;
float cellVoltage_v = 0;
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
  attachInterrupt(digitalPinToInterrupt(switchSpdPin), interruptServiceRoutineSpeed, FALLING); // 0 = interupt on pin 2
  attachInterrupt(digitalPinToInterrupt(switchCadPin), interruptServiceRoutineCadence, FALLING); // 1 = interupt on pin 3
  //attachInterrupt(digitalPinToInterrupt(switchBrakePin), intBrake, FALLING); // 0 = interupt on pin 7 <= in the future


  // Timer interupt
  Timer1.initialize(tSoftwareTimerInterrupt_us); 
  Timer1.attachInterrupt(interruptServiceRoutinePinsAndLEDs);

  // define serial if we want to sent serial information to the serial monitor
  Serial.begin(9600);

  u8g.setColorIndex(1); // Instructs the display to draw with a pixel on.
  u8g.setContrast(16);

  //drawSplash ();

  Init();


}

/***********************************************

   The state machine: the engine of the system.

 ***********************************************
*/
void loop ()
{
  // String Message = "State: " + String (sleepState.getState()) + "  " + String (sleepState.getTimeInState()) + "  [ms]";
  // Serial.println (Message);
  
  // If we are not asleep
  if (sleepState.getState() == false)
  {
   //Serial.println ("Active in Loop");
   updateBlinkers();// Update the blinkers
   updateBattery(); // Read out and calculate the acutual battery status
   updateSpeed();   // Check the speed based on the interupts which have been.
   updateCadence(); // Check the cadence based on the interupts which have been.
   writeScreen();   // Write all the information to the display.
  }
}

/**********************************************************************************************
   Init: after return from sleep or at bootup.
 **********************************************************************************************/

void Init () {
  // Connect all the pins.
  leftSwitch.setPin(switchLeftPin);
  rightSwitch.setPin(switchRightPin);
  upSwitch.setPin(switchHeadUpPin);
  downSwitch.setPin(switchHeadDownPin);
  brakeSwitch.setPin(switchBrakePin);

  leftLed.setPin(ledLeftPin);
  leftLed.minIntensity = 0;
  leftLed.lowIntensity = 0;
  leftLed.highIntensity = 255;
  leftLed.maxIntensity = 255;

  rightLed.setPin(ledRightPin);
  rightLed.minIntensity = 0;
  rightLed.lowIntensity = 0; 
  rightLed.highIntensity = 255;
  rightLed.maxIntensity = 255;
  
  rearLed.setPin(ledRearPin);
  rearLed.minIntensity = 8;
  rearLed.lowIntensity = 16; 
  rearLed.highIntensity = 64;
  rearLed.maxIntensity = 255;

  headLed.setPin(ledHeadPin);
  headLed.minIntensity = 16;
  headLed.lowIntensity = 64; 
  headLed.highIntensity = 255;
  headLed.maxIntensity = 255; 

  head2Led.setPin(ledHead2Pin);
  head2Led.minIntensity = 16;
  head2Led.lowIntensity = 64; 
  head2Led.highIntensity = 255;
  head2Led.maxIntensity = 255;

  
  // Initialise all the in
  headState.setState(DEFAULT_INTENSITY);
  rearState.setState(DEFAULT_INTENSITY);
  blinkerState.setState(DEFAULT_INTENSITY);
  loopState.setState(1);

  drawSplash ();

  // TODO: a loop to set the output of the LED pins so that the LDD modules know that there is a PWM signal.
}



/**********************************************************************************************
   Interupt functions
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

void interruptServiceRoutinePinsAndLEDs ()
{
  interrupts(); // make sure the speed and cadence interupts can go through.
  updateSleep();
  
  if (sleepState.getState() == false)
  {
    // Note 1: the blinkers are out due to timing issues and crashes. 
    // Note 2: the brake will be coupled to the D7 pin with interupt. Perhaps we will not use the timed interupt then. 
    updateRear();
    updateHead(); 
  }
  noInterrupts(); 
}























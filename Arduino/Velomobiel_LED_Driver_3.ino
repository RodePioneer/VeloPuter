/*********************************************************************
*
* Velomobiel led driver board by Gert Beumer
*
*
 ********************************************************************
*
* 2016-01-12
* by G.M. Beumer
*
*
*
*/

#include <U8glib.h>
#include "Switch.cpp"
#include "Led.cpp"
#include "Draw_Icons.cpp"
#include "State.cpp"
#include "Constants.cpp"


//*include "Display.cpp"

enum { 
  LEFT, RIGHT, OFF, LOW_INTENSITY, DEFAULT_INTENSITY, HIGH_INTENSITY, FOG_INTENSITY, BRAKE_INTENSITY, BLINK_WAIT, BLINK_LEFT, BLINK_RIGHT, BLINK_ALARM  };


//**************************************************
// Change this constructor to match your display!!!
//Adafruit_SSD1306 display(NULL); <= alternative display unit.
U8GLIB_SH1106_128X64 u8g(U8G_I2C_OPT_NONE);   // I2C / TWI ACK <= WORKS
//U8GLIB_SH1106_128X64 u8g(U8G_I2C_OPT_DEV_0|U8G_I2C_OPT_FAST); // Dev 0, Fast I2C / TWI  <= WORKS
//U8GLIB_SH1106_128X64 u8g(U8G_I2C_OPT_NO_ACK); // Display which does not send ACK <= WORKS

//**************************************************

Led leftLed, rightLed, rearLed, headLed;
Switch leftSwitch, rightSwitch, upSwitch, downSwitch, brakeSwitch, spdSwitch, cadSwitch;
State rearState, headState, blinkerState, loopState, sleepState;
int batteryStatus = 0;
byte speed_kmh;
byte cadence_rpm = 80;

void setup()   { 
  analogReference(DEFAULT);  // use the 5V pin as ref

  // Define the mode of  the pints
  pinMode(ledRearPin, OUTPUT);      
  pinMode(switchBrakePin, INPUT_PULLUP);  

  pinMode(ledHeadPin, OUTPUT);      
  pinMode(switchHeadUpPin, INPUT_PULLUP);  
  pinMode(switchHeadDownPin, INPUT_PULLUP);  

  pinMode(leftLed.Pin, OUTPUT);      
  pinMode(switchLeftPin, INPUT_PULLUP);  

  pinMode(ledRightPin, OUTPUT);      
  pinMode(switchRightPin, INPUT_PULLUP);  

  pinMode(voltagePin, INPUT);

  pinMode(switchSpdPin, INPUT_PULLUP);      
  pinMode(switchCadPin, INPUT_PULLUP);  
  attachInterrupt(digitalPinToInterrupt(switchSpdPin), intSpd, FALLING); // 0 = interupt on pin 2
  attachInterrupt(digitalPinToInterrupt(switchCadPin), intCad, FALLING); // 1 = interupt on pin 3
  //attachInterrupt(digitalPinToInterrupt(switchBrakePin), intSpd, FALLING); // 0 = interupt on pin 2 //TEST


  //pinMode(speakerPin, OUTPUT);      

  // define serial if we want to sent serial information to the serial monitor
  Serial.begin(9600);
  
  u8g.setColorIndex(1); // Instructs the display to draw with a pixel on.
  u8g.setContrast(16);
}

/*
The state machine: the engine of the system.
 
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
//              Serial.println("Sleep from the UPDATE_SWITCHES");
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
    
  case INIT:
    Init();
    loopState.setState(UPDATE_SWITCHES);
    break;
  }
}



void Init () {
  // Connect all the pins.
  leftSwitch.Pin = switchLeftPin;
  rightSwitch.Pin = switchRightPin;
  upSwitch.Pin = switchHeadUpPin;
  downSwitch.Pin = switchHeadDownPin;
  brakeSwitch.Pin = switchBrakePin;  

  leftLed.Pin = ledLeftPin;
  rightLed.Pin = ledRightPin;
  leftLed.lowIntensity = 0;
  rightLed.lowIntensity = 0;

  // reset all the intensities and states. 
  rearLed.Pin = ledRearPin;
  rearLed.lowIntensity = 8; // it might have been set to FOG of off

  headLed.Pin = ledHeadPin;
  headLed.setLedIntensity(16); // set initial intensity of the frontlight

  // Initialise all the in
  headState.setState(DEFAULT_INTENSITY);
  rearState.setState(DEFAULT_INTENSITY);
  blinkerState.setState(DEFAULT_INTENSITY);
  loopState.setState(0);

  // TODO: a loop to set the output of the LED pins so that the LDD modules know that there is a PWM signal.
}

void updatePinStates()
{
  /*
  Update all switches. After this all switch statuses can be used. 
   */
  brakeSwitch.ReadOut();
  upSwitch.ReadOut();
  downSwitch.ReadOut();
  leftSwitch.ReadOut();
  rightSwitch.ReadOut();
  spdSwitch.ReadOut();
  cadSwitch.ReadOut();

  // if any of the states changed:
  if (upSwitch.hasStateChanged() ||
    downSwitch.hasStateChanged() || 
    leftSwitch.hasStateChanged() || 
    rightSwitch.hasStateChanged() || 
    brakeSwitch.hasStateChanged() || 
    spdSwitch.hasStateChanged() || 
    cadSwitch.hasStateChanged() 
  )
  {
      //    Serial.println("Going to WAKE UP sleepstate");

    sleepState.setState(true);  // set it to sleep first in order to make sure we have a toggle.
    sleepState.setState(false); // sleep to false == wakeup or stay awake.
  }
  
}

/**********************************************************************************************
 * Functions for the sleep loop
 **********************************************************************************************/
void goToSleep()
{
  // turn off all illumination.. Never mind the states: later we will reset them.
  leftLed.setLedIntensity(0);
  rightLed.setLedIntensity(0);
  rearLed.setLedIntensity(0); // set nitial intensity of the rearlight
  headLed.setLedIntensity(0); // set initial intensity of the frontlight

 //     Serial.println("Going to sleep sleepstate");
  sleepState.setState(true); // set sleep state to sleep....
}

/**********************************************************************************************
 *
 *
 * Functions for the active loop
 *
 *
 **********************************************************************************************/

/*
TODO: now this to the min, low, high, max values.
 */
void updateHead() 
{
  static int currentState = rearState.getState();

  if (upSwitch.getState() == LOW && upSwitch.hasStateChanged() && brakeSwitch.getState() == HIGH) 
  {
    switch (headState.getState()) {
    case OFF:
      headState.setState(LOW_INTENSITY);
      break;  
    case LOW_INTENSITY:
      headState.setState(DEFAULT_INTENSITY);
      break;
    case DEFAULT_INTENSITY:
      headState.setState(HIGH_INTENSITY);
      break;
    }
  }

  if (downSwitch.getState() == LOW && downSwitch.hasStateChanged() && brakeSwitch.getState() == HIGH) 
  {
    switch (headState.getState()) {
    case LOW_INTENSITY:
      headState.setState(OFF);
      break;  
    case DEFAULT_INTENSITY:
      headState.setState(LOW_INTENSITY);
      break;
    case HIGH_INTENSITY:
      headState.setState(DEFAULT_INTENSITY);
      break;
    }
  }

  // update intensities.
  switch (headState.getState()) {
  case OFF:
    headLed.setLedIntensity (0);
    break;  
  case LOW_INTENSITY:
    headLed.setLedIntensity (8);
    break;
  case DEFAULT_INTENSITY:
    headLed.setLedIntensity (32); // set new rearlight low intensity
    break;
  case HIGH_INTENSITY:
    headLed.setLedIntensity (255); // set new rearlight low intensity
    break;
  }
} 

void updateBlinkers() 
{
  //
  // What can happen with the blinker:
  // - it is off
  // - it blinks a number of times
  // - it blinks untill it is released. 
  // 
  //  enum { 
  //    BLINK_WAIT, BLINK_LEFT, BLINK_RIGHT, BLINK_ALARM     };
  static long tStartBlink_ms = 0;
  static byte numTimesBlinked = 0; 
  long tNow_ms = millis();

  /*
 
   Depending on the state of the switches the state of the blinker may change. 
   
   */
  if ((leftSwitch.getState() == LOW || rightSwitch.getState() == LOW) && \
              (upSwitch.getState() == LOW || downSwitch.getState() == LOW) && \
               brakeSwitch.getState() == LOW &&
    blinkerState.getState() != BLINK_ALARM)
  {
    blinkerState.setState(BLINK_ALARM);
    tStartBlink_ms = tNow_ms;
    rightLed.setLedHigh();
    leftLed.setLedHigh();
  } 
  else if (rightSwitch.getState() == LOW && rightSwitch.hasStateChanged()) // the exit transision of the state.
  {
    //      Serial.println("R: Leaving WAIT");
    blinkerState.setState(BLINK_RIGHT);  // define the next state
    tStartBlink_ms = tNow_ms; // reset the counters
    numTimesBlinked = 0; 
    rightLed.setLedHigh(); // Turn on the blinker
    leftLed.setLedLow(); // Turn on the blinker
  } 
  else if (leftSwitch.getState() == LOW && leftSwitch.hasStateChanged()) // the exit transision of the state.
  {
    //      Serial.println("L: Leaving WAIT");
    blinkerState.setState(BLINK_LEFT);  // define the next state
    tStartBlink_ms = tNow_ms; // reset the counters
    numTimesBlinked = 0; 
    rightLed.setLedLow(); // Turn on the blinker
    leftLed.setLedHigh(); // Turn on the blinker
  } 
  // exit condition for this blinking states.
  else if (rightSwitch.getState() == HIGH && leftSwitch.getState() == HIGH && 
    ((numTimesBlinked > (2 * numTimesToBlink) && blinkerState.getState() != BLINK_ALARM) ||
    blinkerState.getState() == BLINK_ALARM)
    )
  {
    //      Serial.println("R: Leaving BLINK");
    blinkerState.setState(BLINK_WAIT);
    rightLed.setLedLow();
    leftLed.setLedLow();
 //   display.invertDisplay(false);
  }
  /*
 
   See if we would need to blink. 
   
   */
  if ((tNow_ms - tStartBlink_ms) >= tPeriodBlink_ms)
  {
    tStartBlink_ms = tNow_ms;

    switch (blinkerState.getState()) {
    case BLINK_LEFT:
      leftLed.toggleledIntensity();
      rightLed.setLedLow();
//      display.invertDisplay(numTimesBlinked++ % 2);
      blinkScreen(numTimesBlinked++ % 2);
      break;

    case BLINK_RIGHT:
      // Do we need to toggle the blinker?
      leftLed.setLedLow();
      rightLed.toggleledIntensity();
//      display.invertDisplay(numTimesBlinked++ % 2);
      blinkScreen(numTimesBlinked++ % 2);
      break;

    case BLINK_ALARM:
      leftLed.toggleledIntensity();
      rightLed.toggleledIntensity();
//      display.invertDisplay(numTimesBlinked++ % 2);
      blinkScreen(numTimesBlinked++ % 2);
      break;
    }
  }
}



void updateRear() 
{
  /*
  First we determine how the state changed
   Second we adapt the led settings.
   */

  // Determine the next state
  static byte nonBrakeState = DEFAULT_INTENSITY;
  // brake is on but no up/down switch is active
  if (brakeSwitch.getState() == LOW && brakeSwitch.hasStateChanged() && upSwitch.getState() == HIGH && downSwitch.getState() == HIGH) 
  {
    rearState.setState(BRAKE_INTENSITY);
  } 
  // brake switch is released
  else if (brakeSwitch.getState() == HIGH && brakeSwitch.hasStateChanged())
  {
    rearState.setState(nonBrakeState);
  }
  // brakeswitch is on and we have a up-switch activity: 
  else if (brakeSwitch.getState() == LOW && upSwitch.getState() == LOW && upSwitch.hasStateChanged())
  {
    if (nonBrakeState == LOW_INTENSITY) 
    {
      nonBrakeState = DEFAULT_INTENSITY;
    }
    else if (nonBrakeState == DEFAULT_INTENSITY)     
    {
      nonBrakeState = FOG_INTENSITY;
    }
    rearState.setState(nonBrakeState);
  }
  else if (brakeSwitch.getState() == LOW && downSwitch.getState() == LOW && downSwitch.hasStateChanged())
  {
    if (nonBrakeState == FOG_INTENSITY) 
    {
      nonBrakeState = DEFAULT_INTENSITY;
    }
    else if (nonBrakeState == DEFAULT_INTENSITY) 
    {
      nonBrakeState = LOW_INTENSITY;
    }
    rearState.setState(nonBrakeState);  
  }

  // update intensities according to the set state.
  switch (rearState.getState()) {
  case OFF:
    rearLed.setLedIntensity (0);
    break;  
  case LOW_INTENSITY:
    rearLed.setLedIntensity (8);
    break;
  case DEFAULT_INTENSITY:
    rearLed.setLedIntensity (16); // set new rearlight low intensity
    break;
  case FOG_INTENSITY:
    rearLed.setLedIntensity (64); // set new rearlight low intensity
    break;
  case BRAKE_INTENSITY:
    rearLed.setLedIntensity (255); // set new rearlight low intensity
    break;
  }

}



void updateBattery() 
{
/* 
Reads the value from the specified analog pin. The Arduino board contains a 6 
channel (8 channels on the Mini and Nano, 16 on the Mega), 10-bit analog to 
digital converter. This means that it will map input voltages between 0 and 5 
volts into integer values between 0 and 1023. This yields a resolution between 
readings of: 5 volts / 1024 units or, .0049 volts (4.9 mV) per unit. The input 
range and resolution can be changed using analogReference().
*/
  
  int PinValue = analogRead(voltagePin);

  const byte numSamples = 10;
  const float VRef = 5.06;     
  const float R1 = 1.0;
  const float R2 = 3.3;
  const float VbattFull = 11.2;

  static float PinMean = 0;

  PinMean = (PinMean*(numSamples - 1) + PinValue)/numSamples;
  
  float Vbatt = 4.326 * (VRef/1023) * PinMean; // 4.326 was measured useing a voltage meter
    
  batteryStatus = 100 * Vbatt;
  
  //batteryStatus = Vin;

  //Serial.println(VbattNow);
  //Serial.println(PinMean);
  //Serial.println("---");

  
}

void updateSpeed() 
{
  // Just loop for now. Later we will make stuff better.  
  // speed_kmh++;
  
  if (speed_kmh >= 120)
  {
    speed_kmh = 0;
  }
}

void updateCadence() 
{
  // Just loop for now. Later we will make stuff better.  
  //cadence_rpm++;
  
  if (cadence_rpm >= 120)
  {
    cadence_rpm = 80;
  }
}

void intSpd()
{
  spdSwitch.Interupt();
}

void intCad()
{
  cadSwitch.Interupt();
}





























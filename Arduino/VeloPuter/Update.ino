
/**********************************************************************************************
 *
 * Functions which update the states and .
 *
 **********************************************************************************************/
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
  
  spdSwitch.ReadOut(); // the interupts still have to be "ReadOut" because an interupt is only registered. Not handled any further.
  cadSwitch.ReadOut();
}

void updateSleep ()
{
  //Serial.println("Just checking...");

  // if any of the states changed: make sure we leave the sleep state.

//  byte switchChanged  = (upSwitch.hasStateChanged() ||
//                        downSwitch.hasStateChanged() || 
//                        leftSwitch.hasStateChanged() || 
//                        rightSwitch.hasStateChanged() || 
//                        brakeSwitch.hasStatehanged() || 
//                        spdSwitch.hasStateChanged() || 
//                        cadSwitch.hasStateChanged()) ; 
//  byte alarmLightsAreOn = (blinkerState.getState() != BLINK_ALARM);
//  byte 
//    
//  if (  )
//  {
//    Serial.println("A switch has toggled. Set slaap state to awake (false)"); 
//    sleepState.setState(true);  // set it to sleep first in order to make sure we have a toggle.
//    sleepState.setState(false); // sleep to false == wakeup or stay awake.
//  }
//
//    if ( (sleepState.getTimeInState() > tSleep_ms) && (blinkerState.getState() != BLINK_ALARM))
//  {
//    Serial.println ("sleepState.setState(true);");
//    clearScreen ();
//    goToSleep();
//    sleepState.setState(true);
//  }
  
  
  
  //Serial.println ("updateSleep() 2");
    //
    // If we are asleep and any pin changes: wake up.
    //
    byte switchChanged  = (upSwitch.hasStateChanged() ||
                          downSwitch.hasStateChanged() || 
                          leftSwitch.hasStateChanged() || 
                          rightSwitch.hasStateChanged() || 
                          brakeSwitch.hasStateChanged() || 
                          spdSwitch.hasStateChanged() || 
                          cadSwitch.hasStateChanged()) ;
    if (switchChanged)
    {
      Serial.println ("switchChanged: stay awake or wake up");
      sleepState.setState(true);  // A switch changed. Toggle the state so make sure the time in the state is now reset.  
      sleepState.setState(false); 
    }

  // if we have not has a switch active for a certain time and do not have the alarm lights on: go to sleep
  // and are not already in sleep state.
  if ((sleepState.getTimeInState() > tSleep_ms) && 
      (blinkerState.getState() != BLINK_ALARM) && 
      (sleepState.getState() == false)) 
  {
      Serial.println ("goToSleep () ");
      // and sleep....
      sleepState.setState(true);

      // switch off all lichts.
      leftLed.setLedOff();
      rightLed.setLedOff();
      rearLed.setLedOff();
      headLed.setLedOff();
      head2Led.setLedOff();
    
      // clear the screen
      clearScreen ();    
      Serial.println ("Done ");

  }
}


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
    head2Led.setLedIntensity (0);
    break;  
  case LOW_INTENSITY:
    headLed.setLedIntensity (16);
    head2Led.setLedIntensity (16);
    break;
  case DEFAULT_INTENSITY:
    headLed.setLedIntensity (64); 
    head2Led.setLedIntensity (64); 
    break;
  case HIGH_INTENSITY:
    headLed.setLedIntensity (255); 
    head2Led.setLedIntensity (255); 
    break;
  }
} 

void updateBlinkers() 
{
  //
  // What can happen with the blinker:
  // - it is off (initial state)
  // - it blinks a number of times
  // - it blinks untill it is released. 

  static long tStartBlink_ms = 0;
  static byte numTimesBlinked = 1; 
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
    blinkerState.setState(BLINK_WAIT);
    rightLed.setLedLow();
    leftLed.setLedLow();
    analogWrite(speakerPin, 0);

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
      blinkScreen(numTimesBlinked++ % 2);
      if (numTimesBlinked % 2)
      {
        analogWrite(speakerPin, 15);
      }
      else
      {
        analogWrite(speakerPin, 0);
      }
      break;

    case BLINK_RIGHT:
      // Do we need to toggle the blinker?
      leftLed.setLedLow();
      rightLed.toggleledIntensity();
      blinkScreen(numTimesBlinked++ % 2);
      if (numTimesBlinked % 2)
      {
        analogWrite(speakerPin, 15);
      }
      else
      {
        analogWrite(speakerPin, 0);
      }
      break;

    case BLINK_ALARM:
      leftLed.toggleledIntensity();
      rightLed.toggleledIntensity();
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

Use a cumulative moving average to keep track of the voltage. Note that this implementation
of a CMA means that the influence of every measurement decays exponentially. 
*/
  
  int PinValue = analogRead(voltagePin);

  const byte numSamples = 10;
  const float VRef = 5.00;     
  const float R1 = 1.0;
  const float R2 = 3.3;
  const float VcellFull = 3.9; // Assume that the cell is mostly between 3.6 and 3.9 V in most of it's operational phase.
  const float VcellEmpty = 3.6;
  float batteryVoltage_v;
  byte numOfCells;


  static float PinMean = VcellEmpty;

  PinMean = (PinMean*(numSamples - 1) + PinValue)/numSamples; // the mean voltage on the pin. 
  
  batteryVoltage_v = 4.326 * (VRef/1023) * PinMean; // 4.326 was measured useing a voltage meter
  
  // Assume the cells are no less then 3.2 V and no more 4.2V.
  // cutoffs for 3 cells between 9.0 and 12.8 V. 
  if (batteryVoltage_v < 9.0) numOfCells = 2;
  else if (batteryVoltage_v < 12.8) numOfCells = 3;
  else numOfCells = 4;

  cellVoltage_v = batteryVoltage_v / numOfCells;
    
  batteryStatus = 100 * (cellVoltage_v - VcellEmpty)/(VcellFull - VcellEmpty);

}

void updateSpeed() 
{
  // Calculate the speed from the frequency
  speed_kmh = 3.6 * wheelCircumvention_mm * spdSwitch.getInteruptFrequency(1250) / 1000;
  //String Message = " kmh = " + String(speed_kmh ) + " / " + String (spdSwitch.getInteruptFrequency(1250)) ;
  //Serial.println(Message);
}

void updateCadence() 
{
  // Calculate the cadence from the frequency
  cadence_rpm = 60 * cadSwitch.getInteruptFrequency(2500);
  //String Message = " Cad = " + String(cadence_rpm ) + " / " + String (cadSwitch.getInteruptFrequency(2500)) ;
  //Serial.println(Message);
}

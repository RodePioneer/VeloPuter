
/**********************************************************************************************

   Functions which update the states and .

 **********************************************************************************************/


/*********************************************************************************************************

    Update the headlights

 *********************************************************************************************************/
void updateHead()
{
  //static int currentState = rearState.getState();
  upSwitch.ReadOut();
  downSwitch.ReadOut();

  if (upSwitch.getState() == LOW && upSwitch.hasStateChanged() && brakeSwitch.getState() == HIGH && configSwitch.getState() == HIGH)
  {
    headLed.upLed();
    aux2Led.upLed();
  }

  if (downSwitch.getState() == LOW && downSwitch.hasStateChanged() && brakeSwitch.getState() == HIGH && configSwitch.getState() == HIGH)
  {
    headLed.downLed();
    aux2Led.downLed();
  }
}

/*********************************************************************************************************

    Update the blinkers

 *********************************************************************************************************/
void updateBlinkers()
{
  /*
     A few things can happen:
     - Do not blink
     - Blink right
     - Blink left
     - Blink both (ALARM lights)

  */

  static byte numTimesToBlinkLeft = 0;
  static byte numTimesToBlinkRight = 0;
  static long tStartBlink_ms = 0;
  long tNow_ms = millis();
  byte BlinkOn = 0;

  leftSwitch.ReadOut();
  rightSwitch.ReadOut();
  alarmSwitch.ReadOut();

  //#if defined(QUATRO) || defined(STRADA)
  if (alarmSwitch.hasStateChanged() && alarmSwitch.getState() == LOW )
  {
    stateAlarmBlinkersOn = !stateAlarmBlinkersOn;

    if (stateAlarmBlinkersOn == false)
    {
      numTimesToBlinkLeft = 0;
      numTimesToBlinkRight = 0;
      // Just in case.... otherwise they might stay on.
      rightLed.setLedOff();
      leftLed.setLedOff();
    }
  }
  //#endif

  if (stateAlarmBlinkersOn)
  { // We are blinking alarm lights.
    numTimesToBlinkLeft = 1;
    numTimesToBlinkRight = 1;
  }
  else
  { // regular stuff to do.
    if (rightSwitch.getState() == LOW )
    { // blink right
      if (rightSwitch.hasStateChanged())
      {
        numTimesToBlinkRight = numTimesToBlink * 2;
        tStartBlink_ms = 0;// reset the timer in case we switched quickely from left to right
      }
      else
      {
        numTimesToBlinkRight = max(numTimesToBlinkRight, 1);
      }
      numTimesToBlinkLeft = 0;

    }
    else if (leftSwitch.getState() == LOW )
    { // blink left
      if (leftSwitch.hasStateChanged())
      {
        numTimesToBlinkLeft = numTimesToBlink * 2;
        tStartBlink_ms = 0; // reset the timer in case we switched quickely from left to right
      }
      else
      {
        numTimesToBlinkLeft = max(numTimesToBlinkLeft, 1);
      }
      numTimesToBlinkRight = 0;
    }
  }

  /*
     Do the actual blinking
  */

  //
  // See if enough time has passed since the last change of the leds.
  //
  if ((tNow_ms - tStartBlink_ms) >= tPeriodBlink_ms)
  {

    // Zoomer ands screen blinking in sync with the actual leds. Don't keep track.
    BlinkOn = (rightLed.getLedIntensity() > 0 || leftLed.getLedIntensity() > 0);

    tStartBlink_ms = tNow_ms;

    // blink left
    if (numTimesToBlinkLeft > 0 && numTimesToBlinkRight == 0)
    {
      numTimesToBlinkLeft--;

      if (leftLed.getLedIntensity() == leftLed.offIntensity) leftLed.setLedMax();
      else leftLed.setLedOff();

      rightLed.setLedOff();


    }
    // blink right
    else if (numTimesToBlinkRight > 0 && numTimesToBlinkLeft == 0)
    {
      numTimesToBlinkRight--;

      if (rightLed.getLedIntensity() == rightLed.offIntensity) rightLed.setLedMax();
      else rightLed.setLedOff();

      leftLed.setLedOff();
    }
    // blink alarm
    else if (numTimesToBlinkRight > 0 && numTimesToBlinkLeft > 0)
    { // alarm, make sure they are in sync
      if (rightLed.getLedIntensity() == rightLed.offIntensity)
      {
        rightLed.setLedMax();
        leftLed.setLedMax();
      }
      else
      {
        rightLed.setLedOff();
        leftLed.setLedOff();
      }
    }
    else if (numTimesToBlinkRight == 0 && numTimesToBlinkLeft == 0 && BlinkOn)
    {
      // Apearently the leds are still on while the blinker switch is in the off setting and we no longer need to do anything
      rightLed.setLedOff();
      leftLed.setLedOff();
    }


    //
    // blink the screen
    //
    BlinkOn = (rightLed.getLedIntensity() > 0 || leftLed.getLedIntensity() > 0);

    if (BlinkOn)
    {
      u8g.begin(); // Make sure the display gets reset when we blink
      u8g.setContrast(setOledIntensity);
      u8g.sleepOn();
    }
    else u8g.sleepOff();

    // 
    // zoomer
    //
    if (stateAlarmBlinkersOn == false)
    {
      analogWrite(speakerPin, BlinkOn * speakerVolume);
    }
    else
    {
      // set to zero during Alarm.
      analogWrite(speakerPin, 0);
    }
  }

}

/*********************************************************************************************************

    Update the rearlight
 
 **********************************************************************************************************/
void updateRear()
{
  /*
    The rear lights are normally at a nominal intensity.
    There are four intensities at which the rear lights are run:
    1) low intensity when riding convoy.
    2) default intensity
    3) higher intensity when riding in fog or other situations in which higer visibility is needed.
    4) brakelight: max intensity  when brakeing. Note that this is independant of the brakelight itself.
  
  */

  /*
    First we determine how the state changed
    Second we adapt the led settings.

    Note that we keep track of the return state when the brake is released here here.
  */
  static byte ledPreviousIntensity = rearLedOffIntensity;
  long tNow_ms = millis();
  int setNotMoving = (speed_kmh == 0 && cadence_rpm == 0 && cadenceSwitch.getInteruptActive() && speedSwitch.getInteruptActive());

  brakeSwitch.ReadOut();

  //
  // only brakeswitch is on, and changed and we are moving -> turn on brake light to Max.
  //
  if (brakeSwitch.getState() == LOW 
      && brakeSwitch.hasStateChanged() 
      && upSwitch.getState() == HIGH 
      && downSwitch.getState() == HIGH)
  {
    if (not setNotMoving)
    {
      ledPreviousIntensity = rearLed.getLedIntensity();
      rearLed.setLedMax();
      auxLed.setLedMax();
    }
  }
  //
  // When the brakeswitch is applied too long switch off the breaklights.
  //
  else if (brakeSwitch.getState() == LOW 
           && !brakeSwitch.hasStateChanged() 
           && tNow_ms - brakeSwitch.getTimeLastChange_ms() > tDurationBrakeLight_ms 
           && rearLed.getLedIntensity() == rearLed.maxIntensity)
  {
      rearLed.setLedIntensity(ledPreviousIntensity);
      auxLed.setLedOff();
  }
  //
  // brakeswitch is off -> return to previous state. Only when the brakeled is max.
  //
  else if (brakeSwitch.getState() == HIGH 
           && brakeSwitch.hasStateChanged() 
           && rearLed.getLedIntensity() == rearLed.maxIntensity)
  {
    rearLed.setLedIntensity(ledPreviousIntensity);
    auxLed.setLedOff();
  }

  //
  // Brakeswitch is on, upswitch has changed- > default goes up as fog light
  //
  else if (brakeSwitch.getState() == LOW 
           && upSwitch.getState() == LOW 
           && upSwitch.hasStateChanged())
  {
    rearLed.setLedIntensity(ledPreviousIntensity);
    rearLed.upLed();
    ledPreviousIntensity = rearLed.getLedIntensity();
    auxLed.setLedOff();
  }

  //
  // Brakeswicht is on, downswitch has changed -> turn off fog light or go to dim
  //
  else if (brakeSwitch.getState() == LOW 
           && downSwitch.getState() == LOW 
           && downSwitch.hasStateChanged())
  {
    rearLed.setLedIntensity(ledPreviousIntensity);
    rearLed.downLed();
    ledPreviousIntensity = rearLed.getLedIntensity();
    auxLed.setLedOff();
  }


  //
  // When standing still AND not pedaling, turn it off. Note this only works when both speed and cadende are working
  //
  if (setNotMoving
      && brakeSwitch.getState() == LOW)
    {
      rearLed.setLedIntensity(ledPreviousIntensity);
      auxLed.setLedOff();
    }

  //
  // When riding with fog lights (ie rear set to rearLedMediumIntensity we flash the rear light once every N seconds. 
  // Do not do this when the brake is applied. Only for the QuatroVelo, DF & other three-wheelers don't have aux light.
  //
#if defined(QUATRO)
  if (rearLed.getLedIntensity() == rearLedMediumIntensity 
      && brakeSwitch.getState() == HIGH)
  {
      // When high, set to off
      if (auxLed.getLedIntensity() == auxLedMaxIntensity 
          // Go high for 1/4th of the blink frequency duration
          && (tNow_ms - auxLed.getTimeLastChange_ms()) >= tFogFlashHigh_ms)
          {
              auxLed.setLedOff();
          }
      // when off set to high
      else if (auxLed.getLedIntensity() == auxLedOffIntensity 
          // Go low for 10 times the blink duration
          && (tNow_ms - auxLed.getTimeLastChange_ms()) >= tFogFlashLow_ms)
          {
              auxLed.setLedMax();
          }

  }
#endif
#if defined(ICB_DF)
if (rearLed.getLedIntensity() == rearLedMediumIntensity 
      && brakeSwitch.getState() == HIGH)
  {
    if (   (rearLed.getFlashOnStatus() && (tNow_ms - rearLed.getTimeLastChange_ms()) >= tFogFlashHigh_ms) 
        || (!rearLed.getFlashOnStatus() && (tNow_ms - rearLed.getTimeLastChange_ms()) >= tFogFlashLow_ms))
    {
        rearLed.toggleFlashLed();
    }
  }
#endif
  
}

/*********************************************************************************************************

    The configuration interface

 *********************************************************************************************************/
void updateConfig()
{
  configSwitch.ReadOut();

  // See of there is also a up or down switch active: disable or enable the battery check.
  if ( configSwitch.getState() == LOW && downSwitch.getState() == LOW)
  {
    doBatteryCheck = false;
  }
  else if (configSwitch.getState() == LOW && upSwitch.getState() == LOW )
  {
    doBatteryCheck = true;
  }
  else if (configSwitch.hasStateChanged() && configSwitch.getState() == LOW )
  {
    if (setOledIntensity == 255)
    {
      setOledIntensity = 0;
    }
    else
    {
      setOledIntensity = 255;
    }
    u8g.begin();
    u8g.setContrast(setOledIntensity);
  }
}

/*********************************************************************************************************

    Calculate the speed

 *********************************************************************************************************/
void updateSpeed()
{
  /*
    Check the timestamps on the interupts.
    then from that calculate the speed in km/h.
  */
  speedSwitch.ReadOut();
  speed_kmh = byte(round(0.0036 * wheelCircumference_mm * speedSwitch.getInteruptFrequency(1250)));

}

/*********************************************************************************************************

    Calculate the cadence

 *********************************************************************************************************/
void updateCadence()
{
  /*
    Check the timestamps on the interupts.
    then from that calculate the cadens in RPM.
  */
  cadenceSwitch.ReadOut();
  cadence_rpm = byte(round(60 * cadenceSwitch.getInteruptFrequency(2500)));
}

/*********************************************************************************************************

    Calculate the cadence

 *********************************************************************************************************/
void updateGear()
{
  /*
    Teeth Rear =  Teeth chain ring * Cadence (Hz) /  axle Speed (Hz)
    axle speed = front axle speed * (front wheel circumference / read wheel circumference)
  */
  float gearOnCassette = float(setTeethOnCainring) * gearOnCassette_scaling * cadenceSwitch.getInteruptFrequency(1500) / speedSwitch.getInteruptFrequency(1500);

  gearOnCassette_teeth = round(gearOnCassette);

}

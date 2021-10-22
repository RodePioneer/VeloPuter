
/*********************************************************************************************************

    Update the Indicators

 *********************************************************************************************************/
void updateIndicators()
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


  if (alarmSwitch.hasStateChanged() && alarmSwitch.getState() == LOW )
  {
    //
    // toggle the alarm state because the alarm button was hit.
    //
    stateAlarmBlinkersOn = !stateAlarmBlinkersOn;
    // Reset the blinker counters
    numTimesToBlinkLeft = 0;
    numTimesToBlinkRight = 0;
    // Just in case.... otherwise they might stay on or the hazzars can blink out of sync
    rightLed.setLedOff();
    leftLed.setLedOff();
  }

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
    BlinkOn = (rightLed.getICurrentIntensity() > 1 || leftLed.getICurrentIntensity() > 1);
    tStartBlink_ms = tNow_ms;

    //
    // blink left
    //
    if (numTimesToBlinkLeft > 0 && numTimesToBlinkRight == 0)
    {
      numTimesToBlinkLeft--;
      leftLed.setLedToggleMax();
      rightLed.setLedOff();
    }
    //
    // blink right
    //
    else if (numTimesToBlinkRight > 0 && numTimesToBlinkLeft == 0)
    {
      numTimesToBlinkRight--;
      rightLed.setLedToggleMax();
      leftLed.setLedOff();
    }
    //
    // Hazzard lights: blink with all indicators.
    //
    else if (numTimesToBlinkRight > 0 && numTimesToBlinkLeft > 0)
    {
      // Make sure they are in sync
      rightLed.setLedToggleMax();
      leftLed.setLedToggleMax();
    }
    else if (numTimesToBlinkRight == 0 && numTimesToBlinkLeft == 0 && BlinkOn)
    {
      //
      // Apearently the leds are still on while the blinker switch is in the off setting and we no longer need to do anything
      //
      rightLed.setLedOff();
      leftLed.setLedOff();
    }


    //
    // blink the screen
    //
    BlinkOn = (rightLed.getICurrentIntensity() > 1  || leftLed.getICurrentIntensity() > 1);

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

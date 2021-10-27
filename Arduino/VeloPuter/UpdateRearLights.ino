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

  */

  /*
    Get the state of affairs
  */
  bool setIsBrakeSwitchOn      = brakeSwitch.getState() == LOW;
  bool setIsBrakeSwitchChanged = brakeSwitch.hasStateChanged();
  bool setIsBrakeSwitchTimerExpired = (millis() - brakeSwitch.getTimeLastChange_ms()) > (1000 * tDurationBrakeLight_s);
  bool setIsUpSwitchOnChanged   = upSwitch.getState() == LOW && upSwitch.hasStateChanged();
  bool setIsDownSwitchOnChanged = downSwitch.getState() == LOW && downSwitch.hasStateChanged();
  bool setIsMoving = not (speed_kmh == 0 && cadence_rpm == 0 && cadenceSwitch.getInteruptActive() && speedSwitch.getInteruptActive());   //(speed_kmh != 0 || cadence_rpm != 0) && cadenceSwitch.getInteruptActive() && speedSwitch.getInteruptActive();
  bool setIsBrakeLedOn = brakeLed.getICurrentIntensity() == brakeLed.IMax();

  /*
    Determine what to do
  */
  bool setDoRearOff = false;
  bool setDoRearMax = false;
  bool setDoRearUp = false;
  bool setDoRearDown = false;

  // Todo: why no brake after sleep?
  // Todo: clode clean
  // TODO: make sure it is faster. Skip unnececary loops.

  if ((not setIsBrakeSwitchOn) and setIsBrakeLedOn)
  {
    setDoRearOff = true;
  }
  else if (setIsBrakeSwitchOn and setIsBrakeSwitchTimerExpired and setIsBrakeLedOn)
  {
    setDoRearOff = true;
  }
  else if (setIsBrakeSwitchOn and setIsBrakeSwitchChanged and setIsMoving)
  {
    setDoRearMax = true;
  }
  else if (setIsBrakeSwitchOn and setIsUpSwitchOnChanged)
  {
    setDoRearUp = true;
  }
  else if (setIsBrakeSwitchOn and setIsDownSwitchOnChanged)
  {
    setDoRearDown = true;
  }
  // else: do nothing.....

  /*
    Update the leds
  */
  if (setDoRearMax)
  {
    // Set both LEDs to max but make sure we can set the rear light back to its current value after the brake is released.
    brakeLed.setLedMax();
    rearLed.setLedStoreCurrentIntensityAsPrevious ();
    rearLed.setLedMax();
  }
  else if (setDoRearOff)
  {
    // Brake is off. Back to normal.
    brakeLed.setLedOff();
    rearLed.setLedToPreviousIntensity  ();
  }
  else if (setDoRearUp)
  {
    // Increase the intensity at the back.
    brakeLed.setLedOff();
    // Note we do never go up to the max intensity because we need a difference with the brake light (max is reserved for braking)
    rearLed.setLedToPreviousIntensity  ();
    if (rearLed.getICurrentIntensity() < (rearLed.IMax() - 2))
    {
      rearLed.upLed();
      rearLed.setLedStoreCurrentIntensityAsPrevious (); // TODO: why is this needed??
    }
  }
  else if (setDoRearDown)
  {
    // Decrease the intensity at the back
    brakeLed.setLedOff();
    rearLed.setLedToPreviousIntensity ();
    rearLed.downLed();
    rearLed.setLedStoreCurrentIntensityAsPrevious ();// TODO: why is this needed??

  }

  //
  // Disable code below
  //
  //
  if (false) {
    long tNow_ms = millis();
    int setNotMoving = (speed_kmh == 0 && cadence_rpm == 0 && cadenceSwitch.getInteruptActive() && speedSwitch.getInteruptActive());

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
        //ledPreviousIntensity = rearLed.getICurrentIntensity();
        rearLed.setLedToggleMax();
        brakeLed.setLedToggleMax();
      }
    }
    //
    // When the brakeswitch is applied too long switch off the breaklights.
    //
    else if (brakeSwitch.getState() == LOW
             && !brakeSwitch.hasStateChanged()
             && tNow_ms - brakeSwitch.getTimeLastChange_ms() > 1000 * tDurationBrakeLight_s
             && rearLed.getICurrentIntensity() == rearLed.IMax())
    {
      rearLed.setLedToggleMax();
      brakeLed.setLedOff();
    }
    //
    // brakeswitch is off -> return to previous state. Only when the brakeled is max.
    //
    else if (brakeSwitch.getState() == HIGH
             && brakeSwitch.hasStateChanged()
             && rearLed.getICurrentIntensity() == rearLed.IMax())
    {
      rearLed.setLedToggleMax();
      brakeLed.setLedOff();
    }

    //
    // Brakeswitch is on, upswitch has changed- > default goes up as fog light
    //
    else if (brakeSwitch.getState() == LOW
             && upSwitch.getState() == LOW
             && upSwitch.hasStateChanged())
    {
      rearLed.setLedToggleMax();
      rearLed.upLed();
      //ledPreviousIntensity = rearLed.getICurrentIntensity();
      brakeLed.setLedOff();
    }

    //
    // Brakeswicht is on, downswitch has changed -> turn off fog light or go to dim
    //
    else if (brakeSwitch.getState() == LOW
             && downSwitch.getState() == LOW
             && downSwitch.hasStateChanged())
    {
      rearLed.setLedToggleMax();
      rearLed.downLed();
      //ledPreviousIntensity = rearLed.getICurrentIntensity();
      brakeLed.setLedOff();
    }


    //
    // When standing still AND not pedaling, turn it off. Note this only works when both speed and cadende are working
    //
    if (setNotMoving
        && brakeSwitch.getState() == LOW)
    {
      rearLed.setLedToggleMax();
      brakeLed.setLedOff();
    }

    //
    // When riding with fog lights (ie rear set to rearLedMediumIntensity we flash the rear light once every N seconds.
    // Do not do this when the brake is applied. Only for the QuatroVelo, DF & other three-wheelers don't have aux light.
    //
    //#if defined(QUATRO)
    if (rearLed.getICurrentIntensity() >= 2
        && brakeSwitch.getState() == HIGH)
    {
      // When high, set to off
      if (brakeLed.getICurrentIntensity() == brakeLed.IMax()
          // Go high for 1/4th of the blink frequency duration
          && (tNow_ms - brakeLed.getTimeLastChange_ms()) >= tFogFlashHigh_ms)
      {
        brakeLed.setLedToggleMax();
      }
      // when off set to high
      else if (brakeLed.getICurrentIntensity() <= 1
               // Go low for 10 times the blink duration
               && (tNow_ms - brakeLed.getTimeLastChange_ms()) >= tFogFlashLow_ms)
      {
        brakeLed.setLedToggleMax();
      }

    }
    //#endif
    //#if defined(ICB_DF)
    //if (rearLed.getLedIntensity() == rearLedMediumIntensity
    //      && brakeSwitch.getState() == HIGH)
    //  {
    //    if (   (rearLed.getFlashOnStatus() && (tNow_ms - rearLed.getTimeLastChange_ms()) >= tFogFlashHigh_ms)
    //        || (!rearLed.getFlashOnStatus() && (tNow_ms - rearLed.getTimeLastChange_ms()) >= tFogFlashLow_ms))
    //    {
    //        rearLed.toggleFlashLed();
    //    }
    //  }
    //#endif
  }
}

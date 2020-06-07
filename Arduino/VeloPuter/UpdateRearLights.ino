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
      brakeLed.setLedMax();
    }
  }
  //
  // When the brakeswitch is applied too long switch off the breaklights.
  //
  else if (brakeSwitch.getState() == LOW 
           && !brakeSwitch.hasStateChanged() 
           && tNow_ms - brakeSwitch.getTimeLastChange_ms() > 1000*tDurationBrakeLight_s 
           && rearLed.getICurrentIntensity() == rearLed.IMax())
  {
      rearLed.setLedIntensity(ledPreviousIntensity);
      brakeLed.setLedOff();
  }
  //
  // brakeswitch is off -> return to previous state. Only when the brakeled is max.
  //
  else if (brakeSwitch.getState() == HIGH 
           && brakeSwitch.hasStateChanged() 
           && rearLed.getLedIntensity() == rearLed.IMax())
  {
    rearLed.setLedIntensity(ledPreviousIntensity);
    brakeLed.setLedOff();
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
    brakeLed.setLedOff();
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
    brakeLed.setLedOff();
  }


  //
  // When standing still AND not pedaling, turn it off. Note this only works when both speed and cadende are working
  //
  if (setNotMoving
      && brakeSwitch.getState() == LOW)
    {
      rearLed.setLedIntensity(ledPreviousIntensity);
      brakeLed.setLedOff();
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
      if (brakeLed.getLedIntensity() == brakeLedMaxIntensity 
          // Go high for 1/4th of the blink frequency duration
          && (tNow_ms - brakeLed.getTimeLastChange_ms()) >= tFogFlashHigh_ms)
          {
              brakeLed.setLedOff();
          }
      // when off set to high
      else if (brakeLed.getLedIntensity() == brakeLedOffIntensity 
          // Go low for 10 times the blink duration
          && (tNow_ms - brakeLed.getTimeLastChange_ms()) >= tFogFlashLow_ms)
          {
              brakeLed.setLedMax();
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

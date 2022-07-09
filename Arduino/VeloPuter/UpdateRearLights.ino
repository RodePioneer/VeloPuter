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

  /****************************************************************************************

    Get the state of affairs: aa determine the state of all relevant sensors and LEDs.

  ****************************************************************************************/
  long tNow_ms = millis();
  bool setIsMoving = speed_kmh > 0 or speedSwitch.getInteruptActive() == 0;
  bool setIsBrakeSwitchOn      = brakeSwitch.getState() == LOW;
  bool setIsBrakeSwitchChanged = brakeSwitch.hasStateChanged();
  bool setIsBrakeSwitchTimerExpired = (millis() - brakeSwitch.getTimeLastChange_ms()) > (1000 * tDurationBrakeLight_s);
  bool setIsUpSwitchOnChanged   = upSwitch.getState() == LOW && upSwitch.hasStateChanged();
  bool setIsDownSwitchOnChanged = downSwitch.getState() == LOW && downSwitch.hasStateChanged();
  bool setIsBrakeLedOn = brakeLed.getICurrentIntensity() == brakeLed.iIntensityMaxAllowed();
  bool setIsFlashHighExpired = (rearLed.getICurrentIntensity() >= 2) and     setIsBrakeLedOn and ((tNow_ms - brakeLed.getTimeLastChange_ms()) >= tFogFlashHigh_ms);
  bool setIsFlashLowExpired  = (rearLed.getICurrentIntensity() >= 2) and not setIsBrakeLedOn and ((tNow_ms - brakeLed.getTimeLastChange_ms()) >= tFogFlashLow_ms );

  /*
    Set the default actions.
  */
  bool setDoRearOff = false;
  bool setDoRearMax = false;
  bool setDoRearUp = false;
  bool setDoRearDown = false;
  bool setDoBrakeFlashOn = false;
  bool setDoBrakeFlashOff = false;

  /****************************************************************************************

     Determine what to do by analysing the state of affairs

  ****************************************************************************************/
  // TODO fix weird issue with moving and non-noving flash / all lights out.
  //x :- fuck

  // Note that the order is relevant. 

  // The brake switch is not applied but was just released and the LED is on: set all rear lights back to normal.
  if (not setIsBrakeSwitchOn and setIsBrakeLedOn and setIsBrakeSwitchChanged)
  {
    setDoRearOff = true;
  }
  // The up-switch was turned on while the brake switch is applied: increase the rear lights intensity
  else if (setIsBrakeSwitchOn and setIsUpSwitchOnChanged)
  {
    setDoRearUp = true;
  }
  // The down-switch was turned on while the brake switch is applied: reduce the rear lights intensity
  else if (setIsBrakeSwitchOn and setIsDownSwitchOnChanged)
  {
    setDoRearDown = true;
  }
  // The brake switch is applied, but has been for a long time and the brake light is on: assume that we are standing still for a long time and set all rear lights back to normal.
  else if (setIsBrakeSwitchOn and setIsBrakeSwitchTimerExpired and setIsBrakeLedOn)
  {
    setDoRearOff = true;
  }
  // The brake switch is applied, it changed since the last time we checked and we are actually moving (or don't have speed/cadence sensors active): turn on brake and rear lights to show we are braking
  else if (setIsBrakeSwitchOn and setIsBrakeSwitchChanged and setIsMoving)
  {
    setDoRearMax = true;
  }
  // The brake switch is applied but we stopped moving: set all rear lights back to normal.
  else if (setIsBrakeSwitchOn and not setIsMoving)
  {
    setDoRearOff = true;
  }
  // The brake switch is not applied and has not changed and brake light has been ON long enough: turn OFF the brake light.
  else if (not setIsBrakeSwitchOn and not setIsBrakeSwitchChanged and setIsFlashHighExpired and setIsMoving)
  {
    setDoBrakeFlashOff = true;
  }
  // The brake switch is not applied and has not changed and brake light has been OFF long enough: turn ON the brake light.
  else if (not setIsBrakeSwitchOn and not setIsBrakeSwitchChanged and setIsFlashLowExpired and setIsMoving)
  {
    setDoBrakeFlashOn = true;
  }


  /****************************************************************************************

    Update the leds depending on the brake handle

  ****************************************************************************************/

  if (setDoRearMax)
  {
    // Set both LEDs to max but make sure we can set the rear light back to its current value after the brake is released.
    brakeLed.setLedMax();
    rearLed.setLedStoreCurrentIntensityAsPrevious ();
    rearLed.setLedMax();
  }
  else if (setDoRearOff)
  {
    // Turn off the brake light and set the rear lights back to a previous intensity.
    brakeLed.setLedOff();
    rearLed.setLedToPreviousIntensity  ();
  }
  else if (setDoRearUp)
  {
    // Increase the intensity at the back. Since this is a configuration: turn off the brake light.
    brakeLed.setLedOff();
    // Note we do never go up to the max intensity because we need a difference with the brake light (max is reserved for braking)
    rearLed.setLedToPreviousIntensity  ();
    if (rearLed.getICurrentIntensity() < (rearLed.iIntensityMaxAllowed() - 2))
    {
      rearLed.upLed();
      rearLed.setLedStoreCurrentIntensityAsPrevious ();
    }
  }
  else if (setDoRearDown)
  {
    // Decrease the intensity at the back. Since this is a configuration: turn off the brake light.
    brakeLed.setLedOff();
    rearLed.setLedToPreviousIntensity ();
    rearLed.downLed();
    rearLed.setLedStoreCurrentIntensityAsPrevious ();
  }
  else if (setDoBrakeFlashOn)
  {
    // For extra visibility shortly flash the brake light, turn on.
    brakeLed.setLedMax();
  }
  else if (setDoBrakeFlashOff)
  {
    // For extra visibility shortly flash the brake light, turn off.
    brakeLed.setLedOff();
  }
}

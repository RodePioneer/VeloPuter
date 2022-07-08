
/*********************************************************************************************************

    Update the headlights

********************************************************************************************************
**
** On up-switch: more light
** On down0switch: less light
** Conditions:
** Only act when the switch has changed. Not on static
** the brake switch is off
** the config switch is off
*/
void updateHead()
{

  /****************************************************************************************

    Get the state of affairs: aa determine the state of all relevant sensors and LEDs.

  ****************************************************************************************/
  bool setIsBrakeOff = brakeSwitch.getState() == HIGH;
  bool setIsConfigOff = configSwitch.getState() == HIGH;
  bool setIsUpSwitchOnChanged   = upSwitch.getState() == LOW && upSwitch.hasStateChanged();
  bool setIsDownSwitchOnChanged = downSwitch.getState() == LOW && downSwitch.hasStateChanged();

  /*
    Set the default actions.
  */
  bool setDoHeadUp = false;
  bool setDoHeadDown = false;

  /****************************************************************************************

     Determine what to do by analysing the state of affairs

  ****************************************************************************************/
  if (setIsUpSwitchOnChanged and setIsBrakeOff and setIsConfigOff)
  {
    setDoHeadUp = true;
  }
  else if (setIsDownSwitchOnChanged and setIsBrakeOff and setIsConfigOff)
  {
    setDoHeadDown = true;
  }

  /****************************************************************************************

    Update the leds: more or less...
    
  ****************************************************************************************/
  if (setDoHeadUp)
  {
    // more light
    headLed.upLed();
    // Uncomment the next line if there is a headlight attached to aux
    auxLed.upLed();
  }
  else if (setDoHeadDown)
  {
    // less light
    headLed.downLed();
    // Uncomment the next line if there is a headlight attached to aux
    auxLed.downLed();
  }
}

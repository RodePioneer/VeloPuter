
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

  //
  // Determine what to do
  //
  // Check if the brake and the config are off
  bool setIsBrakeOff = brakeSwitch.getState() == HIGH;
  bool setIsConfigOff = configSwitch.getState() == HIGH;

  // Do we up or lower the lights?
  bool setDoHeadUp = upSwitch.getState() == LOW && upSwitch.hasStateChanged() && setIsBrakeOff && setIsConfigOff;
  bool setDoHeadDown = downSwitch.getState() == LOW && downSwitch.hasStateChanged() && setIsBrakeOff && setIsConfigOff;

  //
  // Update the leds
  //
  // more light
  if (setDoHeadUp)
  {
    headLed.upLed();
    // Uncomment the next line if there is a headlight attached to aux2
    //auxLed.upLed();
  }

  // less light
  if (setDoHeadDown)
  {
    headLed.downLed();
    // Uncomment the next line if there is a headlight attached to aux2
    //auxLed.downLed();
  }
}

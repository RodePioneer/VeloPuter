
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
  bool isBrakeOff = brakeSwitch.getState() == HIGH;
  bool isConfigOff = configSwitch.getState() == HIGH;

  // Do we up or lower the lights?
  bool setDoUp = upSwitch.getState() == LOW && upSwitch.hasStateChanged() && isBrakeOff && isConfigOff;
  bool setDoDown = downSwitch.getState() == LOW && downSwitch.hasStateChanged() && isBrakeOff && isConfigOff;

  //
  // Update the leds
  //
  // more light
  if (setDoUp)
  {
    headLed.upLed();
    // Uncomment the next line if there is a headlight attached to aux2
    //aux2Led.upLed();
  }

  // less light
  if (setDoDown)
  {
    headLed.downLed();
    // Uncomment the next line if there is a headlight attached to aux2
    //aux2Led.downLed();
  }
}

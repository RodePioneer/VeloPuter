
/**********************************************************************************************

   Functions which update the states and .

 **********************************************************************************************/


/*********************************************************************************************************

    The configuration interface

 *********************************************************************************************************
   The configuration manages
   - Intensity of the oled display. (push = toggle
   - Performing the battery check. THis DISABLES the battery management and watchdog.
*/
void updateConfig()
{
  /****************************************************************************************

    Get the state of affairs: aa determine the state of all relevant sensors and LEDs.

  ****************************************************************************************/
  bool setIsConfigSwitchOn = configSwitch.getState() == LOW;
  bool setIsConfigSwitchChanged = configSwitch.hasStateChanged() == 1;
  bool setIsUpSwitchOn = upSwitch.getState() == LOW;
  bool setIsDownSwitchOn = downSwitch.getState() == LOW;

  bool setDoPowerManagementOff = false;
  bool setDoPowerManagementOn = false;
  bool setToggleOledIntensity = false;

  /****************************************************************************************

    Determine what to do by analysing the state of affairs

  ****************************************************************************************/
  // See of there is also a up or down switch active: disable or enable the battery check.
  if (setIsConfigSwitchOn and setIsDownSwitchOn)
  {
    setDoPowerManagementOff = true;
  }
  else if (setIsConfigSwitchOn and setIsUpSwitchOn)
  {
    setDoPowerManagementOn = true;
  }
  else if (setIsConfigSwitchOn and setIsConfigSwitchChanged)
  {
    setToggleOledIntensity = true;
  }
  /****************************************************************************************

    Update the Oled or the powermanagement depending on the brake handle

  ****************************************************************************************/
  // See of there is also a up or down switch active: disable or enable the battery check.
  if (setDoPowerManagementOff)
  {
    doBatteryCheck = false;
    myBattery.setBatteryColorCode(BATTERY_GREEN);
  }
  else if (setDoPowerManagementOn)
  {
    doBatteryCheck = true;
  }
  else if (setToggleOledIntensity)
  {
    if (setOledIntensity >= 128)
    {
      setOledIntensity = 0;
    }
    else
    {
      setOledIntensity = 255;
    }
    // TODO: why the begin? Find out and / or explain
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

    Calculate the gear

  We use the ratio between the speed and cadence to work out the gear on the cassete, given a vertain chainring.
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

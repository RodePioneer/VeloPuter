
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

    The configuration interface

 *********************************************************************************************************
 * The configuration manages 
 * - Intensity of the oled display. (push = toggle
 * - Performing the battery check. THis DISABLES the battery management and watchdog.
 */
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

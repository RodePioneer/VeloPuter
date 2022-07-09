
/*********************************************************************************************************

    Update the battery

 *********************************************************************************************************/
// ** TODO: only update when there is a chagne..... not every bloody run.
void updatePowerManagement()
{ /*
    Determnine battery regime

     Green: normal operation
     Orange: we need to preserve power. No more high beams, lower intensity on all LEDs.
     Red: okay now we are almost fucked.
     Black: down. We are now dead.

  */
  //  long tNow_ms = millis();
  //  long tLastCheck_ms = tNow_ms;
  //  const long tTimeBetweenBatteryUpdates_ms = 3000;
  //
  //  tNow_ms = millis();
  //
  //  if ((tNow_ms - tLastCheck_ms) > tTimeBetweenBatteryUpdates_ms)
  //  {
  //    tLastCheck_ms = tNow_ms;
  myBattery.updateBattery(); // Update the battery status

  if (doBatteryCheck) {

    // The state of the battery
    byte setCurrentBatteryColorCode = myBattery.getBatteryColorCode();


    /*
       We update the color coding for all the LED's
       Inside the LED update routing this information is used to determine what is the actual max which we may set.
    */
    headLed.setMyBatteryGetColorCode(setCurrentBatteryColorCode);
    auxLed.setMyBatteryGetColorCode(setCurrentBatteryColorCode);
    rearLed.setMyBatteryGetColorCode(setCurrentBatteryColorCode);
    brakeLed.setMyBatteryGetColorCode(setCurrentBatteryColorCode);
    rightLed.setMyBatteryGetColorCode(setCurrentBatteryColorCode);
    leftLed.setMyBatteryGetColorCode(setCurrentBatteryColorCode);

    //
    // Aditionally: if we went to black: kill the interupts so we won't wake up when we go to sleep.
    // The only way to get back online is to disconnect the power and then power up again.
    //
    if (setCurrentBatteryColorCode == BATTERY_BLACK)
    {
      /*
        The battery is almost dead. We now power it down.
        Aditionally we kill the interupts so we won't wake up.

        The only way to get back online is to disconnect the power
        and then power up again.

        Optionally disable the battery check to prevent unwanted shutdowns.
      */
      detachInterrupt(digitalPinToInterrupt(switchSpdPin));
      detachInterrupt(digitalPinToInterrupt(switchCadPin));
      sleepNow ();
    }
  }
  else
  {
    //
    // Monitoring is disables. Set everything to green.
    // This is dangerous but may be nececary in emergencies !!!!
    //
    headLed.setMyBatteryGetColorCode(BATTERY_GREEN);
    auxLed.setMyBatteryGetColorCode(BATTERY_GREEN);
    rearLed.setMyBatteryGetColorCode(BATTERY_GREEN);
    brakeLed.setMyBatteryGetColorCode(BATTERY_GREEN);
    rightLed.setMyBatteryGetColorCode(BATTERY_GREEN);
    leftLed.setMyBatteryGetColorCode(BATTERY_GREEN);
  }
  //}

}

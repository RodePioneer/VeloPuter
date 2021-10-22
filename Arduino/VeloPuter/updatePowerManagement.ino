
/*********************************************************************************************************

    Update the battery

 *********************************************************************************************************/
// ** TODO: only update when there is a chagne..... not every bloody run.
void updatePowerManagement()
{ /*
    Determnine battery regime

     Green: normal operation
     Orange: we need to preserve power.
     Red: okay now we are almost fucked.
     Black: down. We are now dead.

  */
  byte setPreviousBatteryColorCode = BATTERY_GREEN;
  byte setBatteryStatusHasChanged = false;

  if (doBatteryCheck) {

    int setCurrentBatteryColorCode = myBattery.getColorCode();
    //byte setBatteryStatusHasChanged = myBattery.getBatteryStatusHasChanged();

    setBatteryStatusHasChanged = setPreviousBatteryColorCode != setCurrentBatteryColorCode;

    if (setBatteryStatusHasChanged)
    {
      if (setCurrentBatteryColorCode == BATTERY_GREEN)       setBatteryToGreen ();
      else if (setCurrentBatteryColorCode == BATTERY_RED)    setBatteryToRed ();
      else if (setCurrentBatteryColorCode == BATTERY_ORANGE) setBatteryToOrange ();
      else if (setCurrentBatteryColorCode == BATTERY_BLACK)  setBatteryToBlack ();
    }
  }

}

void setBatteryToGreen ()
{ /*
    GREEN

    No limitations
  */

  headLed.SetBatteryLimit(0);
  rearLed.SetBatteryLimit(0);
  brakeLed.SetBatteryLimit(0);
  rightLed.SetBatteryLimit(0);
  leftLed.SetBatteryLimit(0);
  auxLed.SetBatteryLimit(0);
}

void setBatteryToOrange ()
{
  /*
    ORANGE

    No high beam
    Dim indicators
    Dim brakelight
  */
  headLed.SetBatteryLimit(1);
  rearLed.SetBatteryLimit(1);
  brakeLed.SetBatteryLimit(1);
  rightLed.SetBatteryLimit(1);
  leftLed.SetBatteryLimit(1);
  auxLed.SetBatteryLimit(1);
}

void setBatteryToRed ()
{
  /* RED

     We are now more pressen for power consumption.
     No indicators, > just a little hint during the night. No more. Too dim for bright days.
     No brakelight
     No fog light
  */
  headLed.SetBatteryLimit(2);
  rearLed.SetBatteryLimit(2);
  brakeLed.SetBatteryLimit(2);
  rightLed.SetBatteryLimit(2);
  leftLed.SetBatteryLimit(2);
  auxLed.SetBatteryLimit(2);
}

void setBatteryToBlack ()
{
  /*
    The battery is almost dead. We now power it down.
    No more fun. Only option: switch off and on again.
    Optionally disable the battery check to prevent unwanted shutdowns.
  */
  detachInterrupt(digitalPinToInterrupt(switchSpdPin));
  detachInterrupt(digitalPinToInterrupt(switchCadPin));

  sleepNow ();
}

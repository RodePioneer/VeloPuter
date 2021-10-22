
/*********************************************************************************************************

    Update the battery

 *********************************************************************************************************/
void updatePowerManagement()
{ /*
    Determnine battery regime

     Green: normal operation
     Orange: we need to preserve power.
     Red: okay now we are almost fucked.
     Black: down. We are now dead.

     */
  if (doBatteryCheck) {
    if (myBattery.getColorCode() == BATTERY_GREEN)       setBatteryToGreen ();
    else if (myBattery.getColorCode() == BATTERY_RED)    setBatteryToRed ();
    else if (myBattery.getColorCode() == BATTERY_ORANGE) setBatteryToOrange ();
    else if (myBattery.getColorCode() == BATTERY_BLACK)  setBatteryToBlack ();
  }
}

void setBatteryToGreen ()
{
  headLed.setBatteryLimit = 0;
  rearLed.setBatteryLimit = 0;
  brakeLed.setBatteryLimit = 0;
  rightLed.setBatteryLimit = 0;
  leftLed.setBatteryLimit = 0;
  auxLed.setBatteryLimit = 0;
  }

void setBatteryToOrange ()
{
  /*
    ORANGE

    No high beam
    Dim indicators
    Dim brakelight
  */
  headLed.setBatteryLimit = 1;
  rearLed.setBatteryLimit = 1;
  brakeLed.setBatteryLimit = 1;
  rightLed.setBatteryLimit = 1;
  leftLed.setBatteryLimit = 1;
  auxLed.setBatteryLimit = 1;
}

void setBatteryToRed ()
{
  /* RED

     We are now more pressen for power consumption.
     No indicators, > just a little hint during the night. No more. Too dim for bright days.
     No brakelight
     No fog light
  */
  headLed.setBatteryLimit = 2;
  rearLed.setBatteryLimit = 2;
  brakeLed.setBatteryLimit = 2;
  rightLed.setBatteryLimit = 2;
  leftLed.setBatteryLimit = 2;
  auxLed.setBatteryLimit = 2;
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

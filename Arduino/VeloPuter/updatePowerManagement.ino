
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
  headLed.setBatteryStatus = BATTERY_GREEN;
  rearLed.setBatteryStatus = BATTERY_GREEN;
  brakeLed.setBatteryStatus = BATTERY_GREEN;
  rightLed.setBatteryStatus = BATTERY_GREEN;
  leftLed.setBatteryStatus = BATTERY_GREEN;
  auxLed.setBatteryStatus = BATTERY_GREEN;
  }

void setBatteryToOrange ()
{
  /*
    ORANGE

    No high beam
    Dim indicators
    Dim brakelight
  */
  headLed.setBatteryStatus = BATTERY_ORANGE;
  rearLed.setBatteryStatus = BATTERY_ORANGE;
  brakeLed.setBatteryStatus = BATTERY_ORANGE;
  rightLed.setBatteryStatus = BATTERY_ORANGE;
  leftLed.setBatteryStatus = BATTERY_ORANGE;
  auxLed.setBatteryStatus = BATTERY_ORANGE;
}

void setBatteryToRed ()
{
  /* RED

     We are now more pressen for power consumption.
     No indicators, > just a little hint during the night. No more. Too dim for bright days.
     No brakelight
     No fog light
  */
  headLed.setBatteryStatus = BATTERY_RED;
  rearLed.setBatteryStatus = BATTERY_RED;
  brakeLed.setBatteryStatus = BATTERY_RED;
  rightLed.setBatteryStatus = BATTERY_RED;
  leftLed.setBatteryStatus = BATTERY_RED;
  auxLed.setBatteryStatus = BATTERY_RED;
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

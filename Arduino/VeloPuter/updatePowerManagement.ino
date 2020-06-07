 
/*********************************************************************************************************

    Update the battery

 *********************************************************************************************************/
void updatePowerManagement()
{  /*
     Determnine battery regime

      Green: normal operation
      Orange: we need to preserve power.
      Red: okay now we are almost fucked.
      Black: down. We are now dead.

//  */
if (doBatteryCheck) {
    if (myBattery.getColorCode() == BATTERY_GREEN )
      setBatteryToGreen ();
    else if (myBattery.getColorCode() == BATTERY_RED )
    {
      setBatteryToRed ();
    }
    else if (myBattery.getColorCode() == BATTERY_ORANGE )
    {
      setBatteryToOrange ();
    }
    else if (myBattery.getColorCode() == BATTERY_BLACK)
    {
      setBatteryToBlack ();
    }
  }
}

void setBatteryToGreen ()
{}

void setBatteryToOrange ()
{
  /*
    ORANGE

    No high beam
    Dim indicators
    Dim brakelight
  */
//  headLed.highIntensity = headLedMediumIntensity;
//  headLed.maxIntensity = headLedMediumIntensity ;
//  headLed.setLedIntensity (min(headLed.getLedIntensity(), headLedMediumIntensity));
//
//  rearLed.maxIntensity = rearLedHighIntensity;
//  rearLed.setLedIntensity (min(rearLed.getLedIntensity(), rearLedMediumIntensity));
//
//  /*
//     Less bright brakelight
//  */
//  brakeLed.highIntensity = brakeLedMediumIntensity;
//  brakeLed.maxIntensity = brakeLedMediumIntensity;
//  brakeLed.setLedIntensity (min(brakeLed.getLedIntensity(), brakeLedMediumIntensity));
//
//  rightLed.offIntensity =    rightLedOffIntensity;
//  rightLed.lowIntensity =    rightLedOffIntensity;
//  rightLed.mediumIntensity = rightLedOffIntensity;
//  rightLed.highIntensity =   rightLedHighIntensity; // 1/2th the power
//  rightLed.maxIntensity =    rightLedHighIntensity;
//  rightLed.setLedIntensity (min(rightLed.getLedIntensity(), rightLedMediumIntensity));
//
//
//  leftLed.offIntensity =    leftLedOffIntensity;
//  leftLed.lowIntensity =    leftLedOffIntensity;
//  leftLed.mediumIntensity = leftLedOffIntensity;
//  leftLed.highIntensity =   leftLedHighIntensity;
//  leftLed.maxIntensity =    leftLedHighIntensity;
//  leftLed.setLedIntensity (min(leftLed.getLedIntensity(), leftLedMediumIntensity));
}

void setBatteryToRed ()
{
  /* RED

     We are now more pressen for power consumption.
     No indicators, > just a little hint during the night. No more. Too dim for bright days.
     No brakelight
     No fog light
  */
//  headLed.highIntensity = headLedLowIntensity;
//  headLed.maxIntensity = headLedLowIntensity ;
//  headLed.setLedIntensity (min(headLed.getLedIntensity(), headLedMediumIntensity));
//
//  rightLed.offIntensity =    rightLedOffIntensity;
//  rightLed.lowIntensity =    rightLedOffIntensity;
//  rightLed.mediumIntensity = rightLedOffIntensity;
//  rightLed.highIntensity =   rightLedOffIntensity; // 1/2th the power
//  rightLed.maxIntensity =    rightLedOffIntensity;
//  rightLed.setLedIntensity (min(rightLed.getLedIntensity(), rightLedMediumIntensity));
//
//
//  leftLed.offIntensity =    leftLedOffIntensity;
//  leftLed.lowIntensity =    leftLedOffIntensity;
//  leftLed.mediumIntensity = leftLedOffIntensity;
//  leftLed.highIntensity =   leftLedOffIntensity;
//  leftLed.maxIntensity =    leftLedOffIntensity;
//  leftLed.setLedIntensity (min(leftLed.getLedIntensity(), leftLedMediumIntensity));
//
//
//  rearLed.setPinID(ledRearPin);
//  rearLed.offIntensity =    rearLedOffIntensity;
//  rearLed.lowIntensity =    rearLedLowIntensity;
//  rearLed.mediumIntensity = rearLedLowIntensity;
//  rearLed.highIntensity =   rearLedLowIntensity;
//  rearLed.maxIntensity =    rearLedLowIntensity;
//  rearLed.setLedIntensity (min(rearLed.getLedIntensity(), rearLedMediumIntensity));
}

void setBatteryToBlack ()
{
  /*
    The battery is almost dead. We now power it down.
    No more fun. Only option: switch off and on again.
  */
  detachInterrupt(digitalPinToInterrupt(switchSpdPin));
  detachInterrupt(digitalPinToInterrupt(switchCadPin));

  sleepNow ();
}

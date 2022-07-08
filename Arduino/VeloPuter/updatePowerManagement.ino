
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
  bool setBatteryStatusHasChanged = false;


  myBattery.updateBatteryColorCode(); // Update the battery status

  if (doBatteryCheck) {

    // The state of the battery
    byte setCurrentBatteryColorCode = myBattery.getColorCode();

    // Has it changed?
    setBatteryStatusHasChanged = setPreviousBatteryColorCode != setCurrentBatteryColorCode;

    //
    // If the battery state has changed: we need to take some action.
    //
    //
    // This mainly copies the battery status to the LED's
    //
   // if (setBatteryStatusHasChanged)
  ////  {
      /*
       * We update the color coding for all the LED's
       * Inside the LED update routing this information is used to determine what is the actual max which we may set. 
       */
      headLed.setMyBatteryGetColorCode(setCurrentBatteryColorCode);
      auxLed.setMyBatteryGetColorCode(setCurrentBatteryColorCode);
      rearLed.setMyBatteryGetColorCode(setCurrentBatteryColorCode);
      brakeLed.setMyBatteryGetColorCode(setCurrentBatteryColorCode);
      rightLed.setMyBatteryGetColorCode(setCurrentBatteryColorCode);
      leftLed.setMyBatteryGetColorCode(setCurrentBatteryColorCode);
  //  }

    //
    // Aditionally: if we went to black: kill the interupts so we won't wake up when we go to sleep.
    // The only way to get back online is to disconnect the power and then power up again. 
    //
    if (setBatteryStatusHasChanged and (setCurrentBatteryColorCode == BATTERY_BLACK))
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

  }

}

//void setBatteryToGreen ()
//{ /*
//    GREEN
//
//    No limitations
//  */
//
//  rearLed.SetBatteryLimit(0);
//  brakeLed.SetBatteryLimit(0);
//  rightLed.SetBatteryLimit(0);
//  leftLed.SetBatteryLimit(0);
//  auxLed.SetBatteryLimit(0);
//}
//
//void setBatteryToOrange ()
//{
//  /*
//    ORANGE
//
//    No high beam
//    Dim indicators
//    Dim brakelight
//  */
//  headLed.SetBatteryLimit(1);
//  rearLed.SetBatteryLimit(1);
//  brakeLed.SetBatteryLimit(1);
//  rightLed.SetBatteryLimit(1);
//  leftLed.SetBatteryLimit(1);
//  auxLed.SetBatteryLimit(1);
//}
//
//void setBatteryToRed ()
//{
//  /* RED
//
//     We are now more pressen for power consumption.
//     No indicators
//     No brakelight
//     Dim rear light
//  */
//  headLed.SetBatteryLimit(2);
//  rearLed.SetBatteryLimit(2);
//  brakeLed.SetBatteryLimit(2);
//  rightLed.SetBatteryLimit(2);
//  leftLed.SetBatteryLimit(2);
//  auxLed.SetBatteryLimit(2);
//}

//void setBatteryToBlack ()
//{
//  /*
//    The battery is almost dead. We now power it down.
//    No more fun. Only option: switch off and on again.
//    Optionally disable the battery check to prevent unwanted shutdowns.
//  */
//  detachInterrupt(digitalPinToInterrupt(switchSpdPin));
//  detachInterrupt(digitalPinToInterrupt(switchCadPin));
//
//  sleepNow ();
//}

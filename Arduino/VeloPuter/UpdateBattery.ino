
/*********************************************************************************************************

    Update the battery

 *********************************************************************************************************/
void updateBattery()
{

  /*
    Reads the value from the specified analog pin. The Arduino board contains a 6
    channel (8 channels on the Mini and Nano, 16 on the Mega), 10-bit analog to
    digital converter. This means that it will map input voltages between 0 and 5
    volts into integer values between 0 and 1023. This yields a resolution between
    readings of: 5 volts / 1024 units or, .0049 volts (4.9 mV) per unit. The input
    range and resolution can be changed using analogReference().

    Use a cumulative moving average to keep track of the voltage. Note that this implementation
    of a CMA means that the influence of every measurement decays exponentially.


    TODO:
    - use hysteresis to enable going up a state when the battery is switched
    - use a minimal time below the trhesold before lowering the state
    - restructure for better readability of the code
  */
  long tNow_ms = millis();
  //float PinValue = analogRead(voltagePin);
  //static float PinMean = PinValue; // it is inutialised at the fist value and then kept in memory.

  //const byte numSamples = 10 ; // was 20;

  //float batteryVoltage_mv;
  //byte numOfCells = 1;

  //PinMean = (PinMean * (numSamples - 1) + PinValue) / numSamples; // the mean voltage on the pin.


  //batteryVoltage_mv = 24.008 + 19.368 * PinMean ; // Voltage measurements done and linear fit calculated in octave 2020-05-03 @ Boekelo

  //batteryVoltage_mv = myBattery.getVoltage_mv();


  /*
    Assume the cells are no less then 3.2 V and no more 4.2V. We cut off at 3.5V.
    cutoffs for 3 cells between 9.0 and 12.8 V.

    For 3 or 4 cell lipo
    1 cell   3.2 -  3.5 -  4.2
    2 cell   6.4 -  7.0 -  8.4
    3 cell   9.6 - 10.5 - 12.6
    4 cell  12.8 - 14.0 - 16.8
    5 cell  16.0 - 17.5 - 21.0

    Sensible thresholds lie between de full voltage and the 3.2 or 3.5 V

    Note that 1 cell batteries will not suppy enough voltage for the Veloputer.
    For most practical purposes the minimun is 3 cells.
  */

//  if (batteryVoltage_mv >= 17000) numOfCells = 1;       // 5 or more: dislay voltage 4.2*4 = 16.8
//  else if (batteryVoltage_mv >= 12800) numOfCells = 4;  // 4 cells
//  else if (batteryVoltage_mv >= 9000) numOfCells  = 3;  // 3 cells
//  else if (batteryVoltage_mv >= 6000) numOfCells  = 2;  // 5 cells

//  cellVoltage_mv = batteryVoltage_mv / numOfCells;


#if defined(BATTERY_LIPO)
#define NUM_REFDATA 6




  /*
     Determine the battery percentage. This is a measured dischage curve for LiPo.

     measurement data
     Vlow   Vmid  Vhigh   C
     3.78   3.73  x       (5000-2600)/5000 = 48%
     4.2 = 100%
     4.0 = 90 %
     3.75 = 50%
     3.4 = 5%
     V = [4.2, 4.0, 3.75, 3.4]
     C = [100, 90, 50, 0]

    LiPo
  */
  const int V[NUM_REFDATA] = {4200, 4000, 3850, 3750, 3450, 3350};  //mV
  const int C[NUM_REFDATA] = {100,    90,   75,   50,    7,    0};      // % capacity
#elif defined(BATTERY_LIFEPO4)
#define NUM_REFDATA 7
  //
  // LiFePO4
  //

  // Constants from manual, better data is gathered.
  const int V[NUM_REFDATA] = {13952, 12847, 12733, 12455, 12169, 11755, 10424}; // mV
  const int C[NUM_REFDATA] = {  100,    75,    50,    20,    10,     5,     0}; // %capacity
  numOfCells = 1;
#endif


  batteryPercentage_pct = 0;
  for (int i = NUM_REFDATA - 1; i > 0; i--)
  {
    if (myBattery.getVoltageCell_mv() >= V[i])
      batteryPercentage_pct = int((C[i - 1] - C[i]) * (myBattery.getVoltageCell_mv() - V[i]) / (V[i - 1] - V[i]) + C[i]);
  }

  batteryPercentage_pct = constrain(batteryPercentage_pct, 0, 99);

  //statusBattery = batteryPercentageToStatus (int batteryPercentage_pct);

  /*
     Determnine battery regime

      Green: normal operation
      Orange: we need to preserve power.
      Red: okay now we are almost fucked.
      Black: down. We are now dead.

  */
  if (doBatteryCheck && tNow_ms > 1000 * tDelayBatteryCheck_s) {
    const int Batt_pct_limits[3] = {25, 15, 5};
    if (batteryPercentage_pct < Batt_pct_limits[0] && statusBattery == BATTERY_GREEN )
    {
      setBatteryToRed ();
    }
    if (batteryPercentage_pct < Batt_pct_limits[1]  && statusBattery == BATTERY_ORANGE)
    {
      setBatteryToRed ();
    }
    if (batteryPercentage_pct < Batt_pct_limits[2]  && statusBattery == BATTERY_RED)
    {
      setBatteryToBlack ();
    }
  }
}

//byte batteryPercentageToStatus (int batteryPercentage_pct)
//{
//  /*
//    Determnine battery regime
//
//    Green: normal operation
//    Orange: we need to preserve power.
//    Red: okay now we are almost fucked.
//    Black: down. We are now dead.
//
//  */
//  if (doBatteryCheck && tNow_ms > 1000 * tDelayBatteryCheck_s) {
//    const int Batt_pct_limits[3] = {25, 15, 5};
//    if (batteryPercentage_pct < Batt_pct_limits[0] && statusBattery == BATTERY_GREEN )
//    {
//      //setBatteryToRed ();
//      statusBattery == BATTERY_RED
//    }
//    if (batteryPercentage_pct < Batt_pct_limits[1]  && statusBattery == BATTERY_ORANGE)
//    {
//      setBatteryToRed ();
//    }
//    if (batteryPercentage_pct < Batt_pct_limits[2]  && statusBattery == BATTERY_RED)
//    {
//      setBatteryToBlack ();
//    }
//  }
//  return statusBattery;
//}

void setBatteryToOrange ()
{
  /*
    ORANGE

    No high beam
    Dim indicators
    Dim brakelight
  */
  statusBattery = BATTERY_ORANGE;

  headLed.highIntensity = headLedMediumIntensity;
  headLed.maxIntensity = headLedMediumIntensity ;
  headLed.setLedIntensity (min(headLed.getLedIntensity(), headLedMediumIntensity));

  rearLed.maxIntensity = rearLedHighIntensity;
  rearLed.setLedIntensity (min(rearLed.getLedIntensity(), rearLedMediumIntensity));

  /*
     Less bright brakelight
  */
  auxLed.highIntensity = auxLedMediumIntensity;
  auxLed.maxIntensity = auxLedMediumIntensity;
  auxLed.setLedIntensity (min(auxLed.getLedIntensity(), auxLedMediumIntensity));

  rightLed.offIntensity =    rightLedOffIntensity;
  rightLed.lowIntensity =    rightLedOffIntensity;
  rightLed.mediumIntensity = rightLedOffIntensity;
  rightLed.highIntensity =   rightLedHighIntensity; // 1/2th the power
  rightLed.maxIntensity =    rightLedHighIntensity;
  rightLed.setLedIntensity (min(rightLed.getLedIntensity(), rightLedMediumIntensity));


  leftLed.offIntensity =    leftLedOffIntensity;
  leftLed.lowIntensity =    leftLedOffIntensity;
  leftLed.mediumIntensity = leftLedOffIntensity;
  leftLed.highIntensity =   leftLedHighIntensity;
  leftLed.maxIntensity =    leftLedHighIntensity;
  leftLed.setLedIntensity (min(leftLed.getLedIntensity(), leftLedMediumIntensity));
}

void setBatteryToRed ()
{
  /* RED

     We are now more pressen for power consumption.
     No indicators, > just a little hint during the night. No more. Too dim for bright days.
     No brakelight
     No fog light
  */
  headLed.highIntensity = headLedLowIntensity;
  headLed.maxIntensity = headLedLowIntensity ;
  headLed.setLedIntensity (min(headLed.getLedIntensity(), headLedMediumIntensity));

  rightLed.offIntensity =    rightLedOffIntensity;
  rightLed.lowIntensity =    rightLedOffIntensity;
  rightLed.mediumIntensity = rightLedOffIntensity;
  rightLed.highIntensity =   rightLedOffIntensity; // 1/2th the power
  rightLed.maxIntensity =    rightLedOffIntensity;
  rightLed.setLedIntensity (min(rightLed.getLedIntensity(), rightLedMediumIntensity));


  leftLed.offIntensity =    leftLedOffIntensity;
  leftLed.lowIntensity =    leftLedOffIntensity;
  leftLed.mediumIntensity = leftLedOffIntensity;
  leftLed.highIntensity =   leftLedOffIntensity;
  leftLed.maxIntensity =    leftLedOffIntensity;
  leftLed.setLedIntensity (min(leftLed.getLedIntensity(), leftLedMediumIntensity));


  rearLed.setPinID(ledRearPin);
  rearLed.offIntensity =    rearLedOffIntensity;
  rearLed.lowIntensity =    rearLedLowIntensity;
  rearLed.mediumIntensity = rearLedLowIntensity;
  rearLed.highIntensity =   rearLedLowIntensity;
  rearLed.maxIntensity =    rearLedLowIntensity;
  rearLed.setLedIntensity (min(rearLed.getLedIntensity(), rearLedMediumIntensity));

  statusBattery = BATTERY_RED;
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

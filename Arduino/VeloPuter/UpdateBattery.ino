
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
  */
  long tNow_ms = millis();
  float PinValue = analogRead(voltagePin);
  static float PinMean = PinValue; // it is inutialised at the fist value and then kept in memory.

  const byte numSamples = 10 ; // was 20;
 // const float VRef = 5.0; // MEASURED 2020-04-18

  float batteryVoltage_mv;
  // is a global variable, do not declare here.
  // int cellVoltage_mv;
  byte numOfCells;

  PinMean = (PinMean * (numSamples - 1) + PinValue) / numSamples; // the mean voltage on the pin.

  //batteryVoltage_mv = 4326 * (VRef / 1024) * PinMean; // 4.326 was measured useing a voltage meter
  // batteryVoltage_mv = 4000 * (VRef / 1024) * PinMean ; // 4.326 was measured useing a voltage meter
  
  batteryVoltage_mv = 24.008 + 19.368 * PinMean ; // Voltage measurements done and linear fit calculated in octave 2020-05-03 @ Boekelo
                    //0.020824 + 0.020834 * PinMean ; // Voltage measurements done and linear fit calculated in octave 2020-05-03 @ Boekelo

#if defined(BATTERY_LIPO)
#define NUM_REFDATA 6

  // Assume the cells are no less then 3.2 V and no more 4.2V.
  // cutoffs for 3 cells between 9.0 and 12.8 V.

  /* For 3 or 4 cell lipo*/
    if (batteryVoltage_mv >= 16800) numOfCells = 1; // 5 or more: dislay voltage 4.2*4 = 16.8
    else if (batteryVoltage_mv >= 12800) numOfCells = 4; // 4 cells
    else if (batteryVoltage_mv >= 9000) numOfCells  = 3;  // 3 cells
    else if (batteryVoltage_mv >= 5000) numOfCells  = 2;
    else numOfCells = 1;                               // 1,2 cells or other than lipo

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

  */
  //
  // LiPo
  //
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

  cellVoltage_mv = batteryVoltage_mv / numOfCells;
  
  batteryPercentage_pct = 0;
  for (int i = NUM_REFDATA-1; i > 0; i--)
  {
    if (cellVoltage_mv >= V[i])
      batteryPercentage_pct = int((C[i - 1] - C[i]) * (cellVoltage_mv - V[i]) / (V[i - 1] - V[i]) + C[i]);
  }
  //
  //// 84-100% full :
  //  if      (cellVoltage_v >= 4.0) batteryPercentage_pct = int(16.0 * (cellVoltage_v - 4.0) / (4.2 - 4.0) + 84.5);
  //  // 49-84% full
  //  else if (cellVoltage_v >= 3.8) batteryPercentage_pct = int(35.0 * (cellVoltage_v - 3.8) / (4.0 - 3.8) + 49.5);
  //  // 12-49% full
  //  else if (cellVoltage_v >= 3.75) batteryPercentage_pct = int(37.0 * (cellVoltage_v - 3.7) / (3.8 - 3.7) + 12.5);
  //  // 3-12 % full
  //  else if (cellVoltage_v >= 3.4) batteryPercentage_pct = int(9.0  * (cellVoltage_v - 3.4) / (3.7 - 3.4) + 3.5);
  //  // < 3% full
  //  else if (cellVoltage_v <  3.4) batteryPercentage_pct = int(3.0  * (cellVoltage_v - 3.4) / (3.4 - 3.2) + 0.5);


  batteryPercentage_pct = constrain(batteryPercentage_pct, 0, 99);

  /*
     Determnine battery regime

      Green: normal operation
      Orange: we need to preserve power.
      Red: okay now we are almost fucked.
      Black: down. We are now dead.
     
  */
  if (doBatteryCheck) {
    const int Batt_pct_limits[3] = {25, 15, 5};
    if (batteryPercentage_pct < Batt_pct_limits[0] && statusBattery == BATTERY_GREEN && tNow_ms > 10000)
    { /*
       * ORANGE
       * 
         No high beam
         dim blinkers
         dim brakelight
      */
      statusBattery = BATTERY_ORANGE;

      headLed.highIntensity = headLedMediumIntensity;
      headLed.maxIntensity = headLedMediumIntensity ;
      headLed.setLedIntensity (min(headLed.getLedIntensity(), headLedMediumIntensity));

      rearLed.maxIntensity = rearLedHighIntensity;
      rearLed.setLedIntensity (min(rearLed.getLedIntensity(), rearLedMediumIntensity));

      /*
       * Less bright brakelight
       * 
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
    if (batteryPercentage_pct < Batt_pct_limits[1]  && statusBattery == BATTERY_ORANGE)
    {
      /* RED
       * 
         We are now more pressen for power consumption.
         nu blinkers, > just a little hint during the night. No more. Too dim for bright days.
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


      rearLed.setPin(ledRearPin);
      rearLed.offIntensity =    rearLedOffIntensity;
      rearLed.lowIntensity =    rearLedLowIntensity;
      rearLed.mediumIntensity = rearLedLowIntensity;
      rearLed.highIntensity =   rearLedLowIntensity;
      rearLed.maxIntensity =    rearLedLowIntensity;
      rearLed.setLedIntensity (min(rearLed.getLedIntensity(), rearLedMediumIntensity));

      statusBattery = BATTERY_RED;
    }
    if (batteryPercentage_pct < Batt_pct_limits[2]  && statusBattery == BATTERY_RED)
    {
      /*
         The battery is almost dead. We now power it down.
         No more fun. Only option: switch off and on again.
      */
      statusPowerDown =  true;

      detachInterrupt(digitalPinToInterrupt(switchSpdPin)); // 0 = interupt on pin 2
      detachInterrupt(digitalPinToInterrupt(switchCadPin)); // 0 = interupt on pin 2

      // switch off all lichts.
      leftLed.setLedIntensity(0);
      rightLed.setLedIntensity(0);
      rearLed.setLedIntensity(0);
      headLed.setLedIntensity(0);
      auxLed.setLedIntensity(0);

  

      u8g.sleepOn();// Power down the display

      // Turn off everything eccept the Arduino itself.
      digitalWrite(powerOnOffPin, 0);

      
      // Powerdown the Arduino. Note that it only is to be revived by power cycling or the reset button.
      set_sleep_mode(SLEEP_MODE_PWR_DOWN);
      sleep_enable();
      sleep_mode(); //sleep until ever because the interupts are disabled. The only way to boot is to reconnect the battery.

    }
  }
}


/**********************************************************************************************

   Functions which update the states and .

 **********************************************************************************************/

/*********************************************************************************************************

    Determine if we need to go to sleep or not.

 *********************************************************************************************************/
void updateSleep ()
{
  long tLastStateChange_ms = 0;
  long tNow_ms             = millis();
  long tWait_ms            = tSleepNoCadSpd_ms;

  tLastStateChange_ms = max(upSwitch.getTimeLastChange_ms(), downSwitch.getTimeLastChange_ms());
  tLastStateChange_ms = max(tLastStateChange_ms, leftSwitch.getTimeLastChange_ms());
  tLastStateChange_ms = max(tLastStateChange_ms, rightSwitch.getTimeLastChange_ms());
  tLastStateChange_ms = max(tLastStateChange_ms, brakeSwitch.getTimeLastChange_ms());
  tLastStateChange_ms = max(tLastStateChange_ms, cadenceSwitch.getTimeLastChange_ms());
  tLastStateChange_ms = max(tLastStateChange_ms, speedSwitch.getTimeLastChange_ms());


  /*
    TODO:
    - Disable interupts when going to sleep
    - do not go to sleep while in alarm
    - Go to sleep after longer time when the cad and speed have not been set...
  */

  if (cadenceSwitch.getInteruptActive() || speedSwitch.getInteruptActive()) tWait_ms = tSleep_ms;

  //Serial.println ("Wait1 : " + String(tWait_ms));
  //Serial.println ("Wait2 : " + String(cadenceSwitch.getTimeLastChange_ms())) ;
  //Serial.println ("Wait3 : " + String(speedSwitch.getTimeLastChange_ms())) ;

  if ((tNow_ms - tLastStateChange_ms > tWait_ms) && (stateAlarmBlinkersOn == false))
  {
    // This semaphore disables interupts from updating the leds.
    statusPowerDown = true;

    // clear the screen
    u8g.sleepOn();

    // switch off all lichts.
    leftLed.setLedIntensity(0);
    rightLed.setLedIntensity(0);
    rearLed.setLedIntensity(0);
    headLed.setLedIntensity(0);
    auxLed.setLedIntensity(0);

    //
    // Sleep state is low power sleep. The Arduino only reactis to the hardware interupts (cad en spd).
    // This is -appart from power down- the only way to revive the arduino from it's sleep.
    // Only when it gets an hardware interupt the aduino wakes again and willl continue to function as normal.
    // Until that moment it is "trapped" in sleep_mode().
    //
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
    sleep_enable();
    sleep_mode(); //sleep until interupt

    // waking up again: reset all
    sleep_disable();

    // Set the leds to the setting at power on.
    u8g.sleepOff();
    leftLed.setLedOff();
    rightLed.setLedOff();
    rearLed.setLedLow();
    headLed.setLedLow();
    auxLed.setLedLow();

    statusPowerDown = false;
  }
}


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
  int PinValue = analogRead(voltagePin);
  static int PinMean = PinValue; // it is inutialised at the fist value and then kept in memory.

  const byte numSamples = 20;
  const float VRef = 5.00;

  int batteryVoltage_mv;
  // is a global variable, do not declare here.
  // int cellVoltage_mv;
  byte numOfCells;

  PinMean = (PinMean * (numSamples - 1) + PinValue) / numSamples; // the mean voltage on the pin.

  batteryVoltage_mv = 4326 * (VRef / 1023) * PinMean; // 4.326 was measured useing a voltage meter

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
  
  //numOfCells = 1;

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
  */
  if (doBatteryCheck) {
    const int Batt_pct_limits[3] = {20, 10, 5};
    if (batteryPercentage_pct < Batt_pct_limits[0] && statusBattery == BATTERY_GREEN && tNow_ms > 10000)
    {
      /*
         No high beam
      */
      statusBattery = BATTERY_ORANGE;

      headLed.highIntensity = headLedMediumIntensity;
      headLed.maxIntensity = headLedMediumIntensity ;
      headLed.setLedIntensity (min(headLed.getLedIntensity(), headLedMediumIntensity));

      auxLed.highIntensity = auxLedMediumIntensity;
      auxLed.maxIntensity = auxLedMediumIntensity;
      auxLed.setLedIntensity (min(auxLed.getLedIntensity(), auxLedMediumIntensity));


    }
    if (batteryPercentage_pct < Batt_pct_limits[1]  && statusBattery == BATTERY_ORANGE)
    {
      /*
         We are now more pressen for power consumption.
         Dim blinkers, > just a little hint during the night. No more. Too dim for bright days.
         No brakelight
         No fog light

      */
      rightLed.offIntensity = 0;
      rightLed.lowIntensity = 0;
      rightLed.mediumIntensity = 0;
      rightLed.highIntensity = 64; // 1/2th the power
      rightLed.maxIntensity = 64;
      rightLed.setLedIntensity (min(rightLed.getLedIntensity(), rightLedMediumIntensity));


      leftLed.offIntensity = 0;
      leftLed.lowIntensity = 0;
      leftLed.mediumIntensity = 0;
      leftLed.highIntensity = 64;
      leftLed.maxIntensity = 64;
      leftLed.setLedIntensity (min(leftLed.getLedIntensity(), leftLedMediumIntensity));

      rearLed.setPin(ledRearPin);
      rearLed.offIntensity = rearLedOffIntensity;
      rearLed.lowIntensity = rearLedLowIntensity;
      rearLed.mediumIntensity = rearLedLowIntensity;
      rearLed.highIntensity = rearLedLowIntensity;
      rearLed.maxIntensity = rearLedLowIntensity;
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

      // Powerdown the Arduino. Note that it only is to be revived by power cycling or the reset button.
      set_sleep_mode(SLEEP_MODE_PWR_DOWN);
      sleep_enable();
      sleep_mode(); //sleep until ever because the interupts are disabled.

    }
  }
}

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
#if defined(QUILTJE)  || defined(STRADA)
    auxLed.upLed();
#endif
  }

  if (downSwitch.getState() == LOW && downSwitch.hasStateChanged() && brakeSwitch.getState() == HIGH && configSwitch.getState() == HIGH)
  {
    headLed.downLed();

    //
    // Only the strada and Quiltje have double headlights.
    //
#if defined(QUILTJE)  || defined(STRADA)
    auxLed.downLed();
#endif

  }
}



/*********************************************************************************************************

    Update the blinkers

 *********************************************************************************************************/
void updateBlinkers()
{
  /*
     A few things can happen:
     - Do not blink
     - Blink right
     - Blink left
     - Blink both (ALARM lights)

  */

  static byte numTimesToBlinkLeft = 0;
  static byte numTimesToBlinkRight = 0;
  static long tStartBlink_ms = 0;
  long tNow_ms = millis();
  byte BlinkOn = 0;

  leftSwitch.ReadOut();
  rightSwitch.ReadOut();
  alarmSwitch.ReadOut();

  //#if defined(QUATRO) || defined(STRADA)
  if (alarmSwitch.hasStateChanged() && alarmSwitch.getState() == LOW )
  {
    stateAlarmBlinkersOn = !stateAlarmBlinkersOn;

    if (stateAlarmBlinkersOn == false)
    {
      numTimesToBlinkLeft = 0;
      numTimesToBlinkRight = 0;
      // Just in case.... otherwise they might stay on.
      rightLed.setLedOff();
      leftLed.setLedOff();
    }
  }
  //#endif

  if (stateAlarmBlinkersOn)
  { // We are blinking alarm lights.
    numTimesToBlinkLeft = 1;
    numTimesToBlinkRight = 1;
  }
  else
  { // regular stuff to do.
    if (rightSwitch.getState() == LOW )
    { // blink right
      if (rightSwitch.hasStateChanged())
      {
        numTimesToBlinkRight = numTimesToBlink * 2;
        tStartBlink_ms = 0;// reset the timer in case we switched quickely from left to right
      }
      else
      {
        numTimesToBlinkRight = max(numTimesToBlinkRight, 1);
      }
      numTimesToBlinkLeft = 0;

    }
    else if (leftSwitch.getState() == LOW )
    { // blink left
      if (leftSwitch.hasStateChanged())
      {
        numTimesToBlinkLeft = numTimesToBlink * 2;
        tStartBlink_ms = 0; // reset the timer in case we switched quickely from left to right
      }
      else
      {
        numTimesToBlinkLeft = max(numTimesToBlinkLeft, 1);
      }
      numTimesToBlinkRight = 0;
    }
  }

  /*
     Do the actual blinking
  */

  //
  // See if enough time has passed since the last change of the leds.
  //
  if ((tNow_ms - tStartBlink_ms) >= tPeriodBlink_ms)
  {

    // Zoomer ands screen blinking in sync with the actual leds. Don't keep track.
    BlinkOn = (rightLed.getLedIntensity() > 0 || leftLed.getLedIntensity() > 0);

    tStartBlink_ms = tNow_ms;

    // blink left
    if (numTimesToBlinkLeft > 0 && numTimesToBlinkRight == 0)
    {
      numTimesToBlinkLeft--;

      if (leftLed.getLedIntensity() == leftLed.offIntensity) leftLed.setLedMax();
      else leftLed.setLedOff();

      rightLed.setLedOff();


    }
    // blink right
    else if (numTimesToBlinkRight > 0 && numTimesToBlinkLeft == 0)
    {
      numTimesToBlinkRight--;

      if (rightLed.getLedIntensity() == rightLed.offIntensity) rightLed.setLedMax();
      else rightLed.setLedOff();

      leftLed.setLedOff();
    }
    // blink alarm
    else if (numTimesToBlinkRight > 0 && numTimesToBlinkLeft > 0)
    { // alarm, make sure they are in sync
      if (rightLed.getLedIntensity() == rightLed.offIntensity)
      {
        rightLed.setLedMax();
        leftLed.setLedMax();
      }
      else
      {
        rightLed.setLedOff();
        leftLed.setLedOff();
      }
    }
    else if (numTimesToBlinkRight == 0 && numTimesToBlinkLeft == 0 && BlinkOn)
    {
      // Apearently the leds are still on while the blinker switch is in the off setting and we no longer need to do anything
      rightLed.setLedOff();
      leftLed.setLedOff();
    }

    // blink the screen
    BlinkOn = (rightLed.getLedIntensity() > 0 || leftLed.getLedIntensity() > 0);

    if (BlinkOn)
    {
      u8g.begin(); // Make sure the display gets reset when we blink
      u8g.setContrast(setOledIntensity);
      u8g.sleepOn();
    }
    else u8g.sleepOff();

    // zoomer
    if (stateAlarmBlinkersOn == false)
    {
      analogWrite(speakerPin, BlinkOn * speakerVolume);
    }
    else
    {
      // set to zero during Alarm.
      analogWrite(speakerPin, 0);
    }
  }

}


/*********************************************************************************************************

    Update the rearlight

*********************************************************************************************************

  The rear lights are normally at a nominal intensity.
  There are four intensities at which the rear lights are run:
  1) low intensity when riding convoy.
  2) default intensity
  3) higher intensity when riding in fog or other situations in which higer visibility is needed.
  4) brakelight: max intensity  when brakeing. Note that this is independant of the brakelight itself.

*/

void updateRear()
{
  /*
    First we determine how the state changed
    Second we adapt the led settings.

    Note that we keep track of the return state when the brake is released here here.
  */
  static byte ledPreviousIntensity = rearLedOffIntensity;
  long tNow_ms = millis();

  brakeSwitch.ReadOut();

  //
  // only brakeswitch is on, and changed -> turn on brake light to Max.
  //
  if (brakeSwitch.getState() == LOW 
      && brakeSwitch.hasStateChanged() 
      && upSwitch.getState() == HIGH 
      && downSwitch.getState() == HIGH)
  {
    ledPreviousIntensity = rearLed.getLedIntensity();
    rearLed.setLedMax();
#if defined(QUATRO) || defined(ICB_DF)
    auxLed.setLedMax();
#endif
    //DEBUG_CV = DEBUG_CV - 3;
  }
  //
  // When the brakeswitch is applied too long switch off the breaklights.
  //
  else if (brakeSwitch.getState() == LOW 
           && !brakeSwitch.hasStateChanged() 
           && tNow_ms - brakeSwitch.getTimeLastChange_ms() > tDurationBrakeLight_ms 
           && rearLed.getLedIntensity() == rearLed.maxIntensity)
  {
      rearLed.setLedIntensity(ledPreviousIntensity);
#if defined(QUATRO)
      auxLed.setLedOff();
#endif
  }

/* Don't understand this one */
/*
  //
  // only brakeswitch is on, and has been for a while and we push up or down
  //
  else if (brakeSwitch.getState() == LOW 
           && !brakeSwitch.hasStateChanged() 
           && upSwitch.getState() == HIGH 
           && downSwitch.getState() == HIGH)
  {
    ledPreviousIntensity = rearLed.getLedIntensity();
    rearLed.setLedMax();
#if defined(QUATRO) || defined(ICB_DF)
    auxLed.setLedMax();
#endif
    //DEBUG_CV = DEBUG_CV - 3;
  }
*/
  //
  // brakeswitch is off -> return to previous state. Only when the brakeled is max.
  //
  else if (brakeSwitch.getState() == HIGH 
           && brakeSwitch.hasStateChanged() 
           && rearLed.getLedIntensity() == rearLed.maxIntensity)
  {
    rearLed.setLedIntensity(ledPreviousIntensity);
#if defined(QUATRO) || defined(ICB_DF)
    auxLed.setLedOff();
#endif
  }

  //
  // Brakeswitch is on, upswitch has changed- > default goes up as fog light
  //
  else if (brakeSwitch.getState() == LOW 
           && upSwitch.getState() == LOW 
           && upSwitch.hasStateChanged())
  {
    rearLed.setLedIntensity(ledPreviousIntensity);
    rearLed.upLed();
    ledPreviousIntensity = rearLed.getLedIntensity();
#if defined(QUATRO) || defined(ICB_DF)
    auxLed.setLedOff();
#endif
  }

  //
  // Brakeswicht is on, downswitch has changed -> turn off fog light or go to dim
  //
  else if (brakeSwitch.getState() == LOW 
           && downSwitch.getState() == LOW 
           && downSwitch.hasStateChanged())
  {
    rearLed.setLedIntensity(ledPreviousIntensity);
    rearLed.downLed();
    ledPreviousIntensity = rearLed.getLedIntensity();
#if defined(QUATRO) || defined(ICB_DF)
    auxLed.setLedOff();
#endif
  }
}


/*********************************************************************************************************

    The configuration interface

 *********************************************************************************************************/
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
  //
  // Teeth Rear =  Teeth chain ring * Cadence (Hz) /  axle Speed (Hz)
  // axle speed = front axle speed * (front wheel circumference / read wheel circumference)
  //
  float gearOnCassette = float(setTeethOnCainring) * gearOnCassette_scaling * cadenceSwitch.getInteruptFrequency(1500) / speedSwitch.getInteruptFrequency(1500);

  //
  // Find the minimum
  //
  float minError = 999.9;
  float currentError = 999.9;

  // If the calculated gear is smaller than 2/3 of the smallest (first) gear on the cassette, then we're freewheeling
  if (2 * gearOnCassette < setTeethOnCassette[0]) {
    gearOnCassette_teeth = 1;
    return;
  }

  int iEnd = sizeof (setTeethOnCassette) / sizeof (setTeethOnCassette[0]);
  for (byte i = 0; i < iEnd; i++)
  {
    currentError  = fabs(gearOnCassette - setTeethOnCassette[i]);

    if (cadenceSwitch.getInteruptFrequency(1500) > 0 && speedSwitch.getInteruptFrequency(1500) > 0) 
    {
      int iEnd = sizeof (setTeethOnCassette) / sizeof (setTeethOnCassette[0]);
      for (byte i = 0; i < iEnd; i++)
      {
        currentError  = fabs(gearOnCassette - setTeethOnCassette[i]);

        if (currentError < minError)
        {
          minError = currentError;
          gearOnCassette_index = iEnd - i;
          gearOnCassette_teeth = setTeethOnCassette[i];
        }
      }
    }
    else
    { gearOnCassette_teeth = 1;
      return;
    }
  }
}


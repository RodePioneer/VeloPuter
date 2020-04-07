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
    - do not go to sleep while in alarm
    - Go to sleep after longer time when the cad and speed have not been set...
  */

  if (cadenceSwitch.getInteruptActive() || speedSwitch.getInteruptActive()) tWait_ms = tSleep_ms;

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

    // turn off power
     digitalWrite(powerOnOffPin, 0);


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


    // turn on power
    digitalWrite(powerOnOffPin, 1);
       
    // Set the leds to the setting at power on.
    leftLed.setLedOff();
    rightLed.setLedOff();
    rearLed.setLedLow();
    headLed.setLedLow();
    auxLed.setLedOff();

    brakeSwitch.ReadOut();
    upSwitch.ReadOut();
    downSwitch.ReadOut();

    // turn on the display. Note that because we cut power to the display it how needs some time to boot up. Show the spashscreen.
    delay (100); 
    drawSplash ();      
    //u8g.sleepOff();
    //u8g.begin();
    //u8g.setContrast(0);


    statusPowerDown = false;
  }
}

/*********************************************************************************************************

   Determine if we need to go to sleep or not.

 *********************************************************************************************************/
void updateSleep ()
{
  long tLastStateChange_ms = millis();
  long tNow_ms             = millis();
  long tSleep_ms           = long(tSleepNoCadSpd_min) * 1000 * 60;

  tLastStateChange_ms = max(upSwitch.getTimeLastChange_ms(), downSwitch.getTimeLastChange_ms());
  tLastStateChange_ms = max(tLastStateChange_ms, leftSwitch.getTimeLastChange_ms());
  tLastStateChange_ms = max(tLastStateChange_ms, rightSwitch.getTimeLastChange_ms());
  tLastStateChange_ms = max(tLastStateChange_ms, brakeSwitch.getTimeLastChange_ms());
  tLastStateChange_ms = max(tLastStateChange_ms, cadenceSwitch.getTimeLastChange_ms());
  tLastStateChange_ms = max(tLastStateChange_ms, speedSwitch.getTimeLastChange_ms());

  // We detected a cadence or a speed sensor. Reduce the time until sleep.
  if (cadenceSwitch.getInteruptActive() || speedSwitch.getInteruptActive()) tSleep_ms = long(tSleepCadSpd_min) * 1000 * 60;

  // When the time has passed and the alarm blinkers are NOT on go to sleep.
  if ((tNow_ms - tLastStateChange_ms > tSleep_ms) && (stateAlarmBlinkersOn == false))
  {
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

    // Make sure we do not dive into sleep right away again
    //tLastStateChange_ms = millis();


    // turn on power
    digitalWrite(powerOnOffPin, 1);

    // Set the leds to the setting at power on.
    leftLed.setLedOff();
    rightLed.setLedOff();
    rearLed.setLedLow();
    headLed.setLedLow();
    auxLed.setLedOff();

    //brakeSwitch.ReadOut();
    //upSwitch.ReadOut();
    //downSwitch.ReadOut();
    //speedSwitch.ReadOut();
    //cadenceSwitch.ReadOut();

    // turn on the display. Note that because we cut power to the display it how needs some time to boot up. Show the spashscreen.
    delay (100);
    drawSplash ();
    //u8g.sleepOff();
    //u8g.begin();
    //u8g.setContrast(0);


    // wakup: just run the setup
    //setup();

    // statusPowerDown = false;
  }
}

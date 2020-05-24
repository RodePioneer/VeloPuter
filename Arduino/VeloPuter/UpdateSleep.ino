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
  tLastStateChange_ms = max(tLastStateChange_ms, alarmSwitch.getTimeLastChange_ms());
  tLastStateChange_ms = max(tLastStateChange_ms, configSwitch.getTimeLastChange_ms());
  tLastStateChange_ms = max(tLastStateChange_ms, cadenceSwitch.getTimeLastChange_ms());
  tLastStateChange_ms = max(tLastStateChange_ms, speedSwitch.getTimeLastChange_ms());

  // We detected a cadence or a speed sensor. Reduce the time until sleep.
  if (cadenceSwitch.getInteruptActive() || speedSwitch.getInteruptActive())
  {
    tSleep_ms = long(tSleepCadSpd_min) * 1000 * 60;
  }

  // When the time has passed and the alarm blinkers are NOT on go to sleep.
  if ((tNow_ms - tLastStateChange_ms > tSleep_ms) && (stateAlarmBlinkersOn == false))
  {
    // Sleep untill we detect an interupt.
    sleepNow ();

    // Disable the processor sleep
    sleep_disable();

    // Go thourgh the same bootup as on power on.
    wakeUp ();
  }
}

void sleepNow ()
{
  /*
    Turn off the display
  */
  u8g.sleepOn();

  /*
    Turn off all Lights
  */
  headLed.setLedBlack();
  rearLed.setLedBlack();
  brakeLed.setLedBlack();
  leftLed.setLedBlack();
  rightLed.setLedBlack();
  auxLed.setLedBlack();
  
  /*
    Turn off the buzzer
  */
  analogWrite(speakerPin, 0);

  /*
    Additionaly turn off power to all part except the Arduino itself.
  */
  digitalWrite(powerOnOffPin, 0);

  /*
    Sleep state is low power sleep. The Arduino only reactis to the hardware interupts (cad en spd).
    This is -appart from power down- the only way to revive the arduino from it's sleep.
    Only when it gets an hardware interupt the aduino wakes again and willl continue to function as normal.
    Until that moment it is "trapped" in sleep_mode().
  */
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_enable();

  /*
    Sleep until we detect an interupt (cadence of speed sensor)
    Note: this may never be if the interupts are disabled because of a low power sleep
  */
  sleep_mode();
}

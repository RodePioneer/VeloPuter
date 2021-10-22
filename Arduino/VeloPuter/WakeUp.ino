

  void wakeUp() {

  // Turn on the power to the display, the 5Vout and all the powersources
  pinMode(powerOnOffPin, OUTPUT);
  digitalWrite(powerOnOffPin, 1);


  // setup display
  u8g.setColorIndex(1); // Instructs the display to draw with a pixel on.
  u8g.setContrast(setOledIntensity);


  /**********************************************************************************************
     Init: after return from sleep or at bootup.
   **********************************************************************************************/
  // Inputs: switches and the voltage measurement
  pinMode(switchLeftPin, INPUT_PULLUP);
  leftSwitch.setPinID(switchLeftPin);

  pinMode(switchRightPin, INPUT_PULLUP);
  rightSwitch.setPinID(switchRightPin);

  pinMode(switchHeadUpPin, INPUT_PULLUP);
  upSwitch.setPinID(switchHeadUpPin);

  pinMode(switchHeadDownPin, INPUT_PULLUP);
  downSwitch.setPinID(switchHeadDownPin);

  pinMode(switchBrakePin, INPUT_PULLUP);
  brakeSwitch.setPinID(switchBrakePin);

  pinMode(switchAlarmPin, INPUT_PULLUP);
  alarmSwitch.setPinID(switchAlarmPin);

  pinMode(switchConfigPin, INPUT_PULLUP);
  configSwitch.setPinID(switchConfigPin);


  pinMode(switchSpdPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(switchSpdPin), interruptServiceRoutineSpeed, FALLING); // 0 = interupt on pin 2

  pinMode(switchCadPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(switchCadPin), interruptServiceRoutineCadence, FALLING); // 1 = interupt on pin 3

  pinMode(voltagePin, INPUT);
  analogReference(DEFAULT);  // use the 5V pin as ref
  myBattery.setPinID(voltagePin);

  // Setup all the outputs (5 LED in total and a speaker)

  // Indicator: left
  pinMode(ledLeftPin, OUTPUT);
  leftLed.setPinID(ledLeftPin);
  memcpy (leftLed.setIntensities, leftLedIntensities, sizeof(leftLedIntensities));
//  leftLed.IMax();
  leftLed.setLedOff();

  // Indicator: right
  pinMode(ledRightPin, OUTPUT);
  rightLed.setPinID(ledRightPin);
  memcpy (rightLed.setIntensities, rightLedIntensities,sizeof(rightLedIntensities));
//  rightLed.IMax();
  rightLed.setLedOff();

  // Rear light
  pinMode(ledRearPin, OUTPUT);
  rearLed.setPinID(ledRearPin);
  memcpy (rearLed.setIntensities, rearLedIntensities,sizeof(rearLedIntensities));
//  rearLed.IMax();
  rearLed.setLedLow();

  // Head light
  pinMode(ledHeadPin, OUTPUT);
  headLed.setPinID(ledHeadPin);
  memcpy (headLed.setIntensities, headLedIntensities,sizeof(headLedIntensities));
//  headLed.IMax();
  headLed.setLedLow();

  // Brake light (optional)
  pinMode(ledBrakePin, OUTPUT);
  brakeLed.setPinID(ledBrakePin);
  memcpy (brakeLed.setIntensities, brakeLedIntensities,sizeof(brakeLedIntensities));
//  brakeLed.IMax();
  brakeLed.setLedOff();

  // Extra free potition. 
  pinMode(ledAux2Pin, OUTPUT);
  auxLed.setPinID(ledAux2Pin);
//  auxLed.IMax();
  auxLed.setLedOff();

  // Speaker
  pinMode(speakerPin, OUTPUT);

  // Set the battery type
  myBattery.setType(setBatteryType);


  // Start display
  delay (100); 
  drawSplash ();
}

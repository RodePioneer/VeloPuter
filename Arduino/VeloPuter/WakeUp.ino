

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

  // Setup all the outputs (5 LED in total and a speaker)
  pinMode(ledLeftPin, OUTPUT);
  leftLed.setPinID(ledLeftPin);
  leftLed.offIntensity = leftLedOffIntensity;
  leftLed.lowIntensity = leftLedLowIntensity;
  leftLed.mediumIntensity = leftLedMediumIntensity;
  leftLed.highIntensity = leftLedHighIntensity;
  leftLed.maxIntensity = leftLedMaxIntensity;
  leftLed.setLedOff();

  pinMode(ledRightPin, OUTPUT);
  rightLed.setPinID(ledRightPin);
  rightLed.offIntensity = rightLedOffIntensity;
  rightLed.lowIntensity = rightLedLowIntensity;
  rightLed.mediumIntensity = rightLedMediumIntensity;
  rightLed.highIntensity = rightLedHighIntensity;
  rightLed.maxIntensity = rightLedMaxIntensity;
  rightLed.setLedOff();

  pinMode(ledRearPin, OUTPUT);
  rearLed.setPinID(ledRearPin);
  rearLed.offIntensity = rearLedOffIntensity;
  rearLed.lowIntensity = rearLedLowIntensity;
  rearLed.mediumIntensity = rearLedMediumIntensity;
  rearLed.highIntensity = rearLedHighIntensity;
  rearLed.maxIntensity = rearLedMaxIntensity;
  rearLed.setLedLow();

  pinMode(ledHeadPin, OUTPUT);
  headLed.setPinID(ledHeadPin);
  headLed.offIntensity = headLedOffIntensity;
  headLed.lowIntensity = headLedLowIntensity;
  headLed.mediumIntensity = headLedMediumIntensity;
  headLed.highIntensity = headLedHighIntensity;
  headLed.maxIntensity = headLedMaxIntensity ;
  headLed.setLedLow();

  pinMode(ledAuxPin, OUTPUT);
  auxLed.setPinID(ledAuxPin);
  auxLed.offIntensity = auxLedOffIntensity;
  auxLed.lowIntensity = auxLedLowIntensity;
  auxLed.mediumIntensity = auxLedMediumIntensity;
  auxLed.highIntensity = auxLedHighIntensity;
  auxLed.maxIntensity = auxLedMaxIntensity;
  auxLed.setLedOff();

  pinMode(ledAux2Pin, OUTPUT);
  aux2Led.setPinID(ledAux2Pin);
  aux2Led.offIntensity = aux2LedOffIntensity;
  aux2Led.lowIntensity = aux2LedLowIntensity;
  aux2Led.mediumIntensity = aux2LedMediumIntensity;
  aux2Led.highIntensity = aux2LedHighIntensity;
  aux2Led.maxIntensity = aux2LedMaxIntensity;
  aux2Led.setLedOff();


  pinMode(speakerPin, OUTPUT);

  // Start display
  delay (100); 
  drawSplash ();
}

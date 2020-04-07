
void setup()   {

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
  leftSwitch.setPin(switchLeftPin);

  pinMode(switchRightPin, INPUT_PULLUP);
  rightSwitch.setPin(switchRightPin);

  pinMode(switchHeadUpPin, INPUT_PULLUP);
  upSwitch.setPin(switchHeadUpPin);

  pinMode(switchHeadDownPin, INPUT_PULLUP);
  downSwitch.setPin(switchHeadDownPin);

  pinMode(switchBrakePin, INPUT_PULLUP);
  brakeSwitch.setPin(switchBrakePin);

  pinMode(switchAlarmPin, INPUT_PULLUP);
  alarmSwitch.setPin(switchAlarmPin);

  pinMode(switchConfigPin, INPUT_PULLUP);
  configSwitch.setPin(switchConfigPin);


  pinMode(switchSpdPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(switchSpdPin), interruptServiceRoutineSpeed, FALLING); // 0 = interupt on pin 2

  pinMode(switchCadPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(switchCadPin), interruptServiceRoutineCadence, FALLING); // 1 = interupt on pin 3

  pinMode(voltagePin, INPUT);
  analogReference(DEFAULT);  // use the 5V pin as ref

  // Setup all the outputs (5 LED in total and a speaker)
  pinMode(ledLeftPin, OUTPUT);
  leftLed.setPin(ledLeftPin);
  leftLed.offIntensity = leftLedOffIntensity;
  leftLed.lowIntensity = leftLedLowIntensity;
  leftLed.mediumIntensity = leftLedMediumIntensity;
  leftLed.highIntensity = leftLedHighIntensity;
  leftLed.maxIntensity = leftLedMaxIntensity;
  leftLed.setLedOff();

  pinMode(ledRightPin, OUTPUT);
  rightLed.setPin(ledRightPin);
  rightLed.offIntensity = rightLedOffIntensity;
  rightLed.lowIntensity = rightLedLowIntensity;
  rightLed.mediumIntensity = rightLedMediumIntensity;
  rightLed.highIntensity = rightLedHighIntensity;
  rightLed.maxIntensity = rightLedMaxIntensity;
  rightLed.setLedOff();

  pinMode(ledRearPin, OUTPUT);
  rearLed.setPin(ledRearPin);
  rearLed.offIntensity = rearLedOffIntensity;
  rearLed.lowIntensity = rearLedLowIntensity;
  rearLed.mediumIntensity = rearLedMediumIntensity;
  rearLed.highIntensity = rearLedHighIntensity;
  rearLed.maxIntensity = rearLedMaxIntensity;
  rearLed.setLedLow();

  pinMode(ledHeadPin, OUTPUT);
  headLed.setPin(ledHeadPin);
  headLed.offIntensity = headLedOffIntensity;
  headLed.lowIntensity = headLedLowIntensity;
  headLed.mediumIntensity = headLedMediumIntensity;
  headLed.highIntensity = headLedHighIntensity;
  headLed.maxIntensity = headLedMaxIntensity ;
  headLed.setLedLow();

  pinMode(ledAuxPin, OUTPUT);
  auxLed.setPin(ledAuxPin);
  auxLed.offIntensity = auxLedOffIntensity;
  auxLed.lowIntensity = auxLedLowIntensity;
  auxLed.mediumIntensity = auxLedMediumIntensity;
  auxLed.highIntensity = auxLedHighIntensity;
  auxLed.maxIntensity = auxLedMaxIntensity;
#if defined(QUILTJE)  || defined(STRADA)
  auxLed.setLedLow();
#elif defined(QUATRO) || defined(ICB_DF)
  auxLed.setLedOff();
#endif

pinMode(ledAux2Pin, OUTPUT);
  aux2Led.setPin(ledAux2Pin);
  aux2Led.offIntensity = aux2LedOffIntensity;
  aux2Led.lowIntensity = aux2LedLowIntensity;
  aux2Led.mediumIntensity = aux2LedMediumIntensity;
  aux2Led.highIntensity = aux2LedHighIntensity;
  aux2Led.maxIntensity = aux2LedMaxIntensity;
  aux2Led.setLedLow();


  pinMode(speakerPin, OUTPUT);

  // Start display
  delay (100); 
  drawSplash ();
}

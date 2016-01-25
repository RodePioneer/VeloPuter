
/**********************************************************************************************
 *
 * Functions which manage the OLED display.
 *
 **********************************************************************************************/


void writeScreen ()
{

  
  int tDelay_ms = 250; // 4 time per second
  static long tNew = 0;
  static long tOld = 0;
  tNew = millis();

  if ((tNew - tOld) >= tDelay_ms)
  {
    // For display purposes only
    cadence_rpm++;
    speed_kmh++;

    // display
    u8g.firstPage();
    do {  
      drawBattery(batteryStatus);
      drawLightIcons();
      drawSpeed();
      drawCadence();
    } while( u8g.nextPage() );

    // reset the timer
    tOld = tNew;
  }
}

// To clear the screen we only need an empty picture loop.
void clearScreen ()
{
    u8g.firstPage();
    do {  
      drawClear ();
    } while( u8g.nextPage() );
}

void drawClear ()
{
}

void blinkScreen (byte doBlink)
{
    if (doBlink)
    {
      u8g.firstPage();
      do {  
      drawBlink ();      
      } while( u8g.nextPage() );
    }
}

void drawBlink ()
{
  u8g.setColorIndex(1);
  u8g.drawBox (0, 0, 128, 64);
}

void drawSpeed()
{
  u8g.setColorIndex(1);
  u8g.setFont(u8g_font_helvR18n); //12,14,18
  u8g.setFontPosTop();
  u8g.setScale2x2();
  if (speed_kmh < 10)
    u8g.setPrintPos (39,0);
  else if (speed_kmh < 100)
    u8g.setPrintPos (27,0);
  else 
    u8g.setPrintPos (15,0);
  
  u8g.print (speed_kmh);
  u8g.undoScale();
}

void drawCadence()
{
 
  u8g.setColorIndex(1);
  u8g.setFont(u8g_font_helvR14n);
  u8g.setFontPosBottom();
  u8g.setPrintPos (99,64);
  u8g.print (cadence_rpm);
}

void drawBattery(int batteryStatus)
{/*
 Status is 0-100%
 */
 // batteryStatus = constrain(batteryStatus,0,99);

  const byte hBattery_px = 46;
  const byte wBattery_px = 17;
  const byte rBattery_px = 0;
  const byte cBattery_px = 0;
  const byte hKnob_px = 8;
  const byte lineWidth_px = 2;

  // the body of the battery
  byte hBody_px = hBattery_px - hKnob_px + lineWidth_px*2;
  byte cKnob_px = wBattery_px / 3 - 1+ cBattery_px;
  byte wKnob_px = wBattery_px - cKnob_px*2 - 1- cBattery_px;

  // The knob on top of the battery
  byte rKnob_px = rBattery_px;   
  byte rBody_px = hKnob_px - 2*lineWidth_px;
  
  // The bar in the battery
  byte hBar_px = (batteryStatus * (hBody_px-4 *lineWidth_px)) / 100 ;
  byte rBar_px = rBody_px + hBattery_px - hBar_px - 4*lineWidth_px;
  byte cBar_px = cBattery_px + 2*lineWidth_px;
  byte wBar_px = wBattery_px - 4*lineWidth_px;

  // draw the knob (non moving part)
  u8g.setColorIndex(1);
  u8g.drawRBox (cKnob_px, rKnob_px, wKnob_px, hKnob_px, 2);

  // draw the body (non moving part)
  u8g.setColorIndex(1);
  u8g.drawRBox (cBattery_px, rBody_px, wBattery_px,hBody_px,1);
  u8g.setColorIndex(0);
  u8g.drawRBox (cBattery_px+lineWidth_px, rBody_px+lineWidth_px, wBattery_px-2*lineWidth_px, hBody_px-2*lineWidth_px, 1);

  // draw the content/status (moving part)
  u8g.setColorIndex(128);
  if (hBar_px > 4) 
    u8g.drawRBox (cBar_px, rBar_px, wBar_px, hBar_px, 1);
  else if (hBar_px > 1)
    u8g.drawBox  (cBar_px, rBar_px, wBar_px, hBar_px);

  // The text underneath
  u8g.setColorIndex(1);
  u8g.setFont(u8g_font_helvR14n);
  u8g.setFontPosBottom();
  
  // the if statement makes that the end of the number is at a fixed position. 
  if (batteryStatus >= 10)
    u8g.setPrintPos (0,64);
  else
    u8g.setPrintPos (10,64);
  u8g.print (batteryStatus);

  // tmp: display the voltage under the batery
  u8g.drawBox(8, 62, 2, 2);
}

void drawLightIcons ()
{
  const byte r = 50;
  const byte c1 = 28+5;
  const byte c2 = 64;
  
  switch (headState.getState()) {
  case OFF:
    u8g.drawBitmapP (c1, r, 3, 14,  icoNone);
    break;  
  case LOW_INTENSITY:
    u8g.drawBitmapP (c1, r, 3, 14,  icoLowBeam);
    break;
  case DEFAULT_INTENSITY:
    u8g.drawBitmapP (c1, r, 3, 14,  icoDefaultBeam);
    break;
  case HIGH_INTENSITY:
    u8g.drawBitmapP (c1, r, 3, 14,  icoHighBeam);
    break;
  }

  switch (rearState.getState()) {
  case OFF:
    u8g.drawBitmapP (c2, r, 3, 14,  icoNone);
    break;  
  case LOW_INTENSITY:
    u8g.drawBitmapP (c2, r, 3, 14,  icoLowRear);
    break;
  case DEFAULT_INTENSITY:
    u8g.drawBitmapP (c2, r, 3, 14,  icoHighRear);
    break;
  case FOG_INTENSITY:
    u8g.drawBitmapP (c2, r, 3, 14, icoFogRear);
    break;
  case BRAKE_INTENSITY:
    u8g.drawBitmapP (c2, r, 3, 14, icoBrakeRear);
    break;
  }
}
//
//const uint8_t icoHighBeam[] U8G_PROGMEM = {
//B00000011, B11111100, B11111111,
//B00000111, B11111100, B11111111,
//B00011110, B00001110, B00000000,
//B00111000, B00000110, B00000000,
//B01110000, B00000110, B01111111,
//B01100000, B00000110, B01111111,
//B01100000, B00000110, B00000000,
//B01100000, B00000110, B00000000,
//B01100000, B00000110, B01111111,
//B01110000, B00000110, B01111111,
//B00111000, B00000110, B00000000,
//B00011110, B00001100, B00000000,
//B00001111, B11111100, B11111111,
//B00000011, B11111100, B11111111 };
//
//const uint8_t icoMaxBeam[] U8G_PROGMEM = {
//B00000011, B11111100, B11111111,
//B00000111, B11111100, B11111111,
//B00011110, B00001110, B00000000,
//B00111000, B00000110, B00000000,
//B01110000, B00000110, B01111111,
//B01100001, B11000110, B01111111,
//B01100011, B11000110, B00000000,
//B01100011, B11000110, B00000000,
//B01100001, B11000110, B01111111,
//B01110000, B00000110, B01111111,
//B00111000, B00000110, B00000000,
//B00011110, B00001100, B00000000,
//B00001111, B11111100, B11111111,
//B00000011, B11111100, B11111111 };
//
//const uint8_t icoHighBeam2[] U8G_PROGMEM = {
//B00000011, B11111100, B11111111,
//B00000111, B11111100, B11111111,
//B00011110, B00001110, B00000000,
//B00111000, B11100110, B00000000,
//B01110011, B10110110, B01111111,
//B01101110, B00010110, B01111111,
//B01101000, B11010110, B00000000,
//B01101000, B11010110, B00000000,
//B01101110, B00010110, B01111111,
//B01110011, B10110110, B01111111,
//B00111000, B11100110, B00000000,
//B00011110, B00001100, B00000000,
//B00001111, B11111100, B11111111,
//B00000011, B11111100, B11111111 };
//
///* LowBeam */
//const uint8_t icoDefaultBeam2[] U8G_PROGMEM = {
//B00000011, B11111100, B00000000,
//B00000111, B11111100, B00000000,
//B00011110, B00001110, B00000000,
//B00111000, B00000110, B01100000,
//B01110000, B00000110, B01111100,
//B01100000, B00000110, B00011111,
//B01100000, B00000110, B00000011,
//B01100000, B00000110, B00000000,
//B01100000, B00000110, B00000000,
//B01110000, B00000110, B01100000,
//B00111000, B00000110, B01111100,
//B00011110, B00001100, B00011111,
//B00001111, B11111100, B00000011,
//B00000011, B11111100, B00000000 }; 
//
///* DefaultBeam */
//const uint8_t icoDefaultBeam[] U8G_PROGMEM = {
//B00000011, B11111100, B00000000,
//B00000111, B11111100, B00000000,
//B00011110, B00001110, B01100000,
//B00111000, B00000110, B00011100,
//B01110000, B00000110, B00000011,
//B01100000, B00000110, B01100000,
//B01100000, B00000110, B00011100,
//B01100000, B00000110, B00000011,
//B01100000, B00000110, B01100000,
//B01110000, B00000110, B00011100,
//B00111000, B00000110, B00000011,
//B00011110, B00001100, B01100000,
//B00001111, B11111100, B00011100,
//B00000011, B11111100, B00000011 }; 
//
///* DaylightBeam */
//const uint8_t icoLowBeam[] U8G_PROGMEM = {
//B00000011, B11111100, B00000000,
//B00000111, B11111100, B00000000,
//B00011110, B00001110, B00000000,
//B00111000, B00000110, B00000000,
//B01110000, B00000110, B00000000,
//B01100000, B00000110, B00000000,
//B01100000, B00000110, B00000000,
//B01100000, B00000110, B00000000,
//B01100000, B00000110, B00000000,
//B01110000, B00000110, B00000000,
//B00111000, B00000110, B00000000,
//B00011110, B00001100, B00000000,
//B00001111, B11111100, B00000000,
//B00000011, B11111100, B00000000 }; 
//
///* None */
//const uint8_t icoNone[] U8G_PROGMEM = {
//B00000000, B00000000, B00000000,
//B00000000, B00000000, B00000000,
//B00000000, B00000000, B00000000,
//B00000000, B00000000, B00000000,
//B00000000, B00000000, B00000000,
//B00000000, B00000000, B00000000,
//B00000000, B00000000, B00000000,
//B00000000, B00000000, B00000000,
//B00000000, B00000000, B00000000,
//B00000000, B00000000, B00000000,
//B00000000, B00000000, B00000000,
//B00000000, B00000000, B00000000,
//B00000000, B00000000, B00000000,
//B00000000, B00000000, B00000000 }; 
//
///* DaylightBeam Rear */
//const uint8_t icoHighRear[] U8G_PROGMEM = {
//B00000000, B00000000, B00000000,
//B10000000, B00000000, B00000001,
//B11000000, B00000000, B00000011,
//B01100000, B00000000, B00000110,
//B00100011, B10000001, B11000100,
//B00000110, B11000011, B01100000,
//B11100110, B01100110, B01100111,
//B11100110, B01100110, B01100111,
//B00000110, B11000011, B01100000,
//B00100011, B10000001, B11000100,
//B01100000, B00000000, B00000110,
//B11000000, B00000000, B00000011,
//B10000000, B00000000, B00000001,
//B00000000, B00000000, B00000000 }; 
//
///* DaylightBeam */
//const uint8_t icoLowRear[] U8G_PROGMEM = {
//B00000000, B00000000, B00000000,
//B00000000, B00000000, B00000000,
//B00000000, B00000000, B00000000,
//B00000000, B00000000, B00000000,
//B00000011, B10000001, B11000000,
//B00000110, B11000011, B01100000,
//B00000110, B01100110, B01100000,
//B00000110, B01100110, B01100000,
//B00000110, B11000011, B01100000,
//B00000011, B10000001, B11000000,
//B00000000, B00000000, B00000000,
//B00000000, B00000000, B00000000,
//B00000000, B00000000, B00000000,
//B00000000, B00000000, B00000000 }; 
//
///* DaylightBeam */
//const uint8_t icoFogRear[] U8G_PROGMEM = {
//B00000000, B00000000, B00000000,
//B11000000, B00000000, B00000011,
//B01100000, B00000000, B00000110,
//B01100000, B00000000, B00000110,
//B00110011, B10000001, B11001100,
//B00110110, B11000011, B01101100,
//B01100110, B01100110, B01100110,
//B01100110, B01100110, B01100110,
//B11000110, B11000011, B01100011,
//B11000011, B10000001, B11000011,
//B01100000, B00000000, B00000110,
//B01100000, B00000000, B00000110,
//B00000000, B00000000, B00001100,
//B00000000, B00000000, B00000000 }; 
//
///* DaylightBeam */
//const uint8_t icoBrakeRear[] U8G_PROGMEM = {
//B00000000, B00111100, B00000000,
//B00011000, B11111111, B00011000,
//B00011001, B11100111, B10011000,
//B00110011, B10011001, B11001100,
//B00110011, B00011000, B11001100,
//B01100111, B00011000, B11100110,
//B01100110, B00011000, B01100110,
//B01100110, B00011000, B01100110,
//B01100111, B00000000, B11100110,
//B00110011, B00011000, B11001100,
//B00110011, B10011001, B11001100,
//B00011001, B11100111, B10011000,
//B00011000, B11111111, B00011000,
//B00000000, B00111100, B00000000 }; 


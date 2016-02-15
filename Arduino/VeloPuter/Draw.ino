
/**********************************************************************************************
 *
 * Functions which manage the OLED display.
 *
 **********************************************************************************************/

//
// ToDo: make this a proper name space and include the icons.
//

// A splash schreen
void drawSplash ()
{
    u8g.firstPage();
    do {  
      u8g.drawBitmapP (0, 0, 16, 64,  icoSplash);
    } while( u8g.nextPage() );

    delay (1500);

}

void writeScreen ()
{
 // int tDelay_ms = 250; // 4 time per second
//  static long tNew = 0;
//  static long tOld = 0;
//  tNew = millis();

//  if ((tNew - tOld) >= tDelay_ms)
 // {
    // display
    u8g.firstPage();
    do {  
      drawBatteryIcon();
      drawBatteryText();
      drawLightIcons();
      drawSpeed();
      drawCadence();
    } while( u8g.nextPage() );

  //  // reset the timer
    //tOld = tNew;
//  }
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
    u8g.setPrintPos (39-5,0);
  else if (speed_kmh < 100)
    u8g.setPrintPos (27-5,0);
  else 
    u8g.setPrintPos (15-5,0);
  
  u8g.print (speed_kmh);
  u8g.undoScale();
}

void drawCadence()
{
  u8g.setColorIndex(1);
  u8g.setFont(u8g_font_helvR14n);
  u8g.setFontPosBottom();

  if (cadence_rpm < 10)
    u8g.setPrintPos (118,48); // cad <10
  else if (cadence_rpm < 100)
    u8g.setPrintPos (109,48); // cad >= 10, <100
  else 
    u8g.setPrintPos (99,48); // cad >= 100
  u8g.print (cadence_rpm);
}

void drawBatteryText()
{
  
  // The text underneath the battery
  u8g.setColorIndex(1);
  u8g.setFont(u8g_font_helvR14n);
  u8g.setFontPosBottom();
  
  // the if statement makes that the end of the number is at a fixed position. 
  u8g.setPrintPos (0,64);   
  u8g.print (cellVoltage_v);
}

void drawBatteryIcon()
{/*
 Status is 0-100%
 */
  batteryStatus = constrain(batteryStatus,0,99);

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
}

void drawLightIcons ()
{
  const byte r = 50;
  const byte c1 = 28+32;
  const byte c2 = 128-24;

  //
  // The icon which indicated the status of the headlight.
  //
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

  //
  // The icon which indicated the status of the rearlight.
  //
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



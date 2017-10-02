
/**********************************************************************************************

   Functions which manage the OLED display.

 **********************************************************************************************/

//
// ToDo: make this a proper name space and include the icons.
//


void drawSplash ()
{
  /*
    // A splash schreen
  */
  u8g.begin();
  u8g.setContrast(setOledIntensity);
  u8g.firstPage();
  do {
    u8g.drawBitmapP (22, 24, 10, 10,  icoSplash);
    u8g.setContrast(setOledIntensity);
  } while ( u8g.nextPage() );
  delay (1000);
}

void drawScreen ()
{
  /*
    // This is the main function which makes sure all information is plotted in the OLED display.
  */
  drawBatteryText();
  drawLightIcons();
  drawSpeed();
  drawCadence();
  drawGear();
  drawBatteryIcon();
  //drawSensors();
  //drawDebug();

}

void blinkScreen (byte doBlink)
{
  /*
    // Blinking the screen when the blinkers are on.
  */
  if (doBlink)
  {
    u8g.firstPage();
    do {
      u8g.setColorIndex(1);
      u8g.drawBox (0, 0, 128, 64);
    } while ( u8g.nextPage() );
  }
}

void drawSpeed()
{
  /*
    // Display the speed. Note that we double the display scale in order to
    // get larger characters without the need for an other font to be loaded.
  */
  u8g.setColorIndex(1);
  u8g.setFont(u8g_font_helvR18r); //12,14,18

  u8g.setFontPosTop();
  u8g.setScale2x2();

  // The starting potion depends on the number of characters to display.
  if (speed_kmh < 10)
    u8g.setPrintPos (39 - 6, 0);
  else if (speed_kmh < 100)
    u8g.setPrintPos (27 - 6, 0);
  else
    u8g.setPrintPos (15 - 6, 0);

  u8g.print (speed_kmh);
  u8g.undoScale();
}

void drawCadence()
{
  /*
    // This draws the cadence
  */
  u8g.setColorIndex(1);
  u8g.setFont(u8g_font_helvR14r);
  u8g.setFontPosBottom();

  // The starting potion depends on the number of characters to display.
  if (cadence_rpm < 10)
  {
    u8g.setPrintPos (118, 49); // cad <10
  }
  else if (cadence_rpm < 100)
  {
    u8g.setPrintPos (109, 49); // cad >= 10, <100
  }
  else
  {
    u8g.setPrintPos (99, 49); // cad >= 100
  }
  u8g.print (cadence_rpm);
}

void drawBatteryText()
{
  /*
    // The text underneath the battery
  */
  u8g.setColorIndex(1);
  u8g.setFont(u8g_font_helvR14r);
  u8g.setFontPosBottom();

  // the if statement makes that the end of the number is at a fixed position.
  u8g.setPrintPos (0, 68);
  u8g.print (float(cellVoltage_mv) / 1000);
}

void drawBatteryIcon()
{
  /*
    // Defining how big the battery icon is displayed
  */
  const byte hBattery_px = 46;
  const byte wBattery_px = 17;
  const byte rBattery_px = 0;
  const byte cBattery_px = 5;
  const byte hKnob_px = 8;
  const byte lineWidth_px = 2;

  // the body of the battery
  byte hBody_px = hBattery_px - hKnob_px + lineWidth_px * 2;
  //byte cKnob_px = wBattery_px / 3 - 1+ cBattery_px;
  //byte wKnob_px = wBattery_px - cKnob_px*2 - 1 - cBattery_px;
  byte wKnob_px = 2 * (wBattery_px / 6 ) + 3 ;
  byte cKnob_px = (wBattery_px - wKnob_px) / 2 + cBattery_px;

  // The knob on top of the battery
  byte rKnob_px = rBattery_px;
  byte rBody_px = hKnob_px - 2 * lineWidth_px;

  if (doBatteryCheck)
  {
    // The bar in the battery
    byte hBar_px = (batteryPercentage_pct * (hBody_px - 4 * lineWidth_px)) / 100 ;
    byte rBar_px = rBody_px + hBattery_px - hBar_px - 4 * lineWidth_px;
    byte cBar_px = cBattery_px + 2 * lineWidth_px;
    byte wBar_px = wBattery_px - 4 * lineWidth_px;

    // draw the knob (non moving part)
    u8g.setColorIndex(1);
    u8g.drawRBox (cKnob_px, rKnob_px, wKnob_px, hKnob_px, 2);

    // draw the body (non moving part)
    u8g.setColorIndex(1);
    u8g.drawRBox (cBattery_px, rBody_px, wBattery_px, hBody_px, 1);
    u8g.setColorIndex(0);
    u8g.drawRBox (cBattery_px + lineWidth_px, rBody_px + lineWidth_px, wBattery_px - 2 * lineWidth_px, hBody_px - 2 * lineWidth_px, 1);

    // draw the content/status (moving part)
    u8g.setColorIndex(1);
    if (hBar_px > 4)
      u8g.drawRBox (cBar_px, rBar_px, wBar_px, hBar_px, 1);
    else if (hBar_px > 1)
      u8g.drawBox  (cBar_px, rBar_px, wBar_px, hBar_px);

    // Battery level indication

    if (statusBattery == BATTERY_ORANGE)
    {
      //Serial.println ("Draw Orange");
      u8g.setColorIndex(1);
      u8g.drawBox (wBattery_px / 2 + cBattery_px - 1, rBody_px + 4, 3, hBody_px / 2 - 2);
      u8g.drawBox (wBattery_px / 2 + cBattery_px - 1, rBody_px + hBody_px / 2 + 5, 3, 3);

    }
    else if (statusBattery == BATTERY_RED)
    {
      //Serial.println ("Draw Red");
      u8g.setColorIndex(1);
      byte rCircle_px = hBattery_px / 2;
      byte cCircle_px = wBattery_px / 2 + cBattery_px;
      byte dCircle_px = wBattery_px / 2 ;
      u8g.drawDisc (cCircle_px, rCircle_px, dCircle_px + 5);
      u8g.setColorIndex(0);
      u8g.drawDisc (cCircle_px, rCircle_px, dCircle_px + 2);


      byte Delta_px = 0.707 * dCircle_px + 2.5;
      u8g.setColorIndex(1);

      for (int i = -2; i <= 2; i++)
      {
        u8g.drawLine (cCircle_px - Delta_px + i, rCircle_px - Delta_px , cCircle_px + Delta_px + i, rCircle_px + Delta_px);
      }
    }
  }
  else
  {

  }
}

void drawLightIcons ()
{
  /*
    // define where the icons are to be displayed.
  */
  const byte r = 50;
  const byte c1 = 28 + 32;
  const byte c2 = 128 - 24;

  /*
    // The icon which indicated the status of the headlight.
  */

  if (headLed.getLedIntensity() == headLed.offIntensity)         u8g.drawBitmapP (c1, r, 3, 14, icoNone);
  else if (headLed.getLedIntensity() == headLed.lowIntensity)    u8g.drawBitmapP (c1, r, 3, 14, icoLowBeam);
  else if (headLed.getLedIntensity() == headLed.mediumIntensity) u8g.drawBitmapP (c1, r, 3, 14, icoDefaultBeam);
  else if (headLed.getLedIntensity() == headLed.highIntensity)   u8g.drawBitmapP (c1, r, 3, 14, icoHighBeam);
  else if (headLed.getLedIntensity() == headLed.maxIntensity)    u8g.drawBitmapP (c1, r, 3, 14, icoHighBeam);

  /*
    // The icon which indicated the status of the rearlight.
  */
  if (rearLed.getLedIntensity() == 0)                            u8g.drawBitmapP (c2, r, 3, 14, icoNone);
  else if (rearLed.getLedIntensity() == rearLed.offIntensity)    u8g.drawBitmapP (c2, r, 3, 14, icoLowRear);
  else if (rearLed.getLedIntensity() == rearLed.lowIntensity)    u8g.drawBitmapP (c2, r, 3, 14, icoHighRear);
  else if (rearLed.getLedIntensity() == rearLed.mediumIntensity) u8g.drawBitmapP (c2, r, 3, 14, icoFogRear);
  else if (rearLed.getLedIntensity() == rearLed.maxIntensity)    u8g.drawBitmapP (c2, r, 3, 14, icoBrakeRear);
}

void drawSensors()
{
  /*
     Draw indicators for the interust sensitive sensors.
  */
  if (speedSwitch.getInteruptActive())
  {
    u8g.drawDisc (120, 4, 3);
  }
  if (cadenceSwitch.getInteruptActive())
  {
    u8g.drawDisc (110, 4, 3);
  }
}

void drawDebug()
{
  //
  // Display which config is loaded
  //
  u8g.setPrintPos (97, 17);

#if defined(QUILTJE)
  u8g.print (2);
#elif defined(QUATRO) || defined(ICB_DF)
  u8g.print (3);
#elif defined(STRADA)
  u8g.print (1);
#endif

}

void drawGear()
{
  //
  // Display which gear we are riding in
  //

#if defined(QUATRO) || defined(ICB_DF)
  if (gearOnCassette_teeth > 9 and gearOnCassette_teeth < 100) // no NAN and inf on display.
  {


    // Either the
    //    if (gearOnCassette_index >= 10)
    //    {
    //      u8g.setPrintPos (109, 17);
    //    }
    //    else
    //    {
    //      u8g.setPrintPos (119, 17);
    //    }

 //   int col = 129 - 10 * gearOnCassette_string.length();
 //   u8g.setPrintPos (col, 17);
 //   u8g.print (gearOnCassette_string);

    // temp: also print the actual number
    u8g.setPrintPos (94,17); //94 when float
    u8g.print (gearOnCassette_teeth);

        // temp: also print the actual number
    u8g.setPrintPos (109,33); //94 when float
    int gearOnCassette_teeth_int = round(gearOnCassette_teeth + 0.5);
    u8g.print (gearOnCassette_teeth_int);

  }
  else
  {
    u8g.setPrintPos (119, 17);
    u8g.print ('-');
    u8g.setPrintPos (119, 33);
    u8g.print ('-');
  }
#endif
}



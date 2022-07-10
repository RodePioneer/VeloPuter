
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
  delay (2000);
}

void drawScreen ()
{
  /*
     This is the main function which makes sure all information is plotted in the OLED display.
  */
  drawInit();
  drawBatteryIcon();
  drawBatteryText();
  drawLightIcons();
  drawSpeed();
  drawCadence();
  drawGear();


  //
  // TEMP : uncomment this code when debugging the battery management.
  //

  // u8g.setFont(u8g_font_helvR14r);
  //
  //  u8g.setPrintPos (70, 33);
  //  u8g.print(myBattery.getVoltageCell_mv());
  //  u8g.setPrintPos (40, 33);
  //  u8g.print (myBattery.getBatteryColorCode());
  //  u8g.setPrintPos (40, 49);
  //  u8g.print (headLed.iIntensityMaxAllowed());
  //  u8g.setPrintPos (70, 49);
  //  u8g.print (headLed.iIntensityMaxAllowed());
  //
  // TEMP
  //
}

/****************************************************************************************************

   Functions for controlling the display which do not display anything

 *****************************************************************************************************/

/*

   Init: define font and set the intensity

*/
void drawInit()
{
  //  u8g.setFont(u8g_font_helvR18r); //12,14,18
  //  u8g.setFont(u8g_font_helvB18n); //12,14,18 BROKEN
  //  u8g.setFont(u8g_font_fub17n);
  u8g.setFont(u8g_font_fur17n);
  u8g.setColorIndex(1);
}

/*
  Blinking the screen when the blinkers are on.
*/

void blinkScreen (byte doBlink)
{
  if (doBlink)
  {
    u8g.firstPage();
    do {
      u8g.setColorIndex(1);
      u8g.drawBox (0, 0, 128, 64);
    } while ( u8g.nextPage() );
  }
}
/****************************************************************************************************

   Functions which display text on the display

 *****************************************************************************************************/

/*
   Display the speed.
*/
void drawSpeed()
{

  const byte row =  2; 
  const byte col =  30;
  const byte dcol = 13;
  u8g.setFontPosTop();

  // Note that we double the display scale in order to get larger characters without the need for an other font to be loaded.
  u8g.setScale2x2();

  // The starting potion depends on the number of characters to display.
  if (speed_kmh < 10)
    u8g.setPrintPos (col, row);
  else if (speed_kmh < 100)
    u8g.setPrintPos (col - dcol, row);
  else
    u8g.setPrintPos (col - dcol * 2, row);

  if (speedSwitch.getInteruptActive())
  {
    // Active: print a number
    u8g.print (speed_kmh);
  }
  else
  {
    // Not active: print a dash
    u8g.print ('-');
  }

  u8g.undoScale();
}

/*
   Display the cadence
*/
void drawCadence()
{
  // Define hte position
  const byte row =  22;
  const byte col =  115;
  const byte dcol = 13;
  u8g.setFontPosTop();

  // The starting potion depends on the number of characters to display.
  if (cadence_rpm < 10)
  {
    // For cad <10
    u8g.setPrintPos (col, row); 
  }
  else if (cadence_rpm < 100)
  {
    // For cad >= 10, <100
    u8g.setPrintPos (col - dcol, row); 
  }
  else
  {
    // For cad >= 100
    u8g.setPrintPos (col - dcol * 2, row);
  }

  // Only display the cadence then we have a cadence at all
  if (cadenceSwitch.getInteruptActive())
  {
    // Active: print a number
    u8g.print (cadence_rpm);
  }
  else
  {
    // Not active: print a dash
    u8g.print ('-');
  }
}

/*

   Display the gear (number of teeth) of the cassette

*/
void drawGear()
{
  //
  // Display which gear we are riding in
  //
  const byte row =  0; 
  const byte col =  115; 
  const byte dcol = 13;
  u8g.setFontPosTop();


  if (gearOnCassette_teeth > 8 and gearOnCassette_teeth < 100) // no NAN and inf on display.
  {
    // The starting potion depends on the number of characters to display.

    // number of teeth on current gear
    if (gearOnCassette_teeth < 10)
    {
      // For number of teeth <10
      u8g.setPrintPos (col, row); 
    }
    else if (gearOnCassette_teeth < 100)
    {
      // For number of teeth >= 10, <100
      u8g.setPrintPos (col - dcol, row); 
    }
    else
    {
      // For number of teeth >= 100
      u8g.setPrintPos (col - dcol * 2, row); 
    }

    // Draw it. Note that we assume that the max cassete is 36. If we count more than that we assume that there is a slumpf. 
    // Note that this is build for velomobiles which do not have a front derailieur.
    if (gearOnCassette_teeth < 37.4)
    {
      u8g.print (round(gearOnCassette_teeth));
    }
    else
    {
      // Slumpf is in effect: divide the number of teeth by 2.5
      u8g.print (round(gearOnCassette_teeth / 2.5));
    }
  }
  else
  {
    // Not active: print a dash
    u8g.setPrintPos (col, row); // teeth <10
    u8g.print ('-');
  }
}

void drawBatteryText()
{
  /*
     The text underneath the battery. Currently we show the cell voltage. This can also easily be changed to
  */
  const byte row =  64;
  const byte col =  0;
  u8g.setFontPosBottom();
  u8g.setPrintPos (col, row);

  u8g.print (myBattery.getVoltageCell_v());
}

/****************************************************************************************************

   Functions which display icons on the display

 *****************************************************************************************************/

/*
   Defining how big the battery icon is displayed
*/
void drawBatteryIcon()
{

  const byte hBattery_px = 44; // was 46
  const byte wBattery_px = 17;
  const byte rBattery_px = 0;
  const byte cBattery_px = 0;
  const byte hKnob_px = 8;
  const byte lineWidth_px = 2;

  // the body of the battery
  byte hBody_px = hBattery_px - hKnob_px + lineWidth_px * 2;
  byte wKnob_px = 2 * (wBattery_px / 6 ) + 3 ;
  byte cKnob_px = (wBattery_px - wKnob_px) / 2 + cBattery_px;

  // The knob on top of the battery
  byte rKnob_px = rBattery_px;
  byte rBody_px = hKnob_px - 2 * lineWidth_px;

  if (doBatteryCheck)
  {
    // The bar in the battery
    byte hBar_px = (myBattery.getPercentage_pct() * (hBody_px - 4 * lineWidth_px)) / 100 ;
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

    /*
      Battery level indication

      On level Orange we draw an exclamation mark.
      On level Red we draw an icon indicating a prohibition: a circl with a diagonal line on it.

    */
    if (myBattery.getBatteryColorCode() == BATTERY_ORANGE)
    {
      // Exclamation mark indicating
      u8g.setColorIndex(0);
      u8g.drawBox (wBattery_px / 2 + cBattery_px - 3, rBody_px + 4, 7, hBody_px / 2 + 6);
      u8g.setColorIndex(1);
      u8g.drawBox (wBattery_px / 2 + cBattery_px - 1, rBody_px + 4, 3, hBody_px / 2 - 2);
      u8g.drawBox (wBattery_px / 2 + cBattery_px - 1, rBody_px + hBody_px / 2 + 5, 3, 3);
    }
    else if (myBattery.getBatteryColorCode() == BATTERY_RED)
    {
      // Wide circle.
      byte rCircle_px = hBattery_px / 2;
      byte cCircle_px = wBattery_px / 2 + cBattery_px;
      byte dCircle_px = wBattery_px / 2 ;
      u8g.setColorIndex(0);
      u8g.drawDisc (cCircle_px, rCircle_px, dCircle_px + 7);
      u8g.setColorIndex(1);
      u8g.drawDisc (cCircle_px, rCircle_px, dCircle_px + 4);
      u8g.setColorIndex(0);
      u8g.drawDisc (cCircle_px, rCircle_px, dCircle_px );

      // Diagonal line (5 lines next to each other)
      byte Delta_px = 0.707 * dCircle_px + 2.5;
      u8g.setColorIndex(1);
      for (int i = -2; i <= 2; i++)
      {
        u8g.drawLine (cCircle_px - Delta_px , rCircle_px - Delta_px + i , cCircle_px + Delta_px , rCircle_px + Delta_px + i);
      }
    }
  }
  else
  {

  }
}

/*
   The light icons

   We draw one for the head lights and one for the rear lights.
*/
void drawLightIcons ()
{

  const byte r = 50;
  const byte c1 = 28 + 32;
  const byte c2 = 128 - 24;
  bool setIsBrakeSwitchOn      = brakeSwitch.getState() == LOW;

  /*
     The icon which indicated the status of the headlight.
  */
  if (headLed.getICurrentIntensity() == 1)         u8g.drawBitmapP (c1, r, 3, 14, icoNone);
  else if (headLed.getICurrentIntensity() == 2)    u8g.drawBitmapP (c1, r, 3, 14, icoLowBeam);
  else if (headLed.getICurrentIntensity() == 3)    u8g.drawBitmapP (c1, r, 3, 14, icoDefaultBeam);
  else if (headLed.getICurrentIntensity() == 4)    u8g.drawBitmapP (c1, r, 3, 14, icoHighBeam);

  /*
     The icon which indicated the status of the rearlight.
  */
  //
  if (brakeLed.getICurrentIntensity() == brakeLed.iIntensityMaxAllowed() and setIsBrakeSwitchOn) u8g.drawBitmapP (c2, r, 3, 14, icoBrakeRear);
  else if (rearLed.getICurrentIntensity() == 0)    u8g.drawBitmapP (c2, r, 3, 14, icoNone);
  else if (rearLed.getICurrentIntensity() == 1)    u8g.drawBitmapP (c2, r, 3, 14, icoLowRear);
  else if (rearLed.getICurrentIntensity() == 2)    u8g.drawBitmapP (c2, r, 3, 14, icoHighRear);
  else if (rearLed.getICurrentIntensity() == 3)    u8g.drawBitmapP (c2, r, 3, 14, icoFogRear);
  else if (rearLed.getICurrentIntensity() == 4)    u8g.drawBitmapP (c2, r, 3, 14, icoBrakeRear);
  else if (rearLed.getICurrentIntensity() == 5)    u8g.drawBitmapP (c2, r, 3, 14, icoBrakeRear);
}

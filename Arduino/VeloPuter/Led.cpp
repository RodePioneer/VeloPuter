/*

  A class per Led. Perhaps we should extent it to leds in general?

*/
#include <Arduino.h>

class Led
{
    //private:
  private:
    //volatile byte flashOn = false;
    volatile byte ledIntensity_255 = 0;

    volatile long tLastStateChange_ms = 0;
    byte pin;
    byte iIntensityCurrent = 0;
    //byte iIntensityMaxAllowed =  255;
    byte iIntensityPrevious = 0;
    enum {BATTERY_GREEN, BATTERY_ORANGE, BATTERY_RED, BATTERY_BLACK};
    byte myBatteryGetColorCode = BATTERY_GREEN;

    void setLedIntensity (byte newIntensity_255) // directly set the new LED value, either digital or analog.
    {
      ledIntensity_255 = newIntensity_255;
      //flashOn = true;
      analogWrite(pin, ledIntensity_255);
      tLastStateChange_ms = millis();
    }

    byte IMax (void)
    {
      // Run only once to find the max.
      byte iIntensityMaxPossible = 255;

      if (iIntensityMaxPossible == 255)
      {
        for (byte i = 7; i > 0; i--)
        {
          // We found the end. Set the index to the one position back i-1)
          if (setIntensities[i] == -1) iIntensityMaxPossible = i - 1;
        }
      }

      //
      return iIntensityMaxPossible;
    }

  public:
    //byte setBatteryLimit = 0;

    /************************************************************

       Configuration of the LED

     ************************************************************/
    int setIntensities[8] = { -1, -1, -1, -1, -1, -1, -1, -1} ; // -1 indicates the end of the array /  setting




    byte iIntensityMaxAllowed()
    {
           //
      // Determine the max allowed intensity for this battery color 
      //
      byte IIntensityMaxAllowed = IMax();
      
      if (myBatteryGetColorCode == BATTERY_GREEN)
      {
        IIntensityMaxAllowed = IMax();
      }
      else if (myBatteryGetColorCode == BATTERY_ORANGE)
      {
        IIntensityMaxAllowed = IMax() - 1;
      }
      else if (myBatteryGetColorCode == BATTERY_RED)
      {
        IIntensityMaxAllowed = IMax() - 2;
      }
      else if (myBatteryGetColorCode == BATTERY_BLACK)
      {
        IIntensityMaxAllowed = 0;
      }
      return IIntensityMaxAllowed;
    }

    //
    // Functions for cunfiguring the LED
    //
    void setPinID (byte Pin)
    {
      pin = Pin;
    }


    // Return the INDEX of the current intensity setting.
    byte getICurrentIntensity ()
    {
      return iIntensityCurrent;
    }

    /************************************************************

      Controlling the LED

    ************************************************************/
    void setICurrentIntensity (byte iIntensityNew)
    {
      // TODO: Add coutner and run experiments to track how often this is called. Sole the upa dn down issues we have.
      iIntensityCurrent = iIntensityNew;
      setLedIntensity(setIntensities[iIntensityCurrent]);
    }

    // set the intensity to the sleep/black state.
    void setLedBlack (void)
    {
      iIntensityCurrent = 0;
      setICurrentIntensity(iIntensityCurrent);
    }

    // set the intensity to the off state. Note that this is NOT the black state. It can denote a minimum intensity.
    void setLedOff (void)
    {
      iIntensityCurrent = 1;
      setICurrentIntensity(iIntensityCurrent);
    }

    // Set the intensity to the low setting (default for read and headlights)
    void setLedLow (void) // switch to high / max
    {
      iIntensityCurrent = 2;
      setICurrentIntensity(iIntensityCurrent);
    }
   

    // Increase intensity until at the max setting (array index 1)
    // Add the battery state check here. Limit the up based on the battery level.
    void upLed (void)
    {      
      //
      // Up the intensity if needed
      //
      if (iIntensityCurrent < iIntensityMaxAllowed())
      {
        //iIntensityCurrent++;
        setICurrentIntensity(iIntensityCurrent + 1);
      }
    }

    // Reduce intensity until at the lowest setting (array index 1)
    void downLed (void)
    {
      if (iIntensityCurrent > 1)
      {
        //iIntensityCurrent--;
        setICurrentIntensity(iIntensityCurrent - 1);
      }
    }

    // Add the battery state check here. Limit the up based on the battery level.
    // This is mainly relevant for the brakes and the indicators.
    //    void setLedMax (void) // switch to high / max
    //    {
    //      iIntensityCurrent = IMax();
    //      setICurrentIntensity(iIntensityCurrent);
    //    }

    void setLedMax (void)
    {
      if (iIntensityCurrent != iIntensityMaxAllowed())
      {
        iIntensityCurrent = iIntensityMaxAllowed();
        setICurrentIntensity(iIntensityCurrent);
      }
    }

    void setLedToPreviousIntensity (void)
    {
      if (iIntensityCurrent != iIntensityPrevious)
      {
        iIntensityCurrent = iIntensityPrevious;
        setICurrentIntensity(iIntensityCurrent);
      }
    }


    // When toggeling (for instance a brake light) return to the previous intensity.
    byte getLedPrevious (void)
    {
      return iIntensityPrevious;
    }

    // Store that we should consider the previous intensity
    void setLedStoreCurrentIntensityAsPrevious (void)
    {
      iIntensityPrevious = iIntensityCurrent;
    }

    //
    // TODO: depricate this or use setMax and setOff instead of doing it ourself.
    // only used by the indicators. which Toggle Off and Max . (RENAME THIS TO THAT!)
    //
    void setLedToggleMax (void)
    {
      if (iIntensityCurrent >= iIntensityMaxAllowed()) // use >= because the max may have been lowered.
        // if the intensity is max: go back to the last known value
      {
        iIntensityCurrent = min(iIntensityMaxAllowed(), iIntensityPrevious); // use min in case the IMax and previous are the same....
      }
      else
        // if the intensity is not max: store the intensity and then set hte intensity to max
      {
        iIntensityPrevious = iIntensityCurrent;
        iIntensityCurrent = iIntensityMaxAllowed();
      }
      setICurrentIntensity(iIntensityCurrent);
    }


    long getTimeLastChange_ms (void)
    {
      return tLastStateChange_ms;
    }


    //
    //  Update the color and adjust the intenstiy when needed.
    //  
    void setMyBatteryGetColorCode (byte newBatteryColor)
    {
      // Update battery status
      myBatteryGetColorCode = newBatteryColor;  

      // If needed recude light intensity
      if (iIntensityCurrent > iIntensityMaxAllowed())
      {
        setICurrentIntensity(iIntensityMaxAllowed());
      }
    }

};

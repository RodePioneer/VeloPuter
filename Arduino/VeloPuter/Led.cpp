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
    byte iIntensityMax = 0;
    byte iIntensityPrevious = 0;
    byte setBatteryLimit = 0;

    void setLedIntensity (byte newIntensity_255) // directly set the new LED value, either digital or analog.
    {
      ledIntensity_255 = newIntensity_255;
      //flashOn = true;
      analogWrite(pin, ledIntensity_255);
      tLastStateChange_ms = millis();
    }

  public:
    // used to limit the max intensities according to the battery state.

    //    byte getFlashOnStatus()
    //    {
    //      return flashOn;
    //    }
    //
    //    void toggleFlashLed() // set led intensity to max intensity, without changing internal led intensity (for easy reset)
    //    {
    //      if (flashOn)
    //      {
    //        analogWrite(pin, ledIntensity_255);
    //        flashOn = false;
    //        tLastStateChange_ms = millis();
    //      }
    //      else
    //      {
    //        setLedMax();
    //        //analogWrite(pin, maxIntensity);
    //        flashOn = true;
    //        tLastStateChange_ms = millis();
    //      }
    //    }



    /************************************************************

       Configuration of the LED

     ************************************************************/
    int setIntensities[8] = { -1, -1, -1, -1, -1, -1, -1, -1} ; // -1 indicates the end of the array /  setting

    byte IMax (void)
    // Run only once to find the max.
    {

      if (iIntensityMax == 0)
      {
        for (byte i = 7; i > 0; i--)
        {
          if (setIntensities[i] == -1) iIntensityMax = i - 1;
        }
      }

      //
      // Adjust the max acording to the color code of the battery state
      //
      // setBatteryLimit should be 0 for green, 1 for orange, 2 for red.
      iIntensityMax = iIntensityMax - setBatteryLimit ;

      //
      return iIntensityMax;
    }

    void setPinID (byte Pin)
    {
      pin = Pin;
    }

    byte getPin ()
    {
      return pin;
    }

    void SetBatteryLimit (byte SetBatteryLimit)
    {
      setBatteryLimit = SetBatteryLimit;
    }


    // Return the INDEX of the current intensity setting.
    byte getICurrentIntensity ()
    {
      return iIntensityCurrent;
    }

    /************************************************************

      Controlling the LED

    ************************************************************/

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
      if (iIntensityCurrent < IMax())
      {
        iIntensityCurrent++;
        setICurrentIntensity(iIntensityCurrent);
      }
    }

    // Reduce intensity until at the lowest setting (array index 1)
    void downLed (void)
    {
      if (iIntensityCurrent > 1)
      {
        iIntensityCurrent--;
        setICurrentIntensity(iIntensityCurrent);
      }
    }

    // Add the battery state check here. Limit the up based on the battery level.
    // This is mainly relevant for the brakes and the indicators.
    void setLedMax (void) // switch to high / max
    {
      iIntensityCurrent = IMax();
      setICurrentIntensity(iIntensityCurrent);
    }


    void setLedToggleMax (void)
    {
      if (iIntensityCurrent == IMax())
        // if the intensity is max: go back to the last known value
      {
        iIntensityCurrent = iIntensityPrevious;
      }
      else
        // if the intensity is not max: store the intensity and then set hte intensity to max
      {
        iIntensityPrevious = iIntensityCurrent;
        iIntensityCurrent = IMax();
      }
      setICurrentIntensity(iIntensityCurrent);
    }

    long getTimeLastChange_ms (void)
    {
      return tLastStateChange_ms;
    }

    void setICurrentIntensity (byte iIntensityNew)
    {
      iIntensityCurrent = iIntensityNew;
      setLedIntensity(setIntensities[iIntensityCurrent]);
    }

};

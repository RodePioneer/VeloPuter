/*

  A class per Led. Perhaps we should extent it to leds in general?

*/
#include <Arduino.h>

class Led
{
    //private:
  private:
    volatile byte ledIntensity = 0;
    volatile byte flashOn = false;
    volatile long tLastStateChange_ms = 0;
    byte pin;
    byte iIntensityCurrent = 1;
    byte iIntensityMax = 0;


  public:
//    byte offIntensity = 0; // "group" intensity
//    byte lowIntensity = 0; // "group" intensity
//    byte mediumIntensity = 0; // "normal day ride"
//    byte highIntensity = 0; // "normal night ride"
//    byte maxIntensity = 0; // "max"
    int setIntensities[8] = { -1, -1, -1, -1, -1, -1, -1, -1} ; // -1 indicates the end of the array /  setting

    byte IMax (void)
    // Run only once to find the max. 
    {
      
      if (iIntensityMax == 0)
      {
        for (byte i = 0; i <= 7; i++)
        {
          if (setIntensities[i] == -1) iIntensityMax = i - 1;
        }
      }
      return iIntensityMax;
    }

    void setLedIntensity (byte newIntensity) // directly set the new LED value, either digital or analog.
    {
      ledIntensity = newIntensity;
      flashOn = true;
      analogWrite(pin, ledIntensity);
      tLastStateChange_ms = millis();
    }

    byte getLedIntensity () // return the current intensity of the led.
    {
      return ledIntensity;
    }

    byte getFlashOnStatus()
    {
      return flashOn;
    }

    void toggleFlashLed() // set led intensity to max intensity, without changing internal led intensity (for easy reset)
    {
      if (flashOn)
      {
        analogWrite(pin, ledIntensity);
        flashOn = false;
        tLastStateChange_ms = millis();
      }
      else
      {
        setLedMax();
        //analogWrite(pin, maxIntensity);
        flashOn = true;
        tLastStateChange_ms = millis();
      }
    }

    long getTimeLastChange_ms (void)
    {
      return tLastStateChange_ms;
    }

    void setICurrentIntensity (byte iIntensityNew)
    {
      iIntensityCurrent = iIntensityNew;
    }

    byte getICurrentIntensity ()
    {
      return iIntensityCurrent;
    }

    void setPinID (byte Pin)
    {
      pin = Pin;
    }

    byte getPin ()
    {
      return pin;
    }

    void setLedOff (void)
    {
      setLedIntensity(setIntensities[1]);
    }

    void setLedLow (void) // switch to high / max
    {
      setLedIntensity(setIntensities[2]);
    }

    // Add the battery state check here. Limit the up based on the battery level.
    // This is mainly relevant for the brakes and the indicators.
    void setLedMax (void) // switch to high / max
    {
        setLedIntensity(setIntensities[IMax()]);
    }

    // Increase intensity until at the max setting (array index 1)
    // Add the battery state check here. Limit the up based on the battery level.
    void upLed (void)
    {
      if (iIntensityCurrent < IMax())
      {
        iIntensityCurrent++;
        setLedIntensity(setIntensities[iIntensityCurrent]);
      }
    }

    // Reduce intensity until at the lowest setting (array index 1)
    void downLed (void)
    {
      if (iIntensityCurrent > 1)
      {
        iIntensityCurrent--;
        setLedIntensity(setIntensities[iIntensityCurrent]);
      }
    }

    void upLed4X (void) // Brake light on
    {

    }

    void downLed4X (void) // Brake light off
    {

    }

};

/*

  The class switch deals with all switches. This will be the left, right, up, down and brake handle.
  Possibly it can also be used for the cadence and the speed sensor. That is as of yet unknown.
  That would bepend on wether the signal is high long enough to reliably check it.

*/
#include <Arduino.h>

class Switch
{
  private:
    const byte tDebounce_ms  = 25; // 40 Hz on a reed contact -> 40*1.5 m/s * 3.6 = 216 km/h. Speeds in excess of this cannot be measured (single sensor) or in excess of 108 with two sensors.
    long tLastStateChange_ms = 0;
    long tNow_ms             = 0;
    volatile long tInterupts_ms[10] = {0};

    byte LastState = HIGH;
    byte iEnd = sizeof (tInterupts_ms) / sizeof (tInterupts_ms[0]) - 1;
    byte State = HIGH; // Note that due to the internal pull-up resistor HIGH means an open connection and LOW is a closed switch.
    byte StateChanged = LOW;
    byte Pin = 255; // 255 is a default which denotes that the handleling is done via interupts

  public:
    void setPin (byte pin)
    {
      Pin = pin;
    }

    long getTimeLastChange_ms (void)
    {
      return tLastStateChange_ms;
    }

    byte getPin (void)
    {
      return Pin;
    }

    byte getState (void)
    {
      return State;
    }

    byte hasStateChanged (void)
    {
      return StateChanged;
    }

    byte getInteruptActive (void)
    {
      /*
         An interupt s NOT active when there are zeros left in the
      */
      byte numOfZeros = false;
      for (byte i = 0; i <= iEnd; i++)
      {
        numOfZeros  += (tInterupts_ms[i] == 0);
      }
      //Serial.println ("numZEros pin " + String(numOfZeros)  );
      return (numOfZeros == 0);
    }

    float getInteruptFrequency (long tMaxDelay_ms)
    {
      /***************************************************

         Calculating the frequency we take the average time between interups
         for the last second. This however results in a slow responds to stopping
         and the in ability to handle frequencies of less than one, which for now is fine.

         We will need a solution for this.

       ***************************************************/
      byte numOfInterupts = 0;
      float frequency = 0;

      tNow_ms = millis();

      // First check is the last interupt was within the last tMaxDelay_ms .
      if (tInterupts_ms[iEnd] > (tNow_ms - tMaxDelay_ms) && tNow_ms > 1000) // wait until 1 sec has passed to avoid strange behaviour.
      {
        // Now: find out how many timestamps in the timestamp are within the last delay time.
        for (byte i = 1; i <= iEnd; i++)
        {
          if ( ((tInterupts_ms[iEnd] - tInterupts_ms[iEnd - i]) < tMaxDelay_ms) && \
               (tInterupts_ms[i] != tInterupts_ms[i + 1])) // added this condition to prevent start uip effects counting too many zeroes.
          {
            numOfInterupts++;
          }
          else
          {
            break;
          }
        }

        // Time passed (less than tMaxDelay_ms) since the last interups.
        if (numOfInterupts > 0)
        {
          frequency = float(1000.0 * numOfInterupts) / (tInterupts_ms[iEnd] - tInterupts_ms[iEnd - numOfInterupts]);
        }
      }
      return frequency;
    }

    // Currently we use a seperate Readout modulde. Once this is done the new states can be used.
    // This is to avoid changes between different readouts (the state could change)
    void ReadOut (void)
    {
      tNow_ms = millis();
      LastState = State;
      StateChanged = false;

      // Ignore everything is not enough time has passed.
      if ((tNow_ms - tLastStateChange_ms) >= tDebounce_ms)
      {
        if (Pin != 255) // difference between checked pins and interupt pins. Interupt pins only act on change.
        {
          State = digitalRead(Pin);
          StateChanged = (State != LastState);
        }
        else // interupt based
        {
          // When the last Interupt is after the last change we consider it a change!
          StateChanged = tInterupts_ms[iEnd] > tLastStateChange_ms;
        }

        if (StateChanged)
        {
          tLastStateChange_ms = tNow_ms;
        }
      }
    }

    void Interupt(void)
    {
      //
      // Since it is rather difficult to properly keep track of when the interupts happend and
      // because that is precicely what we need to know we start using an array to keep track of the
      // last number of interupt timestamps. Currently we assume that 10 stamps should be enough since that
      // would cover a whole second at the current debounce pace.
      //

      //
      // When moving at low speeds the sensor can make multiple contacts when the magnet passes the
      // reed-sensor. A solid way to prevent double counts would be to make sure that the
      // bebounce time is 1/8th of a revolution (of both the cadence and the wheel).
      //
      //  Note that we now calculate over the whole array of timestamps but that we could also
      // calculate it over a smaller window. 2016-02-13 /Gert
      //
      long tDebounceVariable_ms = (tInterupts_ms[iEnd] - tInterupts_ms[0]) / (8 * iEnd);

      // define upper als lower limits to the debounce time.
      //
      //  On a 1590 mm wheel the speed for 1 Hz ~ 6 km/h. -> debounce upper limit for this is 0.25 s = 250 ms.
      //
      //
      tDebounceVariable_ms = constrain(tDebounceVariable_ms, tDebounce_ms, 250);

      tNow_ms = millis(); // Note that millis is the time the actual interupt started!!

      // Debounce
      if ((tNow_ms - tInterupts_ms[iEnd]) >= tDebounceVariable_ms)
      {

        // shift the new time into the array.
        for (int i = 0; i < iEnd; i++)
        {
          tInterupts_ms[i] = tInterupts_ms[i + 1];
        }

        // add the latest timestamp.
        tInterupts_ms[iEnd] = tNow_ms;
      }
    }
};








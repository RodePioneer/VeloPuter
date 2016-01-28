/*

 The class switch deals with all switches. This will be the left, right, up, down and brake handle.
 Possibly it can also be used for the cadence and the speed sensor. That is as of yet unknown. 
 That would bepend on wether the signal is high long enough to reliably check it. 
 
 */
#include <Arduino.h>

class Switch
{
private: 
  const byte tDebounce_ms           = 25; // 40 Hz on a reed contact -> 40*1.5 m/s * 3.6 = 216 km/h. Speeds in excess of this cannot be measured (single sensor) or in excess of 108 with two sensors.
  unsigned long tLastStateChange_ms = 0;  
  unsigned long tNow_ms = 0;
  volatile unsigned long tInterupts_ms[10] = {0}; // 25 ms debounce -> max 1 sec.
  
  byte LastState = HIGH;
  byte iEnd = sizeof (tInterupts_ms) / sizeof (tInterupts_ms[0]) - 1;
  byte State = HIGH; // Note that due to the internal pull-up resistor HIGH means an open connection and LOW is a closed switch. 
  byte StateChanged = LOW;
  byte Pin = 255; // -1 is a default which denotes that the handleling is done via interupts
  
public: 
  void setPin (byte pin)
  {
    Pin = pin;
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

  float getInteruptFrequency (void)
  {
    /***************************************************
     * 
     * Calculating the frequency we take the average time between interups 
     * for the last second. This however results in a slow responds to stopping
     * and the in ability to handle frequencies of less than one, which for now is fine. 
     * 
     * We will need a solution for this.
     * 
     ***************************************************/
    byte numOfInterupts = 0;
    const int tMaxDelay_ms = 1000;
    float frequency = 0;
    
    tNow_ms = millis();

    // First check is the last interupt was within the last second. 
    if (tInterupts_ms[iEnd] > (tNow_ms - tMaxDelay_ms))
    {
      // Now: find out how many timestamps in the timestamp are within the last delay time. 
      for (byte i = iEnd - 1; i > 0; i--)
      {
        if ( (tInterupts_ms[iEnd] - tInterupts_ms[i]) < tMaxDelay_ms)
        {
          numOfInterupts++;
        }
        else
        {
          break;
        }
      }
  
      // Time passed (less than one second) between the last two interups.     
      if (numOfInterupts > 1)
      {
        frequency = 1000 * numOfInterupts / (tInterupts_ms[iEnd] - tInterupts_ms[iEnd - numOfInterupts]);
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
  // last number of interupt timestamps. Currently we assume that 40 stamps should be enough since that 
  // would cover a whole second at the current debounce pace. 
  //
    tNow_ms = millis();
    
    // Debouce
    
    if ((tNow_ms - tInterupts_ms[iEnd]) >= tDebounce_ms)
    { 
    
      // shift the new time into the array. 
      for (int i = 0; i <= iEnd; i++)
      {
          tInterupts_ms[i] = tInterupts_ms[i+1];
      }
  
      // add the latest timestamp.
      tInterupts_ms[iEnd] = tNow_ms;
    }
  }
};








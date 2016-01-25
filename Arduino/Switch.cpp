/*

 The class switch deals with all switches. This will be the left, right, up, down and brake handle.
 Possibly it can also be used for the cadence and the speed sensor. That is as of yet unknown. 
 That would bepend on wether the signal is high long enough to reliably check it. 
 
 */
#include <Arduino.h>

//class RPM
//{
//private:
//  float rpm = 0;
//  unsigned long timeStamps_us[10] = {    0,0,0,0,0, 0,0,0,0,0    }; // inut with an invalid value
//
//public:
//  void addTimeStamp(unsigned long t_us)
//  {
//    for (int i=9; i >= 1; i--)
//    {
//      timeStamps_us[i] = timeStamps_us[i-1];
//    }
//    timeStamps_us[0] = t_us;
//  }
//
//  float getRPM()
//  {
//    // calculate the RPM
//    // Use the folowing rules:
//    // -max 2 seconds old.
//    // if too old -> speed == 0
//    int sumT_ms = 0;
//    int sumN = 0;
//
//    for (int i=0; i < 9; i++)
//    {
//      if (timeStamps_us[i+1] !=0)
//      {
//        sumT_ms = sumT_ms + (timeStamps_us[i] - timeStamps_us[i+1])/1000;
//        sumN++;
//      }
//    }
//    
//    if (sumN > 0)
//    {
//      rpm = float(60000 * sumN) / sumT_ms;
//    }
//    else
//    {
//      rpm = 0;
//    }
//    return rpm;
//  }
//};



class Switch
{
public: 
  byte Pin = 255; // -1 is a default which denotes that the handleling is done via interupts
  //  byte numTimesOn = 0;
  //  byte numTimesOff = 0;
//  RPM rpm;

private: 
  byte LastState = HIGH;
  unsigned long tLastStateChange_ms = 0;
  const byte tDebounce_ms = 25;// 40 Hz on a reed contact -> 40*1.5 m/s * 3.6 = 216 km/h. Speeds in excess of this cannot be measured (single sensor) or in excess of 108 with two sensors.

  // thre variables to calculate interupts per second (or an equivilant) used for speed and cadence. 
//  volatile unsigned long tZeroInterupt_us = 0;
//  volatile unsigned long tLastInterupt_us = 0;
//  volatile int numInterupts = 0;
 
  
  volatile byte HadInteruptAfterLastCall = false;

  byte State = HIGH; // Note that due to the internal pull-up resistor HIGH means an open connection and LOW is a closed switch. 
  byte StateChanged = LOW;

  long tNow_ms = 0;

public: 
  byte getState (void)
  {
    return State;
  }

  byte hasStateChanged (void)
  {
    return StateChanged;
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
      if (Pin != 255) // difference between checked pins and interupt pins. Intepupt pins only act on change.
      {
        State = digitalRead(Pin);
        StateChanged = (State != LastState);
      }
      else // interupt based
      {
        StateChanged = HadInteruptAfterLastCall; // the interusts occur on the rising flank.
      }

      if (StateChanged)
      {
        tLastStateChange_ms = tNow_ms;
        //        if (State == HIGH)
        //        {
        //          numTimesOff++;
        //        }
        //        else
        //        {
        //          numTimesOn++;
        //        }
      }
    }
    // reset the interupt watch because we just did the last call. 230
    HadInteruptAfterLastCall = false;
  }

  void Interupt(void)
  {
    HadInteruptAfterLastCall = true;
  }
};








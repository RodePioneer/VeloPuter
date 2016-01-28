#include <Arduino.h>

class State
{
public:
  byte getState()
  {
    return state;
  }
  
  unsigned long getTimeInState()
  {
    return millis() - tLastStateChange_ms;
  }

  byte setState(byte nextState)
  {
    if (state != nextState)
    {
      tLastStateChange_ms = millis();
      state = nextState;
    }
  }
  
private: 
  byte state;
  unsigned long tLastStateChange_ms;
};




/*

 A class per Led. Perhaps we should extent it to leds in general?
 
 */
#include <Arduino.h> 
#include <TimerOne.h>

class Led
{ 
  //private: 
private: 
  volatile byte ledIntensity = lowIntensity;
  byte pin;
  
public:
  byte offIntensity = 0; // "group" intensity
  byte lowIntensity = 0; // "group" intensity
  byte mediumIntensity = 0; // "normal day ride"
  byte highIntensity = 0; // "normal night ride"
  byte maxIntensity = 0; // "max"

  void setLedIntensity (byte newIntensity) // directly set the new LED value, either digital or analog.
  {
    ledIntensity = newIntensity;

    // Because we Timer1 pin 9 and 10 are broken for analogwrite
    if (pin == 9 or pin == 10 ) 
    {
      Timer1.pwm(pin, 4*ledIntensity);
    }
    else
    {
      analogWrite(pin, ledIntensity);
    }
    // tracer on a specific or all pins. 
    //if (pin == 11) Serial.println ("Setting pin " +String(pin) + " to " + String(ledIntensity) );
  
  }

  byte getLedIntensity () // return the current intensity of the led.
  {
    return ledIntensity;
  }

  void setPin (byte Pin)
  {
    pin = Pin;
  }
  
  byte getPin ()
  {
    return pin;
  } 

  void setLedOff (void)
  {
    ledIntensity = offIntensity;
    setLedIntensity(ledIntensity);
  }
  
  void setLedLow (void) // switch to high / max 
  {
    ledIntensity = lowIntensity;
    setLedIntensity(ledIntensity);
  }

  void setLedMedium (void) // switch to low / min
  {
    ledIntensity = mediumIntensity;
    setLedIntensity(ledIntensity);
  }

  void setLedHigh (void) // switch to high / max 
  {
    ledIntensity = highIntensity;
    setLedIntensity(ledIntensity);
  }

  void setLedMax (void) // switch to high / max 
  {
    ledIntensity = maxIntensity;
    setLedIntensity(ledIntensity);
  }

  void upLed (void) // increase the intensity
  {
    if      (ledIntensity == offIntensity) setLedLow();
    else if (ledIntensity == lowIntensity) setLedMedium();
    else if (ledIntensity == mediumIntensity) setLedHigh();
    else if (ledIntensity == highIntensity) setLedMax();
  }
  
  void downLed (void) // lower the intensity
  {
    if      (ledIntensity == lowIntensity) setLedOff();
    else if (ledIntensity == mediumIntensity) setLedLow();
    else if (ledIntensity == highIntensity) setLedMedium();
    else if (ledIntensity == maxIntensity) setLedHigh();
  }
};



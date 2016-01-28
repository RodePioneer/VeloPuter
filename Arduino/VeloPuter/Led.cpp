/*

 A class per Led. Perhaps we should extent it to leds in general?
 
 */
#include <Arduino.h> 
 
class Led
{ 
  //private: 
private: 
  byte ledIntensity = lowIntensity;
  byte Pin;

public:
  byte minIntensity = 8; // "group" intensity
  byte lowIntensity = 16; // "normal day ride"
  byte highIntensity = 255; // "normal night ride"
  byte maxIntensity = 255; // "max"

  void setLedIntensity (byte newState) // directly set the new LED value, either digital or analog.
  {
    ledIntensity = newState;
    analogWrite(Pin, ledIntensity);
  }

  byte getLedIntensity () // return the current intensity of the led.
  {
    return ledIntensity;
  }

  void setPin (byte pin)
  {
    Pin = pin;
  }
  
  byte getPin ()
  {
    return Pin;
  }

  void toggleledIntensity (void) // Flip the state of the LED
  {
    if (ledIntensity == lowIntensity) 
    {
      ledIntensity = highIntensity;
    }
    else // if == high
    {
      ledIntensity = lowIntensity;
    }
    analogWrite(Pin, ledIntensity);  
  }  

  void setLedHigh (void) // switch to high / max 
  {
    ledIntensity = highIntensity;
    analogWrite(Pin, ledIntensity);
  }

  void setLedMin (void) // switch to high / max 
  {
    ledIntensity = minIntensity;
    analogWrite(Pin, ledIntensity);
  }

  void setLedMax (void) // switch to high / max 
  {
    ledIntensity = maxIntensity;
    analogWrite(Pin, ledIntensity);
  }

  void setLedLow (void) // switch to low / min
  {
    ledIntensity = lowIntensity;
    analogWrite(Pin, ledIntensity);
  }

//  void upledIntensity (void) // increase the intensity of the LED using PWM
//  {
//    ledIntensity *= 4;
//    ledIntensity = constrain (ledIntensity, 1, 255);
//    analogWrite(Pin, ledIntensity);  
//  } 
//
//  void downledIntensity (void) // redudce the intensity using PWM
//  {
//    ledIntensity /= 4;
//    ledIntensity = constrain (ledIntensity, 1, 255);
//    analogWrite(Pin, ledIntensity);  
//  } 
};



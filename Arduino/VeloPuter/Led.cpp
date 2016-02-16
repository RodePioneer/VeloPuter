/*

 A class per Led. Perhaps we should extent it to leds in general?
 
 */
#include <Arduino.h> 
#include <TimerOne.h>

class Led
{ 
  //private: 
private: 
  volatile byte ledIntensity = minIntensity;
  byte pin;

public:
  byte minIntensity = 0; // "group" intensity
  byte lowIntensity = 0; // "normal day ride"
  byte highIntensity = 0; // "normal night ride"
  byte maxIntensity = 0; // "max"

  void setLedIntensity (byte newIntensity) // directly set the new LED value, either digital or analog.
  {
    ledIntensity = newIntensity;

    // Because we Timer1 pin 9 and 10 are broken for analogwrite
    if (pin == 9 or pin == 10)
    {
      Timer1.pwm(pin, 4*ledIntensity);
    }
    else
    {
      analogWrite(pin, ledIntensity);
    }
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

  void toggleledIntensity (void) // Flip the state of the LED. Note that the toggle is between Low and High. Off, Min and Max are ignored. 
  {
    if (ledIntensity == lowIntensity) 
    {
      ledIntensity = highIntensity;
    }
    else // if == high
    {
      ledIntensity = lowIntensity;
    }
    setLedIntensity(ledIntensity);  
  }  

  void setLedOff (void)
  {
    ledIntensity = 0;
    setLedIntensity(ledIntensity);
  }
  
  void setLedMin (void) // switch to high / max 
  {
    ledIntensity = minIntensity;
    setLedIntensity(ledIntensity);
  }

  void setLedLow (void) // switch to low / min
  {
    ledIntensity = lowIntensity;
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



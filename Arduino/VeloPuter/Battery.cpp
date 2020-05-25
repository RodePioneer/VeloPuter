
#include <Arduino.h>

class Battery
{
  private:
    long tNow_ms             = 0;
    float PinValue = analogRead(Pin);
    float PinMean = PinValue; // it is initialised at the fist value and then kept in memory.

    const byte numSamples = 10 ; // was 20;

    float batteryVoltage_mv;
    int cellVoltage_mv = 4200;
    byte numOfCells = 1;
    byte Pin = 255; // 255 is a default
    enum {LIPO, LIFEPO4};
    byte SetBatteryType = LIPO;

    int getNumberOfCells ()
    {
      if (SetBatteryType == LIPO)
      {
        if (batteryVoltage_mv >= 17000) numOfCells = 1;       // 5 or more: dislay voltage 4.2*4 = 16.8
        else if (batteryVoltage_mv >= 12800) numOfCells = 4;  // 4 cells
        else if (batteryVoltage_mv >= 9000) numOfCells  = 3;  // 3 cells
        else if (batteryVoltage_mv >= 6000) numOfCells  = 2;  // 5 cells
      }
      else if (SetBatteryType == LIFEPO4)
      {
        // Do not count cells for LIFEPO4
        numOfCells = 1;
      }
      return numOfCells;
    }

  public:
    float getVoltage_mv ()
    {
      PinValue = analogRead(Pin);
      PinMean = (PinMean * (numSamples - 1) + PinValue) / numSamples; // the mean voltage on the pin.
      batteryVoltage_mv = 24.008 + 19.368 * PinMean ; // Voltage measurements done and linear fit calculated in octave 2020-05-03 @ Boekelo
      return batteryVoltage_mv ;
    }

    // Set the pin for this switch
    void setPinID (byte pin)
    {
      Pin = pin;
    }

    // retrieve the rin for this switch
    byte getPin ()
    {
      return Pin;
    }

    // set Battery type
    void setType (byte setBatteryType)
    {
      SetBatteryType = setBatteryType;
    }

    byte getType ()
    {
      return SetBatteryType;
    }

    int getVoltageCell_mv ()
    { // TODO FINISH
      cellVoltage_mv = batteryVoltage_mv / numOfCells;

      return cellVoltage_mv ;

    }
};

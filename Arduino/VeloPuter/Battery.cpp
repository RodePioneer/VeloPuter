
#include <Arduino.h>

class Battery
{
  private:
    enum {LIPO, LIFEPO4};
    byte SetBatteryType = LIPO;
    byte Pin = 255; // 255 is a default
    long tNow_ms             = 0;

    int getNumberOfCells ()
    {
      byte numOfCells = 1;
      if (SetBatteryType == LIPO)
      { // Calculate the number of cells
        float batteryVoltage_mv = getVoltage_mv();
        if (batteryVoltage_mv >= 17000) numOfCells = 1;       // 5 or more: dislay voltage 4.2*4 = 16.8
        else if (batteryVoltage_mv >= 12800) numOfCells = 4;  // 4 cells
        else if (batteryVoltage_mv >= 9000) numOfCells  = 3;  // 3 cells
        else if (batteryVoltage_mv >= 6000) numOfCells  = 2;  // 5 cells
      }
      else if (SetBatteryType == LIFEPO4)
      { // Do not count cells for LIFEPO4
        numOfCells = 1;
      }
      return numOfCells;
    }

  public:
    float getVoltage_mv ()
    { // returns the battery voltage
      const byte numSamples = 10 ; // was 20;
      //PinValue = analogRead(Pin);
      float PinMean = (PinMean * (numSamples - 1) + analogRead(Pin)) / numSamples; // the mean voltage on the pin.
      float batteryVoltage_mv = 24.008 + 19.368 * PinMean ; // Voltage measurements done and linear fit calibrated in octave 2020-05-03 @ Boekelo by Rode Pioneer
      return batteryVoltage_mv ;
    }

    float getVoltageCell_mv ()
    { // returns the battery voltage per cell. This is battery specific.
      return getVoltage_mv() / getNumberOfCells() ;
    }

    byte getPercentage_pct ()
    { // returns the battery charge left as a %. This is battery specific.
      if (SetBatteryType == LIPO)
      { // Calculate the number of cells
        return 50;
      }
      else if (SetBatteryType == LIFEPO4)
      { // Do not count cells for LIFEPO4
        return 99;
      }
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


};

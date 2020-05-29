
#include <Arduino.h>
#include "VeloPuter_Config.h"

class Battery
{
  private:
    enum {LIPO, LIFEPO4};
    enum {BATTERY_GREEN, BATTERY_ORANGE, BATTERY_RED, BATTERY_BLACK};

    byte SetBatteryType = LIPO;
    byte Pin = 255; // 255 is a default
    long tNow_ms             = 0;
    float PinMean = analogRead(Pin);

  public:


    byte getNumberOfCells ()
    {
      byte numOfCells = 1;
      if (SetBatteryType == LIPO)
      { // Calculate the number of cells for LIPO
        if (getVoltage_mv() >= 17000) numOfCells = 1;       // 5 or more: dislay voltage 4.2*4 = 16.8
        else if (getVoltage_mv() >= 12800) numOfCells = 4;  // 4 cells
        else if (getVoltage_mv() >= 9000) numOfCells  = 3;  // 3 cells
        else if (getVoltage_mv() >= 6000) numOfCells  = 2;  // 5 cells
      }
      else if (SetBatteryType == LIFEPO4)
      { // Do not count cells for LIFEPO4
        numOfCells = 1;
      }
      return numOfCells;
    }

    int getVoltage_mv ()
    { // returns the battery voltage
      const byte numSamples = 10 ;
      PinMean = (PinMean * (numSamples - 1) + analogRead(Pin)) / numSamples; // the mean voltage on the pin.
      int batteryVoltage_mv = 24.008 + 19.368 * PinMean ; // Voltage measurements done and linear fit calibrated in octave 2020-05-03 @ Boekelo by Rode Pioneer
      return batteryVoltage_mv ;
    }

    int getVoltageCell_mv ()
    { // returns the battery voltage per cell. This is battery specific.
      return getVoltage_mv() / getNumberOfCells() ;
    }

    byte getPercentage_pct ()
    { /*
        Define the voltage-percentage curves used for the interpolating lookup table
      */
#if defined(BATTERY_LIPO)
#define NUM_REFDATA 6
      /*
         Determine the battery percentage. This is a measured dischage curve for LiPo.

         measurement data
         Vlow   Vmid  Vhigh   C
         3.78   3.73  x       (5000-2600)/5000 = 48%
         4.2 = 100%
         4.0 = 90 %
         3.75 = 50%
         3.4 = 5%
         V = [4.2, 4.0, 3.75, 3.4]
         C = [100, 90, 50, 0]

        LiPo
      */
      const int V[NUM_REFDATA] = {4200, 4000, 3850, 3750, 3450, 3350};  //mV
      const int C[NUM_REFDATA] = {100,    90,   75,   50,    7,    0};      // % capacity
#elif defined(BATTERY_LIFEPO4)
#define NUM_REFDATA 7
      /*
        LiFePO4
        Constants from manual, better data is gathered.
      */
      const int V[NUM_REFDATA] = {13952, 12847, 12733, 12455, 12169, 11755, 10424}; // mV
      const int C[NUM_REFDATA] = {  100,    75,    50,    20,    10,     5,     0}; // %capacity
      numOfCells = 1;
#endif

      // Calc interpolated value
      int batteryPercentage_pct = 0;
      for (int i = NUM_REFDATA - 1; i > 0; i--)
      {
        if (getVoltageCell_mv() >= V[i])
          batteryPercentage_pct = int((C[i - 1] - C[i]) * (getVoltageCell_mv() - V[i]) / (V[i - 1] - V[i]) + C[i]);
      }

      batteryPercentage_pct = constrain(batteryPercentage_pct, 0, 99);
      return batteryPercentage_pct;
    }

    // Get battery color code
    byte getColorCode()
    {
      //
      // TODO: add delay timer before we change status of the color code.
      // TODO: add hysterisis before we change the status of the color code to a fuller level (ie. orange to green)
      // TODO: Battery discharge measurement. 25% seems in practice badly defined. 


      // Code orange is below 25%
      // Code Red is below 15%
      // Code Black is below 5%
      byte statusBattery = BATTERY_GREEN;
      const byte Batt_pct_limits[3] = {25, 15, 5};
      if (getPercentage_pct() >= Batt_pct_limits[0])
      {
        statusBattery = BATTERY_GREEN;
      }
      else if (getPercentage_pct() < Batt_pct_limits[0] &&  getPercentage_pct() >= Batt_pct_limits[1])
      {
        statusBattery = BATTERY_ORANGE;
      }

      else if (getPercentage_pct() < Batt_pct_limits[1] &&  getPercentage_pct() >= Batt_pct_limits[2])
      {
        statusBattery = BATTERY_RED;
      }
      else if (getPercentage_pct() < Batt_pct_limits[0] )
      {
        statusBattery = BATTERY_BLACK;
      }
      return statusBattery;
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

    // return the battery type (unused for for)
    byte getType ()
    {
      return SetBatteryType;
    }


};

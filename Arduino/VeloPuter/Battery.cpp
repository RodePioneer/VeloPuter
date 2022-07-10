


#include <Arduino.h>
#include "VeloPuter_Config.h"

class Battery
{
  private:
    enum {LIPO, LIFEPO4};
    enum {BATTERY_GREEN, BATTERY_ORANGE, BATTERY_RED, BATTERY_BLACK};

    // Init at defaults which are set from the outside
    byte batteryType = LIPO;
    byte batteryPin = 255;

    // Init defaults for values which are used and/or can be requested from outside
    byte batteryNumOfCells = 1;
    long batteryVoltage_mv = 0;
    long batteryCellVoltage_mv = 0; // internal use
    float batteryCellVoltage_v = 0.0; // display use
    byte batteryStatus_pct = 100;
    byte batteryStatus_color = BATTERY_GREEN;


    /*********************************************

      All calculations are done in the private updateXXX functions.
      The data can be retrieved by the getXXX functions.

    *********************************************/


    /******************************************************************************************

       Update: calculate the Battery Voltage [V]

    */
    void updateBatteryVoltage ()
    {
      long PinMean = 0;

      // returns the battery voltage
      if (PinMean == 0)
      {
        // No value set yet. Get one.
        PinMean = analogRead(batteryPin);
      }
      else
      {
        // Use an decaying moving average
        const byte numSamples = 10 ;
        PinMean = (PinMean * (numSamples - 1) + analogRead(batteryPin)) / numSamples;
      }
      batteryVoltage_mv = 19.487 * PinMean ; // Voltage measurements done and linear fit calibrated in octave 2022-07-05 @ Boekelo by Rode Pioneer Removed the offset
    }

    /******************************************************************************************

       Update: From the Battery voltage estimate the number of cells in serries in a LIPO battery

    */
    void updateBatteryNumberOfCells ()
    {
      const long Batt_voltage_limits[4] = {17000, 12800, 9000, 6000};
      if (batteryType == LIPO)
      { // Calculate the number of cells for LIPO
        if (batteryVoltage_mv >= Batt_voltage_limits[0]) batteryNumOfCells = 1;        // 5 or more: dislay voltage 4.2*4 = 16.8
        else if (batteryVoltage_mv >= Batt_voltage_limits[1]) batteryNumOfCells = 4;   // 4 cells
        else if (batteryVoltage_mv >= Batt_voltage_limits[2]) batteryNumOfCells  = 3;  // 3 cells
        else if (batteryVoltage_mv >= Batt_voltage_limits[3]) batteryNumOfCells  = 2;  // 2 cells
      }
      else if (batteryType == LIFEPO4)
      {
        // Do not count cells for LIFEPO4
        batteryNumOfCells = 1;
      }
    }

    /******************************************************************************************

       Update: From the Battery voltage and number of cell calculate the average cell voltage.

    */
    void updateVoltageCell ()
    {
      batteryCellVoltage_mv = batteryVoltage_mv / batteryNumOfCells;
      batteryCellVoltage_v = float(batteryCellVoltage_mv) / 1000.0;
    }

    /******************************************************************************************

       Update: battery color code

    */
    void updateBatteryColorCode()
    { // Code Green   == Full functionality (capacity % above 1st number)
      // Code orange  == no high beams      (capacity % below 2nd number)
      // Code Red     == only weak lights   (capacity % below 3rd number)
      // Code Black   == turn off           (capacity % below last number)


      //    const byte Batt_pct_limits[4] = {80, 25, 15, 5};
      const byte Batt_pct_limits[4] = {80, 66, 33, 5};

      //
      // Only do something and update the timer when we enter a different state
      //

      /*
         The colors go down, only on a switch to a full battery (green)
         the colorcode can go up. This is to accomodate a battery change.

         GREEN -> ORANGE -> RED -> BLACK
           ^        |        |       |
           |        |        |       |
           +--------+--------+-------+
      */

      // We can only enter green when the battery status is high enough
      if ((getPercentage_pct() >= Batt_pct_limits[0]) && (batteryStatus_color != BATTERY_GREEN))
      {
        batteryStatus_color = BATTERY_GREEN;
      }
      else if ((getPercentage_pct() < Batt_pct_limits[1]) && (batteryStatus_color == BATTERY_GREEN))
      {
        batteryStatus_color = BATTERY_ORANGE;
      }
      else if ((getPercentage_pct() < Batt_pct_limits[2]) && (batteryStatus_color == BATTERY_ORANGE))
      {
        batteryStatus_color = BATTERY_RED;
      }
      else if ((getPercentage_pct() < Batt_pct_limits[3]) && (batteryStatus_color == BATTERY_RED))
      {
        batteryStatus_color = BATTERY_BLACK;
      }
    }


    //
    // Get the percentage of charge left the the battery by comparing the cell voltage to a look up table.
    //
    void updatePercentage_pct ()
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
      const int V_mV[NUM_REFDATA] = {4200, 4000, 3850, 3750, 3550, 3350};  //mV
      const int C_pct[NUM_REFDATA] = {100,    90,   75,   50,    7,    0};      // % capacity
#elif defined(BATTERY_LIFEPO4)
#define NUM_REFDATA 7
      /*
        LiFePO4
        Constants from manual, better data is gathered.
      */
      const int V_mV[NUM_REFDATA] = {13952, 12847, 12733, 12455, 12169, 11755, 10424}; // mV
      const int C_pct[NUM_REFDATA] = {  100,    75,    50,    20,    10,     5,     0}; // %capacity
      numOfCells = 1;
#endif

      //
      // Calc interpolated value
      //
      // TODO: Change this to a two step aproach where we first find the index and then only calculate the pct only once.
      int batteryPercentage_pct = 0;
      for (int i = NUM_REFDATA - 1; i > 0; i--)
      {
        if (getVoltageCell_mv() >= V_mV[i])
          // Vcell        = measured cell voltage = getVoltageCell_mv()
          // V_mV[i]      = lower voltage for this intervall
          // V_mV[i - 1]  = upper voltage for this interval
          // C_pct[i]     = lower cappacity for this interval
          // C_pct[i - 1] = upper cappacity for this interval
          //
          //                                        Vcell - V_mV[i]
          // pct =  ( C_pct[i-1] - C_pct[i] ) * -------------------- + C_pct[i]
          //                                     V_mV[i-1]- V_mV[i]
          //
          batteryPercentage_pct = int( (C_pct[i - 1] - C_pct[i]) * (getVoltageCell_mv() - V_mV[i]) / (V_mV[i - 1] - V_mV[i]) + C_pct[i] );
      }

      batteryStatus_pct = constrain(batteryPercentage_pct, 0, 99);
    }

    /*
       Cell voltage in mv for internal use
    */
    int getVoltageCell_mv ()   {
      return batteryCellVoltage_mv;
    }


  public:
    /**************************************************************************************
        Update the battery
    */
    void updateBattery()
    {
      /*
        TODO: Add delay so we don't calculate every second.
      */
      updateBatteryVoltage();
      updateBatteryNumberOfCells();
      updateVoltageCell();
      updatePercentage_pct();
      updateBatteryColorCode ();
    }

    /*
       Return values
    */
    // Voltage is shown below the icon.
    float getVoltageCell_v ()   {
      return batteryCellVoltage_v;
    }
    // The color determines if there is an ! or a X displayed to indicate Orange or Red
    byte getBatteryColorCode() {
      return batteryStatus_color;
    }
    // The percentage determines how high the bar in the battery icon is.
    byte getPercentage_pct ()  {
      return batteryStatus_pct;
    }


    /*
       Configure the battery
    */
    void setPinID (byte Pin) {
      batteryPin = Pin;
    }
    void setType (byte Type) {
      batteryType = Type;
    }
};

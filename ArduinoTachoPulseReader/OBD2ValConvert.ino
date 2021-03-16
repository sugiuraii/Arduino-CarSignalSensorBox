#include "OBD2ValConvert.h"
#include <math.h>

constexpr int NumPulsePerRev = 2;
constexpr int NumPulsePerSpd = 4;

constexpr int ADC_STEP = 4096; // 10bits(1024) * 4 sample
constexpr int ADC_REF_VOLTAGE = 5;

constexpr int BoostADCCoeff = 100;

// Water/Oil temp sensor Constants
// This parameter are derived from Autogauge water/oil temp sensor (9BTP000 3747-SENSOR)
constexpr double WATER_THERMISTOR_R0 = 1306; //Thermistor resistance at 25degC
constexpr double WATER_THERMISTOR_B = 3529;
constexpr double OIL_THERMISTOR_R0 = 1306; //Thermistor resistance at 25degC
constexpr double OIL_THERMISTOR_B = 3529;
// Thermistor current sense reisistance (inserted in series with the thermistor)
constexpr double WATER_THERMISTOR_SENSE_R = 100;
constexpr double OIL_THERMISTOR_SENSE_R = 100;

uint8_t convertToVechicleOBDSpeed(unsigned long speedPulseElapsedTime)
{
    if(speedPulseElapsedTime == 0)
        return 0;
    else
        return (3600*1000000) / (637*speedPulseElapsedTime*NumPulsePerSpd);
}

// Return EngineRPM * 4
uint16_t convertToOBDEngineREV(unsigned long tachoPluseElapsedTime)
{
    if(tachoPluseElapsedTime == 0)
        return 0;
    else
        return (60*1000000*4) / (NumPulsePerRev*tachoPluseElapsedTime);
}

uint8_t convertToOBDManifoldAbsPressure(int boostADCVal)
{
    return BoostADCCoeff * boostADCVal * ADC_REF_VOLTAGE / ADC_STEP;
}

uint8_t convertToOBDCoolantTemperature(int waterTempADCVal)
{
    double WaterTempThermistorSerialResistance = WATER_THERMISTOR_SENSE_R;
    double THERMISTOR_B = WATER_THERMISTOR_B;
    double THERMISTOR_R0 = WATER_THERMISTOR_R0;

    double R = (double)(waterTempADCVal * WaterTempThermistorSerialResistance) / (double)(ADC_STEP - waterTempADCVal);
    double T = THERMISTOR_B/(log(R/THERMISTOR_R0)+THERMISTOR_B/298.15);
    double Tdeg = T - 273.15;
    return Tdeg + 40;
}

uint8_t convertToOBDEngineOILTemperature(int oilTempADCVal)
{
    double WaterTempThermistorSerialResistance = OIL_THERMISTOR_SENSE_R;
    double THERMISTOR_B = OIL_THERMISTOR_B;
    double THERMISTOR_R0 = OIL_THERMISTOR_R0;

    double R = (double)(oilTempADCVal * WaterTempThermistorSerialResistance) / (double)(ADC_STEP - oilTempADCVal);
    double T = THERMISTOR_B/(log(R/THERMISTOR_R0)+THERMISTOR_B/298.15);
    double Tdeg = T - 273.15;
    return Tdeg + 40;
}

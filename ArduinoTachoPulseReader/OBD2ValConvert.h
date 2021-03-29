#ifndef OBD_VAL_CONVERT_H
#define OBD_VAL_CONVERT_H

uint8_t convertToVechicleOBDSpeed(unsigned long speedPulseElapsedTime);
uint16_t convertToOBDEngineREV(unsigned long tachoPluseElapsedTime);
uint8_t convertToOBDManifoldAbsPressure(int boostADCVal);
uint8_t convertToOBDCoolantTemperature(int waterTempADCVal);
uint8_t convertToOBDEngineOILTemperature(int oilTempADCVal);

#endif

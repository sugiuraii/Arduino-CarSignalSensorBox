#ifndef ADC_READ_H
#define ADC_READ_H

#include "SensorADCPinAssign.h"

constexpr int NUM_ADC_CHANNELS = 6;
int* getAnalogReadVal();
void updateAnalogReadVal();

#endif
#include "ADCRead.h"

static int analogReadVal[6];
//AnalogRead Oversampling count (1:No oversample)
constexpr int OVERSAMPLE_COUNT = 4;

int *getAnalogReadVal()
{
    return analogReadVal;
}

void updateAnalogReadVal()
{
    for (int i = 0; i < NUM_ADC_CHANNELS; i++)
    {
        analogReadVal[i] = 0;
        for (int j = 0; j < OVERSAMPLE_COUNT; j++)
            analogReadVal[i] += analogRead(i);
    }
}
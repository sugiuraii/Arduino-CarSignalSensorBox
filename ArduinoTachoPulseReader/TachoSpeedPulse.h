#ifndef TACHO_SPEED_PULSE_H
#define TACHO_SPEED_PULSE_H

#include "ArduinoTachoPulseReader.h"

void tachoSpeedPinSetup();
inline unsigned long getSpeedPulseElapsedTime(const unsigned long nowTime);
inline unsigned long getTachoPulseElapsedTime(const unsigned long nowTime);

#endif
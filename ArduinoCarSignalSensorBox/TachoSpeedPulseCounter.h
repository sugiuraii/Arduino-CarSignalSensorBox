#ifndef TACHO_SPEED_PULSE_COUNTER_H
#define TACHO_SPEED_PULSE_COUNTER_H

void tachoSpeedPinSetup();
inline unsigned long getSpeedPulseElapsedTime(const unsigned long nowTime);
inline unsigned long getTachoPulseElapsedTime(const unsigned long nowTime);

#endif
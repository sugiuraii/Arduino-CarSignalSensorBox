#include "TachoSpeedPulseCounter.h"

//Speed,Tacho pulse reset time (in microsecond)
constexpr unsigned long SPEED_PULSE_EXPIRE_TIME = 1000000;
constexpr unsigned long TACHO_PULSE_EXPIRE_TIME = 1000000;

// Setup speed/tacho pulse trigger pin and interrupt
constexpr int speedPin = 2;
constexpr int speedInterrupt = 0;
constexpr int tachoPin = 3;
constexpr int tachoInterrupt = 1;

volatile static unsigned long speedPulseElapsedTime = 0;
volatile static unsigned long tachoPulseElapsedTime = 0;
volatile static unsigned long speedPulseBeforeTime = 0;
volatile static unsigned long tachoPulseBeforeTime = 0;

void tachoSpeedPinSetup()
{
  pinMode(speedPin, INPUT_PULLUP);
  pinMode(tachoPin, INPUT_PULLUP);

  attachInterrupt(speedInterrupt, interruptSpeedPulse, RISING);
  attachInterrupt(tachoInterrupt, interruptTachoPulse, RISING);
}

inline unsigned long getSpeedPulseElapsedTime(const unsigned long nowTime)
{
  //Overwrite elapsed time if no pulse is detected in 1sec
  if ((nowTime - speedPulseBeforeTime) > SPEED_PULSE_EXPIRE_TIME)
    return 0;
  return speedPulseElapsedTime;
}

inline unsigned long getTachoPulseElapsedTime(const unsigned long nowTime)
{
  //Overwrite elapsed time if no pulse is detected in 1sec
  if ((nowTime - tachoPulseBeforeTime) > TACHO_PULSE_EXPIRE_TIME)
    return 0;
  return tachoPulseElapsedTime;
}

void interruptSpeedPulse()
{
  unsigned long nowTime = micros();
  if (nowTime < speedPulseBeforeTime) //Case of micors() overflow
    speedPulseElapsedTime = nowTime + 4294967295UL - speedPulseBeforeTime;
  else
    speedPulseElapsedTime = nowTime - speedPulseBeforeTime;

  speedPulseBeforeTime = nowTime;
}

void interruptTachoPulse()
{
  unsigned long nowTime = micros();
  if (nowTime < tachoPulseBeforeTime) //Case of micors() overflow
    tachoPulseElapsedTime = nowTime + 4294967295UL - tachoPulseBeforeTime;
  else
    tachoPulseElapsedTime = nowTime - tachoPulseBeforeTime;

  tachoPulseBeforeTime = nowTime;
}

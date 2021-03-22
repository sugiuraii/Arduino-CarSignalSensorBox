#include "PulseGenerator.h"

// Global variables to set pulse/PWM
static unsigned int speed = 0;
static unsigned int tacho = 0;

static unsigned long speedPulseInterval = 0;
static unsigned long tachoPulseInterval = 0;

constexpr int PULSE_PER_TACHO_REV = 2;
constexpr int PULSE_PER_SPEED_REV = 4;

constexpr uint8_t speedPin = 4;
constexpr uint8_t tachoPin = 8;

void setupPulse()
{
  pinMode(speedPin, OUTPUT);
  pinMode(tachoPin, OUTPUT);
}

void setSpeed(unsigned int val)
{
    speed = val;
    if (speed != 0)
        speedPulseInterval = 5651491UL / (speed * PULSE_PER_SPEED_REV);
    else
        speedPulseInterval = 0;

    speedPulseInterval /= 2;
}

void setTacho(unsigned int val)
{
    tacho = val;
    if (tacho != 0)
        tachoPulseInterval = 60000000UL / (PULSE_PER_TACHO_REV * tacho);
    else
        tachoPulseInterval = 0;

    tachoPulseInterval /= 2;
}

void handlePulse()
{
    //Timer counter for pulse
    static unsigned long speedPulseBeforeTime = 0;
    static unsigned long tachoPulseBeforeTime = 0;
    static unsigned long analogReadBeforeTime = 0;
    static int speedPinState = LOW;
    static int tachoPinState = LOW;

    const unsigned long nowTime = micros();

    unsigned long speedPulseElapsedTime;
    unsigned long tachoPulseElapsedTime;

    //Set speed pin state
    if (nowTime > speedPulseBeforeTime)
        speedPulseElapsedTime = nowTime - speedPulseBeforeTime;
    else
        speedPulseElapsedTime = nowTime + 4294967295UL - speedPulseBeforeTime;
    if (speedPulseElapsedTime > speedPulseInterval)
    {
        if (speedPulseInterval != 0) //Speed is not zero
        {
            if (speedPinState == LOW)
            {
                speedPinState = HIGH;
                digitalWrite(speedPin, speedPinState);
            }
            else
            {
                speedPinState = LOW;
                digitalWrite(speedPin, speedPinState);
            }
        }
        speedPulseBeforeTime = nowTime;
    }

    //Set tacho pin state
    if (nowTime > tachoPulseBeforeTime)
        tachoPulseElapsedTime = nowTime - tachoPulseBeforeTime;
    else
        tachoPulseElapsedTime = nowTime + 4294967295UL - tachoPulseBeforeTime;

    if (tachoPulseElapsedTime > tachoPulseInterval)
    {
        if (tachoPulseInterval != 0)
        {
            if (tachoPinState == LOW)
            {
                tachoPinState = HIGH;
                digitalWrite(tachoPin, tachoPinState);
            }
            else
            {
                tachoPinState = LOW;
                digitalWrite(tachoPin, tachoPinState);
            }
        }
        tachoPulseBeforeTime = nowTime;
    }
}
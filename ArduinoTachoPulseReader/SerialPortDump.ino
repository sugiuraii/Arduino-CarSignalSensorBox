#include "SerialPortDump.h"
#include "TachoSpeedPulse.h"
#include "ADCRead.h"

//Serial dump interval in microsecond.
constexpr unsigned int SERIAL_DUMP_INTERVAL=16600;

void sendSerialDumpMsg()
{
    static unsigned long commBeforeTime = 0;
    unsigned long nowTime = micros();
    unsigned long commElapsedTime;
    if (nowTime < commBeforeTime)
        commElapsedTime = nowTime + 4294967295UL - commBeforeTime;
    else
        commElapsedTime = nowTime - commBeforeTime;
    //Send serial
    if (commElapsedTime > SERIAL_DUMP_INTERVAL)
    {
        sendSerialDumpInternal(getSpeedPulseElapsedTime(nowTime), getTachoPulseElapsedTime(nowTime), getAnalogReadVal(), NUM_ADC_CHANNELS);
        commBeforeTime = nowTime;
    }
}

void sendSerialDumpInternal(const unsigned long speedPulseElapsedTime, const unsigned long tachoPulseElapsedTime, const int *analogReadVal, const int numAnalogReadVals)
{
    //Send
    Serial.print("S");
    Serial.print(speedPulseElapsedTime);
    Serial.print("\nT");
    Serial.print(tachoPulseElapsedTime);
    for (int i = 0; i <= numAnalogReadVals; i++)
    {
        Serial.print("\n");
        char tagLetter = 'A' + i;
        Serial.print(tagLetter);
        Serial.print(analogReadVal[i]);
    }
    Serial.print("\n");
}

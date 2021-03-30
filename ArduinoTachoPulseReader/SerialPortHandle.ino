#include "SerialPortHandle.h"
#include "TachoSpeedPulse.h"
#include "ADCRead.h"

//Serial baudrate
//constexpr unsigned long SERIAL_BAUD_RATE = 9600;
constexpr unsigned long SERIAL_BAUD_RATE = 38400;

//communication interval in microsecond.
constexpr unsigned int COMM_INTERVAL=16600;

void setupSerialPort()
{
    //SerialPort setting
    Serial.begin(SERIAL_BAUD_RATE);
}

void sendSerialMsg()
{
    static unsigned long commBeforeTime = 0;
    unsigned long nowTime = micros();
    unsigned long commElapsedTime;
    if (nowTime < commBeforeTime)
        commElapsedTime = nowTime + 4294967295UL - commBeforeTime;
    else
        commElapsedTime = nowTime - commBeforeTime;
    //Send serial
    if (commElapsedTime > COMM_INTERVAL)
    {
        sendSerialMsgInternal(getSpeedPulseElapsedTime(nowTime), getTachoPulseElapsedTime(nowTime), getAnalogReadVal(), NUM_ADC_CHANNELS);
        commBeforeTime = nowTime;
    }
}

void sendSerialMsgInternal(const unsigned long speedPulseElapsedTime, const unsigned long tachoPulseElapsedTime, const int *analogReadVal, const int numAnalogReadVals)
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

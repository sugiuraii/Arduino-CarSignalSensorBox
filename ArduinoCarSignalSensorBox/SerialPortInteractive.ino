#include "SerialPortInteractive.h"
#include "TachoSpeedPulseCounter.h"
#include "OBD2ValConvert.h"
#include "SensorADCPinAssign.h"

constexpr bool SERIAL_INTERACTIVE_DEBUG = false;

void flushSerialInbuf();

void listenSerialInteractiveCommand()
{
    const char cmdChar = (char)Serial.read();
    
    switch(cmdChar)
    {
        case 'T':
            sendTacho();
            break;
        case 'S':
            sendSpeed();
            break;
        case 'B':
            sendBoost();
            break;
        case 'W':
            sendWaterTemp();
            break;
        case 'O':
            sendOilTemp();
            break;
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
            sendAnalogReadVal(cmdChar - '0');
            break;
        default:
            Serial.println(F("INVALID_COMMAND."));
            break;
    }
}

void flushSerialInbuf()
{
    while (Serial.read() != -1)
        Serial.read();

    if (SERIAL_INTERACTIVE_DEBUG)
        Serial.println(F("Flush serial port input buffer finished."));
}

void sendTacho()
{
    const unsigned long nowTime = micros();
    const unsigned long tachoPulseInterval = getTachoPulseElapsedTime(nowTime);
    uint16_t tachoRPM = convertToOBDEngineREVx4(tachoPulseInterval) / 4;
    Serial.println(tachoRPM);
}

void sendSpeed()
{
    const unsigned long nowTime = micros();
    const unsigned long speedPulseInterval = getSpeedPulseElapsedTime(nowTime);
    uint8_t speed = convertToVechicleOBDSpeed(speedPulseInterval);
    Serial.println(speed);
}

void sendBoost()
{
    const int* analogReadVal = getAnalogReadVal();
    uint8_t boost = convertToOBDManifoldAbsPressure(analogReadVal[BOOST_ADC_PIN]);
    Serial.println(boost);
}

void sendWaterTemp()
{
    const int* analogReadVal = getAnalogReadVal();
    uint8_t temp = convertToOBDCoolantTemperature(analogReadVal[WATERTEMP_ADC_PIN]);
    Serial.println(temp);
}

void sendOilTemp()
{
    const int* analogReadVal = getAnalogReadVal();
    uint8_t temp = convertToOBDEngineOILTemperature(analogReadVal[OILTEMP_ADC_PIN]);
    Serial.println(temp);
}

void sendAnalogReadVal(int analogCh)
{
    const int* analogReadVal = getAnalogReadVal();
    Serial.println(analogReadVal[analogCh]);
}
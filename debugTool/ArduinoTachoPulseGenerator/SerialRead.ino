#include "SerialRead.h"
#include "PulseGenerator.h"
#include "ArduinoTachoPulseGenerator.h"

void flushSerialInbuf();

constexpr unsigned long baudrate = 115200;
constexpr int SerialMSGLength = 6; // 5digits + 1header

void setupSerial()
{
    Serial.begin(baudrate);
}

void handleSerial()
{
    if (Serial.available() < SerialMSGLength)
        return;

    char header = (char)Serial.read();
    String valString;

    for (int i = 1; i < SerialMSGLength; i++)
        valString.concat((char)Serial.read());

    unsigned int val = valString.toInt();

    switch (header)
    {
    case 'S':
        setSpeed(val);
        if (MSG_DEBUG)
        {
            Serial.print("Speed is set to ");
            Serial.println(val);
        }
        break;
    case 'T':
        setTacho(val);
        if (MSG_DEBUG)
        {
            Serial.print("Tacho is set to ");
            Serial.println(val);
        }
        break;
    case 'A':
        pwmAnalogOut(3, val);
        break;
    case 'B':
        pwmAnalogOut(5, val);
        break;
    case 'C':
        pwmAnalogOut(6, val);
        break;
    case 'D':
        pwmAnalogOut(9, val);
        break;
    case 'E':
        pwmAnalogOut(10, val);
        break;
    case 'F':
        pwmAnalogOut(11, val);
        break;
    default:
        if (MSG_ERROR)
            Serial.println(F("Invalid header character."));
        flushSerialInbuf();
        break;
    }
}

void flushSerialInbuf()
{
    if (MSG_DEBUG)
        Serial.println(F("Flush serial port input buffer.."));

    while (Serial.read() != -1)
        Serial.read();

    if (MSG_DEBUG)
        Serial.println(F("Flush serial port input buffer finished."));
}

void pwmAnalogOut(uint8_t pin, int val)
{
    switch(pin)
    {
        case 3:
        case 5:
        case 6:
        case 9:
        case 10:
        case 11:
            if(val < 0 || val > 255)
            {
                if(MSG_ERROR)
                    Serial.print(F("Analog write value is out of range."));
                return;
            }
            else
            {
                analogWrite(pin, val);
                if (MSG_DEBUG)
                {
                    Serial.print(F("AnalogWrite pin"));
                    Serial.print(pin);
                    Serial.print(F("is set to "));
                    Serial.println(val);
                }
            }
            break;
        default:
            if(MSG_ERROR)
                Serial.print(F("Not supported pin for analogwrite."));
            return;
    }
}
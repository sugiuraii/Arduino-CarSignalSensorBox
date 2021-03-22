#include "SerialRead.h"
#include "PulseGenerator.h"
#include "ArduinoTachoPulseGenerator.h"

void flushSerialInbuf();

constexpr unsigned long baudrate = 9600;
constexpr int SerialMSGLength = 6; // 5digits + 1header

void setupSerial()
{
    Serial.begin(baudrate);
}

void handleSerial()
{
    char header = (char)Serial.read();
    String valString;
    
    for(int i = 1; i < SerialMSGLength; i++)
        valString.concat((char)Serial.read());

    unsigned int val = valString.toInt();

    switch(header)
    {
        case 'S':
            setSpeed(val);
            if(MSG_DEBUG)
            {
                Serial.print("Speed is set to ");
                Serial.println(val);
            }
            break;
        case 'T':
            setTacho(val);
            if(MSG_DEBUG)
            {
                Serial.print("Tacho is set to ");
                Serial.println(val);
            }
            break;
        case 'A':
            if(MSG_DEBUG)
            {
                Serial.print("AnalogWrite pin3 is set to ");
                Serial.println(val);
            }
            analogWrite(3, (byte)val);
            break;
        case 'B':
            if(MSG_DEBUG)
            {
                Serial.print("AnalogWrite pin5 is set to ");
                Serial.println(val);
            }
            analogWrite(5, (byte)val);
            break;
        case 'C':
            if(MSG_DEBUG)
            {
                Serial.print("AnalogWrite pin6 is set to ");
                Serial.println(val);
            }
            analogWrite(6, (byte)val);
            break;
        case 'D':
            if(MSG_DEBUG)
            {
                Serial.print("AnalogWrite pin9 is set to ");
                Serial.println(val);
            }
            analogWrite(9, (byte)val);
            break;
        case 'E':
            if(MSG_DEBUG)
            {
                Serial.print("AnalogWrite pin10 is set to ");
                Serial.println(val);
            }
            analogWrite(10, (byte)val);
            break;
        case 'F':
            if(MSG_DEBUG)
            {
                Serial.print("AnalogWrite pin11 is set to ");
                Serial.println(val);
            }
            analogWrite(11, (byte)val);
            break;
        default:
            if(MSG_ERROR)
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
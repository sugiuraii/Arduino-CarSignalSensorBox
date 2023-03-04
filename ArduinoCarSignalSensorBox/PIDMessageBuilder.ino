#include "PIDMessageBuilder.h"

void fillValueBytes(byte* const returnBuf, const uint8_t requestedPID, const uint8_t byteOffsetToFill, uint8_t& byteOffsetAfterFill);
void fillAvailablePIDBytes(byte* const returnBuf, const uint8_t requestedPID, const uint8_t byteOffsetToFill, uint8_t& byteOffsetAfterFill);

int  buildPIDValueMessage(byte* const returnBuf, uint8_t& returnByteCount, const uint8_t* requestedPIDList, const uint8_t requestedPIDCount, const uint8_t returnServiceMode)
{
  returnBuf[0] = returnServiceMode;
  uint8_t byteOffset = 1;

  for(uint8_t i = 0; i < requestedPIDCount; i++)
  {
    const uint8_t requestedPID = requestedPIDList[i];
    if ((requestedPID % 0x20) == 0) // Return supported PID flag mode (=> get data from PIDAvailableFlagMap in PROGMEM)
      fillAvailablePIDBytes(returnBuf, requestedPID, byteOffset, byteOffset);
    else // Return value mode (=> get data from PID_Value_Map in RAM)
      fillValueBytes(returnBuf, requestedPID, byteOffset, byteOffset);
  }
  
  // Finally, set returnByteCount
  returnByteCount = byteOffset;

  // Check, at least one PID is available or not
  if(byteOffset == 2) // Offset is not changed
    return PID_NOT_AVAILABLE;
  else
    return NOERROR;
}

void fillValueBytes(byte* const returnBuf, const uint8_t requestedPID, const uint8_t byteOffsetToFill, uint8_t& byteOffsetAfterFill)
{
  int *analogReadVal = getAnalogReadVal();
  switch (requestedPID)
  {
  case 0x05: // PID 0x05 = Engine coolant temperature
  {
    int adcCoolant = analogReadVal[WATERTEMP_ADC_PIN];
    returnBuf[0] = 1 + 2; // Return 1byte
    returnBuf[byteOffsetToFill + 1] = convertToOBDCoolantTemperature(adcCoolant);
    return NOERROR;
  }
  break;
  case 0x0B: // PID 0x0B = Manofold absoulte pressure
  {
    int adcManifoldPres = analogReadVal[BOOST_ADC_PIN];
    returnBuf[0] = 1 + 2; // Return 1byte
    returnBuf[byteOffsetToFill + 1] = convertToOBDManifoldAbsPressure(adcManifoldPres);
    return NOERROR;
  }
  break;
  case 0x0C: // PID 0x0C = Engine speed (rpm)
  {
    unsigned long nowTime = micros();
    unsigned long rpmPulseTime = getTachoPulseElapsedTime(nowTime);
    uint16_t rpmOBDVal = convertToOBDEngineREVx4(rpmPulseTime);
    returnBuf[0] = 2 + 2; // Return 2byte
    returnBuf[byteOffsetToFill + 1] = (byte)((rpmOBDVal & 0xFF00) >> 8);
    returnBuf[byteOffsetToFill + 2] = (byte)((rpmOBDVal & 0x00FF));
    return NOERROR;
  }
  break;
  case 0x0D: // PID 0x0D = Vehicle speeds
  {
    unsigned long nowTime = micros();
    unsigned long vspeedPulseTime = getSpeedPulseElapsedTime(nowTime);
    byte speedOBDDVal = convertToVechicleOBDSpeed(vspeedPulseTime);
    returnBuf[0] = 1 + 2; // Return 2byte
    returnBuf[byteOffsetToFill + 1] = speedOBDDVal;
    return NOERROR;
  }
  break;
  case 0x5C: // PID 0x5C = Engine oil temperature
  {
    int adcOilTemp = analogReadVal[OILTEMP_ADC_PIN];
    returnBuf[0] = 1 + 2; // Return 1byte
    returnBuf[byteOffsetToFill + 1] = convertToOBDEngineOILTemperature(adcOilTemp);
    return NOERROR;
  }
  break;
  default:
    // Requested PID is not match
    return PID_NOT_AVAILABLE;
    break;
  }
}

// Build available PID message (AvailablePID = 0x05, 0x0B, 0x0C, 0x0D, 0x5C)
void fillAvailablePIDBytes(byte* const returnBuf, const uint8_t requestedPID, const uint8_t byteOffsetToFill, uint8_t& byteOffsetAfterFill)
{
  // Fill requested PID to 1st byte
  returnBuf[byteOffsetToFill] = requestedPID;
  switch (requestedPID)
  {
  case 0x00:
    returnBuf[byteOffsetToFill + 1] = 0x08;
    returnBuf[byteOffsetToFill + 2] = 0x38;
    returnBuf[byteOffsetToFill + 3] = 0x00;
    returnBuf[byteOffsetToFill + 4] = 0x01;
    break;
  case 0x20:
    returnBuf[byteOffsetToFill + 1] = 0x00;
    returnBuf[byteOffsetToFill + 2] = 0x00;
    returnBuf[byteOffsetToFill + 3] = 0x00;
    returnBuf[byteOffsetToFill + 4] = 0x01;
    break;
  case 0x40:
    returnBuf[byteOffsetToFill + 1] = 0x00;
    returnBuf[byteOffsetToFill + 2] = 0x00;
    returnBuf[byteOffsetToFill + 3] = 0x00;
    returnBuf[byteOffsetToFill + 4] = 0x10;
    break;
  default:
    returnBuf[byteOffsetToFill + 1] = 0x00;
    returnBuf[byteOffsetToFill + 2] = 0x00;
    returnBuf[byteOffsetToFill + 3] = 0x00;
    returnBuf[byteOffsetToFill + 4] = 0x00;
    if (CANMSG_ERROR)
      Serial.println(F("Invaild PID is requested on fillAvailablePIDBytes."));
  }
  byteOffsetAfterFill = byteOffsetToFill + 5;
}

{
    const uint8_t valByteLength = pgm_read_byte(PIDByteLengthMap + requestedPID);
    if (valByteLength == 0) // Skip build return byte when PID is not available
    {
      byteOffsetAfterFill = byteOffsetToFill;
      return;
    }

    // Fill requested PID to 1st byte
    returnBuf[byteOffsetToFill] = requestedPID;

    // Fill return value bytes
    const unsigned int PIDAddressOffset = pgm_read_word(PIDAddressMap + requestedPID);
    for (uint8_t i = 0; i < valByteLength; i++)
      returnBuf[byteOffsetToFill + 1 + i] = PID_Value_Map[PIDAddressOffset + i];

    // Finally, set byteOffseytAfterFill
    byteOffsetAfterFill = byteOffsetToFill + 1 + valByteLength;
}


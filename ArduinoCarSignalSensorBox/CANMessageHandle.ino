#include "CANMesasgeHandle.h"
#include "OBD2ValConvert.h"
#include "ADCRead.h"
#include "TachoSpeedPulseCounter.h"
#include "SensorADCPinAssign.h"

int buildPIDValueMessage(byte *returnBuf, uint8_t requestedPID);
void buildAvailablePIDMessage(byte *returnBuf, uint8_t requestedPID);

MCP_CAN CAN(10); // CAN CS: pin 10
constexpr int CAN_PAYLOAD_LENGTH = 8;

IsoTp isotp(&CAN, 0);
struct Message_t txMsg;

// ECU (this controller) CAN ID
constexpr unsigned long ECU_CAN_ID = 0x7E0;
constexpr unsigned long ECU_CAN_RESPONSE_ID = ECU_CAN_ID + 0x008;

// Debug message serial out switch
constexpr bool CANMSG_DEBUG = false;
constexpr bool CANMSG_ERROR = true;
constexpr bool CANMSG_FATAL = true;

// Output CAN handle time (for debug and performance analysis)
constexpr bool CANMSG_TIME_MEAS = false;

// Return code of buildPIDValueMessage
constexpr int NOERROR = 0;
constexpr int PID_NOT_AVAILABLE = -1;

void initializeCAN()
{
  bool initSucess = false;
  while (!initSucess)
  {
    if (CAN_OK == CAN.begin(MCP_ANY, CAN_250KBPS, MCP_8MHZ)) // init can bus : baudrate = 250k    
    {
      Serial.println(F("CAN BUS Shield init ok!"));
      initSucess = true;
    }
    else
    {
      Serial.println(F("CAN BUS Shield init fail"));
      Serial.println(F("Init CAN BUS Shield again"));
      delay(100);
      initSucess = false;
    }
  }
}

void handleCANMessage()
{
  unsigned long canMsgHandleStartTime;
  if (CANMSG_TIME_MEAS)
    canMsgHandleStartTime = micros();

  if (CANMSG_DEBUG)
    Serial.println(F("CAN message handle start."));

  byte canBuf[CAN_PAYLOAD_LENGTH];
  unsigned long canId;
  unsigned char len;

  CAN.readMsgBuf(&canId, &len, canBuf);

  if (len > CAN_PAYLOAD_LENGTH)
  {
    if (CANMSG_FATAL)
      Serial.println(F("FATAL: CAN read message length exceed CAN_PAYLOAD_LENGTH."));

    return;
  }
  if (CANMSG_DEBUG)
  {
    Serial.print(F("Msg from canId: "));
    Serial.println(canId, HEX);
  }

  // Ignore query if the ID do not match with this ECU ID (or 0x7DF(send to all ECU))
  if((canId != 0x7DF) && (canId != ECU_CAN_ID))
  {
    if (CANMSG_DEBUG)
      Serial.println(F("CAM ID do not match with this ECU's ID."));
    
    return;
  }

  const uint8_t queryMessageLength = canBuf[0];
  const uint8_t serviceMode = canBuf[1];
  if (queryMessageLength != 2)
  {
    if (CANMSG_ERROR)
      Serial.println(F("ERROR: CAN query message byte length is not 2."));

    return;
  }
  if (serviceMode != 0x01)
  {
    if (CANMSG_ERROR)
      Serial.println(F("ERROR: CAN query service mode needs to be 1 (show current data)."));

    return;
  }

  const uint8_t requestedPID = canBuf[2];
  if (CANMSG_DEBUG)
  {
    Serial.print(F("PID query: "));
    Serial.println(requestedPID, HEX);
  }

  // Build up CAN return message
  const uint8_t returnServiceMode = serviceMode + 0x40;
  byte returnBuf[8] = {0x00, returnServiceMode, requestedPID, 0x00, 0x00, 0x00, 0x00, 0x00};

  if ((requestedPID % 0x20) == 0) // Return supported PID flag mode
    buildAvailablePIDMessage(returnBuf, requestedPID);
  else // Return value mode (=> get data from PID_Value_Map in RAM)
  {
    int pidValMessageResult = buildPIDValueMessage(returnBuf, requestedPID);
    if (pidValMessageResult == PID_NOT_AVAILABLE)
    {
      if (CANMSG_ERROR)
        Serial.println(F("ERROR: CAN query PID is not supported."));
      return;
    }
  }

  // Send CAN return message.
  CAN.sendMsgBuf(ECU_CAN_RESPONSE_ID, 0, 8, returnBuf);

  if(CANMSG_TIME_MEAS)
  {
    Serial.print(F("CAN message handle time (micros): "));
    Serial.println(micros() - canMsgHandleStartTime);
  }
  
  if (CANMSG_DEBUG)
  {
    Serial.print(F("Return byte length: "));
    Serial.println(returnBuf[0], DEC);
    Serial.print(F("Return service mode: "));
    Serial.println(returnBuf[1], HEX);
    Serial.print(F("Return PID: "));
    Serial.println(returnBuf[2], HEX);
    Serial.print(F("Return. Value (with padding): "));
    for (int i = 3; i < 8; i++)
    {
      Serial.print(returnBuf[i], HEX);
      if (i == 7)
        Serial.println();
      else
        Serial.print(",");
    }
  }
}

int buildPIDValueMessage(byte *returnBuf, uint8_t requestedPID)
{
  int *analogReadVal = getAnalogReadVal();
  switch (requestedPID)
  {
  case 0x05: // PID 0x05 = Engine coolant temperature
  {
    int adcCoolant = analogReadVal[WATERTEMP_ADC_PIN];
    returnBuf[0] = 1 + 2; // Return 1byte
    returnBuf[3] = convertToOBDCoolantTemperature(adcCoolant);
    return NOERROR;
  }
  break;
  case 0x0B: // PID 0x0B = Manofold absoulte pressure
  {
    int adcManifoldPres = analogReadVal[BOOST_ADC_PIN];
    returnBuf[0] = 1 + 2; // Return 1byte
    returnBuf[3] = convertToOBDManifoldAbsPressure(adcManifoldPres);
    return NOERROR;
  }
  break;
  case 0x0C: // PID 0x0C = Engine speed (rpm)
  {
    unsigned long nowTime = micros();
    unsigned long rpmPulseTime = getTachoPulseElapsedTime(nowTime);
    uint16_t rpmOBDVal = convertToOBDEngineREVx4(rpmPulseTime);
    returnBuf[0] = 2 + 2; // Return 2byte
    returnBuf[3] = (byte)((rpmOBDVal & 0xFF00) >> 8);
    returnBuf[4] = (byte)((rpmOBDVal & 0x00FF));
    return NOERROR;
  }
  break;
  case 0x0D: // PID 0x0D = Vehicle speeds
  {
    unsigned long nowTime = micros();
    unsigned long vspeedPulseTime = getSpeedPulseElapsedTime(nowTime);
    byte speedOBDDVal = convertToVechicleOBDSpeed(vspeedPulseTime);
    returnBuf[0] = 1 + 2; // Return 2byte
    returnBuf[3] = speedOBDDVal;
    return NOERROR;
  }
  break;
  case 0x5C: // PID 0x5C = Engine oil temperature
  {
    int adcOilTemp = analogReadVal[OILTEMP_ADC_PIN];
    returnBuf[0] = 1 + 2; // Return 1byte
    returnBuf[3] = convertToOBDEngineOILTemperature(adcOilTemp);
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
void buildAvailablePIDMessage(byte *returnBuf, uint8_t requestedPID)
{
  const byte valByteLength = 4;
  const byte returnByteLength = valByteLength + 2;
  returnBuf[0] = returnByteLength;
  switch (requestedPID)
  {
  case 0x00:
    returnBuf[3] = 0x08;
    returnBuf[4] = 0x38;
    returnBuf[5] = 0x00;
    returnBuf[6] = 0x01;
    break;
  case 0x20:
    returnBuf[3] = 0x00;
    returnBuf[4] = 0x00;
    returnBuf[5] = 0x00;
    returnBuf[6] = 0x01;
    break;
  case 0x40:
    returnBuf[3] = 0x00;
    returnBuf[4] = 0x00;
    returnBuf[5] = 0x00;
    returnBuf[6] = 0x10;
    break;
  default:
    returnBuf[3] = 0x00;
    returnBuf[4] = 0x00;
    returnBuf[5] = 0x00;
    returnBuf[6] = 0x00;
    if (CANMSG_ERROR)
      Serial.println(F("Invaild PID is requested on buildAvailanblePIDMessage."));
  }
}
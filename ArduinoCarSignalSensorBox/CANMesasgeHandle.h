#ifndef CANMESSAGE_HANDLE_H
#define CANMESSAGE_HANDLE_H

#include "mcp_can.h"
#include "mcp_can_dfs.h"
#include <SPI.h>
#include <avr/pgmspace.h>
#include <iso-tp.h>

#include "PIDMessageBuilder.h"

void initializeCAN();
void handleCANMessage();

extern MCP_CAN CAN;

// ECU (this controller) CAN ID
constexpr unsigned long ECU_CAN_ID = 0x7E0;
constexpr unsigned long ECU_CAN_RESPONSE_ID = ECU_CAN_ID + 0x008;

// ECU response(reply wait in ms)
constexpr int ECU_WAIT = 0; 

// Debug message serial out switch
constexpr bool PIDSET_DEBUG = false;
constexpr bool PIDSET_ERROR = true;
constexpr bool CANMSG_DEBUG = false;
constexpr bool CANMSG_TIME_MEAS = false;
constexpr bool CANMSG_FREERAM_MEAS = false;
constexpr bool CANMSG_ERROR = true;
constexpr bool CANMSG_FATAL = true;

//constexpr int SERIAL_MSG_LENGTH = 11;
constexpr int CAN_PAYLOAD_LENGTH = 8;

#endif

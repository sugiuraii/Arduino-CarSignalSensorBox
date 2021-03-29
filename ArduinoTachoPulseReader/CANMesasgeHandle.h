#ifndef CANMESSAGE_HANDLE_H
#define CANMESSAGE_HANDLE_H

#include "mcp2515_can.h"

void initializeCAN();
void handleCANMessage();

constexpr int CAN_PAYLOAD_LENGTH = 8;
extern mcp2515_can CAN;

#endif

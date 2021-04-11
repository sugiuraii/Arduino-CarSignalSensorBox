#ifndef CANMESSAGE_HANDLE_H
#define CANMESSAGE_HANDLE_H

#include "mcp2515_can.h"

void initializeCAN();
void handleCANMessage();

extern mcp2515_can CAN;

#endif

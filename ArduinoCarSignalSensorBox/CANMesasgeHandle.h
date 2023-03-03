#ifndef CANMESSAGE_HANDLE_H
#define CANMESSAGE_HANDLE_H

#include "mcp_can.h"
#include "mcp_can_dfs.h"
#include <iso-tp.h>

void initializeCAN();
void handleCANMessage();

extern MCP_CAN CAN;

#endif

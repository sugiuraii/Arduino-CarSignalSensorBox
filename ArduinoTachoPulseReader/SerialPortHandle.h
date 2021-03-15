#ifndef SERIAL_PORT_HANDLE_H
#define SERIAL_PORT_HANDLE_H

void setupSerialPort();
void sendSerialMsg(unsigned long speedPulseElapsedTime, unsigned long tachoPulseElapsedTime, int *analogReadVal, int numAnalogReadVals);

#endif
# Arduino-CarSignalSensorBox for WebSocketGaugeServer

## Setup for [WebSocketGaugeServer](https://github.com/sugiuraii/WebSocketGaugeServer)

To use ArduinoCarSignalSensorBox for [WebSocketGaugeServer](https://github.com/sugiuraii/WebSocketGaugeServer), write this sketch after enabling Serial dump mode.
* See https://github.com/sugiuraii/Arduino-CarSignalSensorBox#serial-dump-mode

1. Clone or download sketch source.
2. Modify `ArduinoCarSignalSensorBox/ArduinoCarSignalSensorBox.h` as,
```c++
#ifndef ARDUINO_CARSIGNAL_SENSOR_BOX_H
#define ARDUINO_CARSIGNAL_SENSOR_BOX_H

constexpr bool SERIAL_DUMP_ENABLE = true; // Set here to true
constexpr bool SERIAL_INTERACTIVE_ENABLE = false; // Set false
constexpr bool CAN_OBD_ENABLE = false;  // Set false

#endif
```
3. Write sketch to Arduino board.


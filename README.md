# Arduino Pulse counter / ADC reader

##Table of contents
* [Description](#description)
* [System diagram](#system_diagram)
* [Requirement](#requirement)
* [Install](#install)

## <a name="description"> Description</a>
This program (arduino sketch) measure pulse interval, read ADC and send the pulse interval and ADC reading to PC via serial port (on USB).

This program can be used with [ArduinoCOM_Websocket_Server](https://github.com/sugiuraii/DefiSSMCOM_WebsocketServer).

The pulse counter/adc reader program consists of only 1 sketch file of `ArduinoTachoPulseReader/ArduinoTachoPulseReader.ino`.

(`debugTool/ArduinoTachoPulseGenerator/ArduinoTachoPulseGenerator.ino` is debug tool to generate pulse. This can bes used to check the operation of `ArduinoTachoPulseReader.ino`).

Please note that the this program simply send pulse 

## <a name="system_diagram"> System diagram </a>
![WebsocketDiagram](README.img/WebsocketDashboardArduino.png)

## <a name="requirement"> Requirement </a>
* Arduino UNO or compatible board.
* Arduino IDE

## <a name="install">Install</a>
Simply compile `ArduinoTachoPulseReader/ArduinoTachoPulseReader.ino`



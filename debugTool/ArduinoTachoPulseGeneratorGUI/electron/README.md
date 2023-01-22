# Electron project for ArduinoTachoPulseGeneratorGUI
This directory contains sources to build ArduinoTachoPulseGeneratorGUI app as desktop app, by using electron.

# Before running this.
Before running this, please build backend asp.net binary (root of this project), and copy backend binary files to `server-bin`.

```
cd ArduinoTachoPulseGeneratorGUI
dotnet publish
cd electron
mkdir server-bin
cd server-bin
cp -r ../bin/Debug/net6.0/publish/* ./
```

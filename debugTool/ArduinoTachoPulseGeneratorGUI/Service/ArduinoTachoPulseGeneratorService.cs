using System;
using System.Collections.Generic;
using System.Linq;
using System.IO.Ports;
using System.Text;
using Microsoft.Extensions.Logging;
using System.IO;

namespace ArduinoTachoPulseGeneratorGUI.Service
{
    public enum WriteValueType
    {
        Tacho,
        Speed,
        Analog3,
        Analog5,
        Analog6,
        Analog9,
        Analog10,
        Analog11
    }

    public class ArduinoTachoPulseGeneratorService : IDisposable
    {
        private readonly ILogger<ArduinoTachoPulseGeneratorService> logger;
        public event EventHandler<bool> CommunicateStateChanged;
        public event EventHandler<Exception> CommunicateErrorOccured;
        private bool RunningState = false;
        private SerialPort serialPort;

        public ArduinoTachoPulseGeneratorService(ILogger<ArduinoTachoPulseGeneratorService> logger)
        {
            this.logger = logger;
        }

        public void Dispose()
        {
            CommunicateStop();
        }

        public void CommunicateStart(string portName)
        {
            try
            {
                logger.LogInformation("Serial port open.");
                logger.LogInformation("Portname : " + portName);
                serialPort = new SerialPort(portName);
                serialPort.BaudRate = 9600;
                serialPort.NewLine = "\n";
                serialPort.Open();
                logger.LogInformation("BaudRate is " + serialPort.BaudRate.ToString());

                WaitReceivingECUSimReady();

                // Enable SerialPort input logging
                serialPort.DataReceived += (sender, e) =>
                {
                    var port = (SerialPort)sender;
                    string message = port.ReadExisting();
                    if (message.Contains("error", StringComparison.OrdinalIgnoreCase))
                        logger.LogWarning("Error message from Arduino : " + message);
                    else
                        logger.LogDebug("Message from Arduino : " + message);
                };

                logger.LogInformation("Communication started.");
                this.RunningState = true;
                if (CommunicateStateChanged != null)
                    CommunicateStateChanged(this, RunningState);

            }
            catch (Exception ex) when ((ex is IOException) || (ex is UnauthorizedAccessException) || (ex is ArgumentException) || (ex is InvalidOperationException))
            {
                logger.LogError(ex.Message);
                CommunicateErrorOccured(this, ex);
            }
        }

        private void WaitReceivingECUSimReady()
        {
            logger.LogInformation("Waiting initialize OK message....");
            while (true)
            {
                string response = serialPort.ReadLine();
                if (response.Contains("Initialize ok!"))
                {
                    logger.LogInformation("Arduino initialize ok message is received.");
                    break;
                }
                if (response.Contains("error", StringComparison.OrdinalIgnoreCase) || response.Contains("fail", StringComparison.OrdinalIgnoreCase))
                {
                    throw new InvalidOperationException("Arduino initialization is failed. Response is :" + response);
                }
            }
        }

        public void CommunicateStop()
        {
            logger.LogInformation("Serial port closing.");
            if (serialPort != null)
            {
                serialPort.Close();
                serialPort.Dispose();
            }
            this.RunningState = false;
            if (CommunicateStateChanged != null)
                CommunicateStateChanged(this, RunningState);
            logger.LogInformation("Serial port closed.");
        }

        public uint GetMaxValue(WriteValueType type)
        {
            if(type == WriteValueType.Tacho)
                return 32768;
            else if(type == WriteValueType.Speed)
                return 255;
            else
                return 255;
        }
        public void WriteValue(WriteValueType type, uint value)
        {
            if(this.RunningState == false || serialPort == null)
                return;

            switch (type)
            {
                case WriteValueType.Tacho:
                    if (value > 65535)
                        throw new ArgumentException(type.ToString() + " value out of range.");
                    serialPort.Write("T" + value.ToString("D5"));
                    break;
                case WriteValueType.Speed:
                    if (value > 65535)
                        throw new ArgumentException(type.ToString() + " value out of range.");
                    serialPort.Write("S" + value.ToString("D5"));
                    break;
                case WriteValueType.Analog3:
                    if (value > 255)
                        throw new ArgumentException(type.ToString() + " value out of range.");
                    serialPort.Write("A" + value.ToString("D5"));
                    break;
                case WriteValueType.Analog5:
                    if (value > 255)
                        throw new ArgumentException(type.ToString() + " value out of range.");
                    serialPort.Write("B" + value.ToString("D5"));
                    break;
                case WriteValueType.Analog6:
                    if (value > 255)
                        throw new ArgumentException(type.ToString() + " value out of range.");
                    serialPort.Write("C" + value.ToString("D5"));
                    break;
                case WriteValueType.Analog9:
                    if (value > 255)
                        throw new ArgumentException(type.ToString() + " value out of range.");
                    serialPort.Write("D" + value.ToString("D5"));
                    break;
                case WriteValueType.Analog10:
                    if (value > 255)
                        throw new ArgumentException(type.ToString() + " value out of range.");
                    serialPort.Write("E" + value.ToString("D5"));
                    break;
                case WriteValueType.Analog11:
                    if (value > 255)
                        throw new ArgumentException(type.ToString() + " value out of range.");
                    serialPort.Write("F" + value.ToString("D5"));
                    break;
            }
        }
    }
}

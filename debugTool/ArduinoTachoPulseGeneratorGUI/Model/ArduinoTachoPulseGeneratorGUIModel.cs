using System;
using System.ComponentModel;
using Microsoft.Extensions.Logging;
using Reactive.Bindings;
using System.Reactive.Linq;
using ArduinoTachoPulseGeneratorGUI.Service;

namespace ArduinoTachoPulseGeneratorGUI.Model
{
    public class ArduinoTachoPulseGeneratorGUIModel : ReactivePropertyBlazorModelBase, IDisposable
    {
        private readonly ILogger logger;
        private readonly ArduinoTachoPulseGeneratorService Service;
        public event EventHandler<Exception> CommunicateErrorOccured;
        public ReactivePropertySlim<string> COMPortName { get; private set; }
        public ReactivePropertySlim<bool> StartButtonEnabled { get; private set; }
        public ReadOnlyReactivePropertySlim<bool> StopButtonEnabled { get; private set; }

        public ReactivePropertySlim<uint> TachoValue { get; set; }
        public uint TachoMax { get; private set; }
        public ReactivePropertySlim<uint> SpeedValue { get; set; }
        public uint SpeedMax { get; private set; }
        public ReactivePropertySlim<uint> Analog3Value { get; set; }
        public uint Analog3Max { get; private set; }
        public ReactivePropertySlim<uint> Analog5Value { get; set; }
        public uint Analog5Max { get; private set; }
        public ReactivePropertySlim<uint> Analog6Value { get; set; }
        public uint Analog6Max { get; private set; }
        public ReactivePropertySlim<uint> Analog9Value { get; set; }
        public uint Analog9Max { get; private set; }
        public ReactivePropertySlim<uint> Analog10Value { get; set; }
        public uint Analog10Max { get; private set; }
        public ReactivePropertySlim<uint> Analog11Value { get; set; }
        public uint Analog11Max { get; private set; }

        public ReactiveCommand StartCommand { get; private set; }
        public ReactiveCommand StopCommand { get; private set; }
        public ArduinoTachoPulseGeneratorGUIModel(ArduinoTachoPulseGeneratorService serivce, ILogger<ArduinoTachoPulseGeneratorGUIModel> logger)
        {
            this.logger = logger;
            this.Service = serivce;

            this.COMPortName = GetDefaultReactivePropertySlim<string>("/dev/ttyUSB0", "COMPortName");
            this.StartButtonEnabled = GetDefaultReactivePropertySlim<bool>(true, "StartButtonEnabled");
            this.StopButtonEnabled = this.StartButtonEnabled.Select(v => !v).ToReadOnlyReactivePropertySlim();

            this.TachoValue = GetDefaultReactivePropertySlim<uint>(0, "TachoValue");
            this.TachoValue.Subscribe(v => Service.WriteValue(WriteValueType.Tacho, v));
            this.TachoMax = Service.GetMaxValue(WriteValueType.Tacho);
            this.SpeedValue = GetDefaultReactivePropertySlim<uint>(0, "SpeedValue");
            this.SpeedValue.Subscribe(v => Service.WriteValue(WriteValueType.Speed, v));
            this.SpeedMax = Service.GetMaxValue(WriteValueType.Speed);
            this.Analog3Value = GetDefaultReactivePropertySlim<uint>(0, "Analog3Value");
            this.Analog3Value.Subscribe(v => Service.WriteValue(WriteValueType.Analog3, v));
            this.Analog3Max = Service.GetMaxValue(WriteValueType.Analog3);
            this.Analog5Value = GetDefaultReactivePropertySlim<uint>(0, "Analog5Value");
            this.Analog5Value.Subscribe(v => Service.WriteValue(WriteValueType.Analog5, v));
            this.Analog5Max = Service.GetMaxValue(WriteValueType.Analog5);
            this.Analog6Value = GetDefaultReactivePropertySlim<uint>(0, "Analog6Value");
            this.Analog6Value.Subscribe(v => Service.WriteValue(WriteValueType.Analog6, v));
            this.Analog6Max = Service.GetMaxValue(WriteValueType.Analog6);
            this.Analog9Value = GetDefaultReactivePropertySlim<uint>(0, "Analog9Value");
            this.Analog9Value.Subscribe(v => Service.WriteValue(WriteValueType.Analog9, v));
            this.Analog9Max = Service.GetMaxValue(WriteValueType.Analog9);
            this.Analog10Value = GetDefaultReactivePropertySlim<uint>(0, "Analog10Value");
            this.Analog10Value.Subscribe(v => Service.WriteValue(WriteValueType.Analog10, v));
            this.Analog10Max = Service.GetMaxValue(WriteValueType.Analog10);
            this.Analog11Value = GetDefaultReactivePropertySlim<uint>(0, "Analog11Value");
            this.Analog11Value.Subscribe(v => Service.WriteValue(WriteValueType.Analog11, v));
            this.Analog11Max = Service.GetMaxValue(WriteValueType.Analog11);

            this.StartCommand = StartButtonEnabled.ToReactiveCommand(); // Can run only on StartButtonEnabled = true;
            this.StartCommand.Subscribe(() => Service.CommunicateStart(COMPortName.Value));
            this.StopCommand = StopButtonEnabled.ToReactiveCommand(); // Can run only on StopButtonEnabled = true;
            this.StopCommand.Subscribe(() => Service.CommunicateStop());

            Service.CommunicateStateChanged += (sender, arg) => StartButtonEnabled.Value = !arg;
            Service.CommunicateErrorOccured += (sender, arg) => this.CommunicateErrorOccured(sender, arg);
        }

        public void Dispose()
        {
            this.COMPortName.Dispose();
            this.StartButtonEnabled.Dispose();
            this.StopButtonEnabled.Dispose();
            this.StartCommand.Dispose();
            this.StopCommand.Dispose();
            this.TachoValue.Dispose();
            this.SpeedValue.Dispose();
            this.Analog3Value.Dispose();
            this.Analog5Value.Dispose();
            this.Analog6Value.Dispose();
            this.Analog9Value.Dispose();
            this.Analog10Value.Dispose();
            this.Analog11Value.Dispose();
        }
    }
}
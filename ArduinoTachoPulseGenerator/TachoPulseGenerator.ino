//Define number of pulse in TACHO and SPEED
#define PULSE_PER_TACHO_REV 2
#define PULSE_PER_SPEED_REV 4

//Define serial statusout baudrate
#define Serial_Baudrate 9600

//Define PIN
#define speedPin 2
#define tachoPin 3
#define speedAnalogVolumePin 0
#define tachoAnalogVolumePin 1

//Define Analog read interval in microsecond
#define AnalogReadInterval 1000000UL

int speed = 0;
int tacho = 0;
unsigned long speedPulseInterval = 0;
unsigned long tachoPulseInterval = 0;

//Timer counter for pulse
unsigned long speedPulseBeforeTime = 0;
unsigned long tachoPulseBeforeTime = 0;
unsigned long analogReadBeforeTime = 0;
int speedPinState = LOW;
int tachoPinState = LOW;

//Read analog input and set speed/tacho
void analogReadAndSetSpeedTacho()
{
  //Set speed
  speed = map(analogRead(speedAnalogVolumePin), 0, 1023, 1, 300);
  //speedPulseInterval = 3600/(637*speed*PULSE_PER_SPEED_REV) * 1E6;
  speedPulseInterval = 5651491UL/(speed*PULSE_PER_SPEED_REV);
  speedPulseInterval /= 2;
  
  //Set tacho
  tacho = map(analogRead(tachoAnalogVolumePin), 0, 1023, 1, 10000);
  //tachoPulseInterval = 60/(PULSE_PER_TACHO_REV*tacho) * 1E6;
  tachoPulseInterval = 60000000UL/(PULSE_PER_TACHO_REV*tacho);
  
  tachoPulseInterval /= 2; 
}

//Out current tacho/speed to serialPort
void serialOut(int _speed, int _tacho)
{
  Serial.print("Speed:");
  Serial.print(_speed);
  Serial.print(" Tacho:");
  Serial.println(_tacho);
}

// the setup routine runs once when you press reset:
void setup() {
  // initialize the digital pin as an output.
  pinMode(speedPin, OUTPUT);
  pinMode(tachoPin, OUTPUT);
  
  Serial.begin(Serial_Baudrate);
  
  analogReadAndSetSpeedTacho();
  serialOut(speed, tacho);
  digitalWrite(speedPin, speedPinState);
  digitalWrite(tachoPin, tachoPinState);

}

// the loop routine runs over and over again forever:
void loop() {
  unsigned long nowTime;
  nowTime = micros();
  
  unsigned long speedPulseElapsedTime;
  unsigned long tachoPulseElapsedTime;
  unsigned long analogReadElapsedTime;
  
  //AnalogRead and set pulseInterval
  if(nowTime > analogReadBeforeTime )
    analogReadElapsedTime = nowTime - analogReadBeforeTime;
  else
    analogReadElapsedTime = nowTime + 4294967295UL - analogReadBeforeTime;
  if(analogReadElapsedTime > AnalogReadInterval) 
  {
    analogReadAndSetSpeedTacho();
    serialOut(speed, tacho);
    analogReadBeforeTime = nowTime;
  }
  
  //Set speed pin state
  if(nowTime > speedPulseBeforeTime )
    speedPulseElapsedTime = nowTime - speedPulseBeforeTime;
  else
    speedPulseElapsedTime = nowTime + 4294967295UL - speedPulseBeforeTime;
  
  if(speedPulseElapsedTime > speedPulseInterval) 
  {
    if(speedPinState == LOW)
    {
      speedPinState = HIGH;
      digitalWrite(speedPin, speedPinState);
    }
    else
    {
      speedPinState = LOW;
      digitalWrite(speedPin, speedPinState);
    }
    speedPulseBeforeTime = nowTime;
  }
  
  //Set tacho pin state
  if(nowTime > tachoPulseBeforeTime )
    tachoPulseElapsedTime = nowTime - tachoPulseBeforeTime;
  else
    tachoPulseElapsedTime = nowTime + 4294967295UL - tachoPulseBeforeTime;
  
  if(tachoPulseElapsedTime > tachoPulseInterval) 
  {
    if(tachoPinState == LOW)
    {
      tachoPinState = HIGH;
      digitalWrite(tachoPin, tachoPinState);
    }
    else
    {
      tachoPinState = LOW;
      digitalWrite(tachoPin, tachoPinState);
    }
    tachoPulseBeforeTime = nowTime;
  }
}


//communication interval in microsecond.
#define COMM_INTERVAL 16000
//Speed,Tacho pulse reset time (in microsecond)
#define SPEED_PULSE_EXPIRE_TIME 500000
#define TACHO_PULSE_EXPIRE_TIME 500000

//AnalogRead Oversampling count (1:No oversample)
#define OVERSAMPLE_COUNT 4

int speedPin = 0;
volatile unsigned long speedPulseBeforeTime=0;
volatile unsigned long speedPulseElapsedTime=0;

int tachoPin = 1;
volatile unsigned long tachoPulseBeforeTime=0;
volatile unsigned long tachoPulseElapsedTime=0;

int analogChannel=0;
int analogReadVal[6];

unsigned long commBeforeTime = 0;

// the setup routine runs once when you press reset:
void setup() {
  pinMode(speedPin, INPUT_PULLUP);
  pinMode(tachoPin, INPUT_PULLUP);
  
  attachInterrupt(speedPin, interruptSpeedPulse, RISING);
  attachInterrupt(tachoPin, interruptTachoPulse, RISING);

  //SerialPort setting
  Serial.begin(38400);  
}

void interruptSpeedPulse()
{
  unsigned long nowTime;
  nowTime = micros();
  if(nowTime < speedPulseBeforeTime) //Case of micors() overflow
    speedPulseElapsedTime = nowTime + 4294967295UL - speedPulseBeforeTime;
  else
    speedPulseElapsedTime = nowTime - speedPulseBeforeTime;
    
  speedPulseBeforeTime = nowTime;
}

void interruptTachoPulse()
{
  unsigned long nowTime;
  nowTime = micros();
  if(nowTime < tachoPulseBeforeTime) //Case of micors() overflow
  tachoPulseElapsedTime = nowTime + 4294967295UL - tachoPulseBeforeTime;
  else
    tachoPulseElapsedTime = nowTime - tachoPulseBeforeTime;
  
  tachoPulseBeforeTime = nowTime;
}


// the loop routine runs over and over again forever:
void loop() {
  int i,j;  
  for(i = 0; i <=5; i++)
  {
    analogReadVal[i] = 0;
    for(j = 0; j < OVERSAMPLE_COUNT; j++)
      analogReadVal[i] += analogRead(i);
  }
  
  unsigned long nowTime;
  unsigned long commElapsedTime;
  nowTime = micros();
  if(nowTime < commBeforeTime)
    commElapsedTime = nowTime + 4294967295UL - commBeforeTime;
  else
    commElapsedTime = nowTime - commBeforeTime;

  //Send serial
  if(commElapsedTime > COMM_INTERVAL)
  {
    //Overwrite elapsed time if no pulse is detected in 1sec
    if((nowTime - speedPulseBeforeTime) > SPEED_PULSE_EXPIRE_TIME)
      speedPulseElapsedTime = 0;
    if((nowTime - tachoPulseBeforeTime) > TACHO_PULSE_EXPIRE_TIME)
      tachoPulseElapsedTime = 0;
    
    send();
    commBeforeTime = nowTime;
  }
  
}

void send(){
  int i;
  unsigned int speedPulseCycle=0;
  unsigned int tachoPulseCycle=0;
  speedPulseCycle = speedPulseElapsedTime;
  tachoPulseCycle = tachoPulseElapsedTime;


  //Send
  Serial.print("SP");
  Serial.print(speedPulseCycle);
  Serial.print(" TA");
  Serial.print(tachoPulseCycle);
  for(i = 0; i<=5; i++)
  {
    Serial.print(" A");
    Serial.print(i);
    Serial.print(analogReadVal[i]);
  }
  Serial.print("\n");
}


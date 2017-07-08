/*
  BSD 3-Clause License
  
  Copyright (c) 2017, sz2 <sugiura.k.nigo@gmail.com>
  All rights reserved.
  
  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:
  
  * Redistributions of source code must retain the above copyright notice, this
    list of conditions and the following disclaimer.
  
  * Redistributions in binary form must reproduce the above copyright notice,
    this list of conditions and the following disclaimer in the documentation
    and/or other materials provided with the distribution.
  
  * Neither the name of the copyright holder nor the names of its
    contributors may be used to endorse or promote products derived from
    this software without specific prior written permission.
  
  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


//communication interval in microsecond.
#define COMM_INTERVAL 16600
//Speed,Tacho pulse reset time (in microsecond)
#define SPEED_PULSE_EXPIRE_TIME 1000000
#define TACHO_PULSE_EXPIRE_TIME 1000000

//Serial baudrate
//#define SERIAL_BAUD_RATE 9600
#define SERIAL_BAUD_RATE 38400

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
  Serial.begin(SERIAL_BAUD_RATE);  
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
  Serial.print("S");
  Serial.print(speedPulseCycle);
  Serial.print("\nT");
  Serial.print(tachoPulseCycle);
  for(i = 0; i<=5; i++)
  {
    Serial.print("\n");
    char tagLetter = 'A' + i;
    Serial.print(tagLetter);
    Serial.print(analogReadVal[i]);
  }
  Serial.print("\n");
}

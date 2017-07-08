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

//Define number of pulse in TACHO and SPEED
#define PULSE_PER_TACHO_REV 2
#define PULSE_PER_SPEED_REV 4

//Define serial statusout baudrate
#define Serial_Baudrate 9600

//Define PIN
#define speedPin 3
#define tachoPin 2
#define speedAnalogVolumePin 0
#define tachoAnalogVolumePin 1

//Define Analog read interval in microsecond
#define AnalogReadInterval 200000UL

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
  speed = map(analogRead(speedAnalogVolumePin), 0, 1023, 0, 300);
  if(speed != 0 )
    //speedPulseInterval = 3600/(637*speed*PULSE_PER_SPEED_REV) * 1E6;
    speedPulseInterval = 5651491UL/(speed*PULSE_PER_SPEED_REV);
  else
    speedPulseInterval = 0;
  
  speedPulseInterval /= 2;
  
  //Set tacho
  tacho = map(analogRead(tachoAnalogVolumePin), 0, 1023, 0, 10000);
  //tachoPulseInterval = 60/(PULSE_PER_TACHO_REV*tacho) * 1E6;
  if(tacho != 0)
    tachoPulseInterval = 60000000UL/(PULSE_PER_TACHO_REV*tacho);
  else
    tachoPulseInterval = 0;
  
  tachoPulseInterval /= 2; 
}

//Out current tacho/speed to serialPort
void serialOut()
{
  Serial.print("Speed:");
  Serial.print(speed);
  Serial.print(" Tacho:");
  Serial.println(tacho);
}

// the setup routine runs once when you press reset:
void setup() {
  // initialize the digital pin as an output.
  pinMode(speedPin, OUTPUT);
  pinMode(tachoPin, OUTPUT);
  
  Serial.begin(Serial_Baudrate);
  
  analogReadAndSetSpeedTacho();
  serialOut();
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
    serialOut();
    analogReadBeforeTime = nowTime;
  }
  
  //Set speed pin state
  if(nowTime > speedPulseBeforeTime )
    speedPulseElapsedTime = nowTime - speedPulseBeforeTime;
  else
    speedPulseElapsedTime = nowTime + 4294967295UL - speedPulseBeforeTime;
  if(speedPulseElapsedTime > speedPulseInterval) 
  {
    if(speedPulseInterval != 0) //Speed is not zero
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
    if(tachoPulseInterval != 0)
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
    }
    tachoPulseBeforeTime = nowTime;
  }
}


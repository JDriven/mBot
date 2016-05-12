#include <Servo.h>
#include <Wire.h>
#include <SoftwareSerial.h>
#include "mCore.h"
MeRGBLed rgb;
MeUltrasonic ultr(PORT_3);
MeLineFollower line(PORT_2);
MeLEDMatrix ledMx;
MeIR ir;
MeBuzzer buzzer;
MeTemperature ts;
Me7SegmentDisplay seg;

MeDCMotor MotorL(M1);
MeDCMotor MotorR(M2);
MePort generalDevice;
Servo servo;

int moveSpeed = 200;
int minSpeed = 48;
int factor = 23;

#define NTD1 294
#define NTD2 330
#define NTD3 350
#define NTD4 393
#define NTD5 441
#define NTD6 495
#define NTD7 556
#define NTDL1 147
#define NTDL2 165
#define NTDL3 175
#define NTDL4 196
#define NTDL5 221
#define NTDL6 248
#define NTDL7 278
#define NTDH1 589
#define NTDH2 661
#define NTDH3 700
#define NTDH4 786
#define NTDH5 882
#define NTDH6 990
#define NTDH7 112


#define RUN_F 0x01
#define RUN_B 0x01<<1
#define RUN_L 0x01<<2
#define RUN_R 0x01<<3
#define STOP 0
uint8_t motor_sta = STOP;
enum
{
  MODE_A,
  MODE_B,
  MODE_C
};

typedef struct MeModule
{
    int device;
    int port;
    int slot;
    int pin;
    int index;
    float values[3];
} MeModule;

union{
    byte byteVal[4];
    float floatVal;
    long longVal;
}val;

union{
  byte byteVal[8];
  double doubleVal;
}valDouble;

union{
  byte byteVal[2];
  short shortVal;
}valShort;

MeModule modules[12];
int analogs[8]={A0,A1,A2,A3,A4,A5,A6,A7};
uint8_t mode = MODE_A;

boolean isAvailable = false;
int len = 52;
char buffer[52];
char bufferBt[52];
byte index = 0;
byte dataLen;
byte modulesLen=0;
boolean isStart = false;
char serialRead;
String mVersion = "1.2.103";
float angleServo = 90.0;
unsigned char prevc=0;
boolean buttonPressed = false;
double lastTime = 0.0;
double currentTime = 0.0;
int LineFollowFlag=0;

#define VERSION 0
#define ULTRASONIC_SENSOR 1
#define TEMPERATURE_SENSOR 2
#define LIGHT_SENSOR 3
#define POTENTIONMETER 4
#define JOYSTICK 5
#define GYRO 6
#define SOUND_SENSOR 7
#define RGBLED 8
#define SEVSEG 9
#define MOTOR 10
#define SERVO 11
#define ENCODER 12
#define IR 13
#define IRREMOTE 14
#define PIRMOTION 15
#define INFRARED 16
#define LINEFOLLOWER 17
#define IRREMOTECODE 18
#define SHUTTER 20
#define LIMITSWITCH 21
#define BUTTON 22
#define DIGITAL 30
#define ANALOG 31
#define PWM 32
#define SERVO_PIN 33
#define TONE 34
#define BUTTON_INNER 35
#define LEDMATRIX 41
#define TIMER 50

#define GET 1
#define RUN 2
#define RESET 4
#define START 5


void setup()
{
  Serial.begin(115200);
  Stop();
  disco();
}

void  disco()
{
  rgb.setNumber(16);
  rgb.clear();
  rgb.setColor(10, 0, 0);
  buzzer.tone(NTD1, 300); 
  delay(30);
  rgb.setColor(0, 10, 0);
  buzzer.tone(NTD2, 300);
  delay(30);
  rgb.setColor(0, 0, 10);
  buzzer.tone(NTD3, 300);
  delay(30);
  rgb.clear();

  buzzer.noTone();
}


void writeBuffer(int index,unsigned char c){
  buffer[index]=c;
}
void writeHead(){
  writeSerial(0xff);
  writeSerial(0x55);
}
void writeEnd(){
 Serial.println(); 
}
void writeSerial(unsigned char c){
 Serial.write(c);
}
void readSerial(){
  isAvailable = false;
  if(Serial.available()>0){
    isAvailable = true;
    serialRead = Serial.read();
  }
}

         
void loop()
{

  Serial.println("Hello JDriven!");
  Stop();
  delay(1000);
  
  Serial.print("Move Speed: ");
  Serial.println(moveSpeed);
  
  Serial.println("Forward!");
  Forward();
  delay(1000);
  
  Serial.println("Backward!");
  Backward();
  delay(1000);
  
  Serial.println("TurnLeft!");
  TurnLeft();
  delay(1000);
  
  Serial.println("TurnRight!");
  TurnRight();
  delay(1000);

  Serial.println("ChangeSpeed to 10 and Forward");
  ChangeSpeed(10);
  Forward();
  delay(1000);

  Serial.println("ChangeSpeed to 250 and Forward");
  ChangeSpeed(250);
  Forward();
  delay(1000);


  // TODO distance sensor
  distanceDemo();

  // TODO line follow sensor
  lineFollowDemo();
  


}

void Forward()
{
  MotorL.run(-moveSpeed);
  MotorR.run(moveSpeed);
}
void Backward()
{
  MotorL.run(moveSpeed); 
  MotorR.run(-moveSpeed);
}
void TurnLeft()
{
  MotorL.run(-moveSpeed/10);
  MotorR.run(moveSpeed);
}
void TurnRight()
{
  MotorL.run(-moveSpeed);
  MotorR.run(moveSpeed/10);
}
void Stop()
{
  rgb.clear();
  MotorL.run(0);
  MotorR.run(0);
}

void ChangeSpeed(int spd)
{
  buzzer.tone(NTD5, 300); 
  moveSpeed = spd;
}


void distanceDemo()
{
  uint8_t d = ultr.distanceCm(50);
  static long time = millis();
  randomSeed(analogRead(6));
  uint8_t randNumber = random(2);
  if (d > 15 || d == 0)Forward();
  else if (d > 10) {
    switch (randNumber)
    {
      case 0:
        TurnLeft();
        delay(200);
        break;
      case 1:
        TurnRight();
        delay(200);
        break;
    }
  }
  else
  {
    Backward();
    delay(400);
  }
  delay(100);
}

void lineFollowDemo()
{
  uint8_t val;
  val = line.readSensors();
  if(moveSpeed >230)moveSpeed=230;
  switch (val)
  {
    case S1_IN_S2_IN:
      Forward();
      LineFollowFlag=10;
      break;

    case S1_IN_S2_OUT:
       Forward();
      if (LineFollowFlag>1) LineFollowFlag--;
      break;

    case S1_OUT_S2_IN:
      Forward();
      if (LineFollowFlag<20) LineFollowFlag++;
      break;

    case S1_OUT_S2_OUT:
      if(LineFollowFlag==10) Backward();
      if(LineFollowFlag<10) TurnLeft();
      if(LineFollowFlag>10) TurnRight();
      break;
  }
//  delay(50);
}



#include <Servo.h>
#include <Wire.h>
#include <SoftwareSerial.h>
#include "mCore.h"

#define NOTE_B0  31
#define NOTE_C1  33
#define NOTE_CS1 35
#define NOTE_D1  37
#define NOTE_DS1 39
#define NOTE_E1  41
#define NOTE_F1  44
#define NOTE_FS1 46
#define NOTE_G1  49
#define NOTE_GS1 52
#define NOTE_A1  55
#define NOTE_AS1 58
#define NOTE_B1  62
#define NOTE_C2  65
#define NOTE_CS2 69
#define NOTE_D2  73
#define NOTE_DS2 78
#define NOTE_E2  82
#define NOTE_F2  87
#define NOTE_FS2 93
#define NOTE_G2  98
#define NOTE_GS2 104
#define NOTE_A2  110
#define NOTE_AS2 117
#define NOTE_B2  123
#define NOTE_C3  131
#define NOTE_CS3 139
#define NOTE_D3  147
#define NOTE_DS3 156
#define NOTE_E3  165
#define NOTE_F3  175
#define NOTE_FS3 185
#define NOTE_G3  196
#define NOTE_GS3 208
#define NOTE_A3  220
#define NOTE_AS3 233
#define NOTE_B3  247
#define NOTE_C4  262
#define NOTE_CS4 277
#define NOTE_D4  294
#define NOTE_DS4 311
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_FS4 370
#define NOTE_G4  392
#define NOTE_GS4 415
#define NOTE_A4  440
#define NOTE_AS4 466
#define NOTE_B4  494
#define NOTE_C5  523
#define NOTE_CS5 554
#define NOTE_D5  587
#define NOTE_DS5 622
#define NOTE_E5  659
#define NOTE_F5  698
#define NOTE_FS5 740
#define NOTE_G5  784
#define NOTE_GS5 831
#define NOTE_A5  880
#define NOTE_AS5 932
#define NOTE_B5  988
#define NOTE_C6  1047
#define NOTE_CS6 1109
#define NOTE_D6  1175
#define NOTE_DS6 1245
#define NOTE_E6  1319
#define NOTE_F6  1397
#define NOTE_FS6 1480
#define NOTE_G6  1568
#define NOTE_GS6 1661
#define NOTE_A6  1760
#define NOTE_AS6 1865
#define NOTE_B6  1976
#define NOTE_C7  2093
#define NOTE_CS7 2217
#define NOTE_D7  2349
#define NOTE_DS7 2489
#define NOTE_E7  2637
#define NOTE_F7  2794
#define NOTE_FS7 2960
#define NOTE_G7  3136
#define NOTE_GS7 3322
#define NOTE_A7  3520
#define NOTE_AS7 3729
#define NOTE_B7  3951
#define NOTE_C8  4186
#define NOTE_CS8 4435
#define NOTE_D8  4699
#define NOTE_DS8 4978

MeRGBLed rgb;
MeUltrasonic ultr(PORT_4);
MeLineFollower line(PORT_2);
MeBuzzer buzzer;
MeDCMotor MotorL(M1);
MeDCMotor MotorR(M2);
MePort generalDevice;
MePort lightSensor(8);

int defaultSpeed = 75;
int minSpeed = 75;
int maxSpeed = 75;

int moveSpeed = defaultSpeed;
double lightStrength;

void setup()
{
  Serial.begin(115200);
  Serial.println("Game On JDriven!");
  delay(500);
}

void loop()
{
  if (isFinished()) {
    Serial.println("Finished!!!");
    halt();

    while(true) {
      playSong();
      delay(100);
    }
  }

  Serial.println("Following line");
  followLine();

  if (wallDetected()) {
    Serial.println("Wall Detected! Turning around");
    turnAround();
  } else {
    Serial.println("Crossing detected! Turning left");
    takeLeftTurn();
  }
}

boolean isFinished() {
  return lightSensor.aRead2() < 500;  
}

boolean wallDetected() {
  if (ultr.distanceCm() < 5.0) {
    for (int i = 0; i < 10; i++) {
      if (ultr.distanceCm() > 5.0) {
        return false;
      }
      delay(5);
    }
    return true;
  }
  
  return false;  
}

void followLine() {
  moveSpeed = maxSpeed;
  while (true) {
    uint8_t val = line.readSensors();
  
    switch (val)
    {
      case S1_IN_S2_IN:
        forward();
        break;
  
      case S1_IN_S2_OUT:
        turnLeft();
        delay(5);
        break;
  
      case S1_OUT_S2_IN:
        turnRight();
        delay(5);
        break;
  
      case S1_OUT_S2_OUT:
        moveSpeed = defaultSpeed;
        return;
    }

    if (wallDetected() || isFinished()) {
      moveSpeed = defaultSpeed;
      return;
    }
  }
  
}

void takeLeftTurn() {
  turnLeft();
  while (line.readSensors() == S1_OUT_S2_OUT) {
    delay(1);
  }
}

void takeRightTurn() {
  turnRight();
  while (line.readSensors() == S1_OUT_S2_OUT) {
    delay(1);
  }
}

void turnAround() {
  rotate();
  while (line.readSensors() != S1_OUT_S2_OUT) {
    delay(1);
  }
  while (line.readSensors() != S1_IN_S2_IN) {
    delay(1);
  }
  forward();
}


void  disco()
{
  rgb.setNumber(16);
  rgb.clear();
  rgb.setColor(10, 0, 0);
  buzzer.tone(294, 300); 
  delay(30);
  rgb.setColor(0, 10, 0);
  buzzer.tone(330, 300);
  delay(30);
  rgb.setColor(0, 0, 10);
  buzzer.tone(350, 300);
  delay(30);
  rgb.clear();

  buzzer.noTone();
}

void lightSensorDemo() {
   lightStrength = lightSensor.aRead2();
   Serial.print("LightStrength: ");
   Serial.println(lightStrength);
}
       
void motorDemo() 
{
  Serial.print("Move Speed: ");
  Serial.println(moveSpeed);
  
  Serial.println("Forward!");
  forward();
  delay(1000);
  
  Serial.println("Backward!");
  backward();
  delay(1000);
  
  Serial.println("TurnLeft!");
  turnLeft();
  delay(1000);
  
  Serial.println("TurnRight!");
  turnRight();
  delay(1000);

  Serial.println("ChangeSpeed to minSpeed and Forward");
  changeSpeed(minSpeed);
  forward();
  delay(1000);

  Serial.println("ChangeSpeed to maxSpeed and Forward");
  changeSpeed(maxSpeed);
  forward();
  delay(1000);

  Serial.println("ChangeSpeed to maxSpeed and Forward");
  changeSpeed(maxSpeed);
  forward();
  delay(1000);

  Serial.println("Halt");
  halt();
}

void rotate() {
  MotorL.run(-moveSpeed);
  MotorR.run(-moveSpeed);
}

void forward()
{
  MotorL.run(-moveSpeed);
  MotorR.run(moveSpeed);
}
void backward()
{
  MotorL.run(moveSpeed); 
  MotorR.run(-moveSpeed);
}
void turnLeft()
{
  MotorL.run(-moveSpeed/10);
  MotorR.run(moveSpeed);
}
void turnRight()
{
  MotorL.run(-moveSpeed);
  MotorR.run(moveSpeed/10);
}
void halt()
{
  rgb.clear();
  MotorL.run(0);
  MotorR.run(0);
}

void changeSpeed(int spd)
{
  buzzer.tone(441, 300); 
  moveSpeed = spd;
}


void distanceDemo()
{
  uint8_t d = ultr.distanceCm();
  Serial.print("Distance: ");
  Serial.println(d);
}

void lineFollowDemo()
{
  uint8_t val = line.readSensors();

  bool leftIn = false;
  bool rightIn = false;
  
  switch (val)
  {
    case S1_IN_S2_IN:
      leftIn = true;
      rightIn = true;
      break;

    case S1_IN_S2_OUT:
      leftIn  = true;
      rightIn = false;
      break;

    case S1_OUT_S2_IN:
      leftIn = false;
      rightIn = true;
      break;

    case S1_OUT_S2_OUT:
      leftIn = false;
      rightIn = false;
      break;
  }
  Serial.print("LeftIn: ");
  Serial.println(leftIn);
  Serial.print("RightIn: ");
  Serial.println(rightIn);
}



void playSong() {
  buzzer.tone(NOTE_DS5, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_E5, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_FS5, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  delay(62*1.3);
  
  buzzer.tone(NOTE_B5, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_E5, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_DS5, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_E5, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_FS5, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_B5, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_DS6, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_E6, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_DS6, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_AS5, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_B5, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  delay(62*1.3);
  
  buzzer.tone(NOTE_FS5, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  delay(62*1.3);
  
  buzzer.tone(NOTE_DS5, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_E5, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_FS5, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  delay(62*1.3);
  
  buzzer.tone(NOTE_B5, 125);
  delay(125*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_CS6, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_AS5, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_B5, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_CS6, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_E6, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_DS6, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_E6, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_CS6, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_FS4, 125);
  delay(125*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_GS4, 125);
  delay(125*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_D4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_DS4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_FS2, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_CS4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_D4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_CS4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_B3, 125);
  delay(125*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_B3, 125);
  delay(125*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_CS4, 125);
  delay(125*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_D4, 125);
  delay(125*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_D4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_CS4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_B3, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_CS4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_DS4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_FS4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_GS4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_DS4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_FS4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_CS4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_DS4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_B3, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_CS4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_B3, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_DS4, 125);
  delay(125*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_FS4, 125);
  delay(125*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_GS4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_DS4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_FS4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_CS4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_DS4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_B3, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_D4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_DS4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_D4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_CS4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_B3, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_CS4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_D4, 125);
  delay(125*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_B3, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_CS4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_DS4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_FS4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_CS4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_D4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_CS4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_B3, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_CS4, 125);
  delay(125*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_B3, 125);
  delay(125*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_CS4, 125);
  delay(125*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_FS4, 125);
  delay(125*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_GS4, 125);
  delay(125*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_D4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_DS4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_FS2, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_CS4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_D4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_CS4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_B3, 125);
  delay(125*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_B3, 125);
  delay(125*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_CS4, 125);
  delay(125*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_D4, 125);
  delay(125*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_D4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_CS4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_B3, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_CS4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_DS4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_FS4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_GS4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_DS4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_FS4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_CS4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_DS4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_B3, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_CS4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_B3, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_DS4, 125);
  delay(125*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_FS4, 125);
  delay(125*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_GS4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_DS4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_FS4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_CS4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_DS4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_B3, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_D4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_DS4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_D4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_CS4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_B3, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_CS4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_D4, 125);
  delay(125*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_B3, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_CS4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_DS4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_FS4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_CS4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_D4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_CS4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_B3, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_CS4, 125);
  delay(125*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_B3, 125);
  delay(125*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_B3, 125);
  delay(125*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_B3, 125);
  delay(125*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_FS3, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_GS3, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_B3, 125);
  delay(125*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_FS3, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_GS3, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_B3, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_CS4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_DS4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_B3, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_E4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_DS4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_E4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_FS4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_B3, 125);
  delay(125*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_B3, 125);
  delay(125*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_FS3, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_GS3, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_B3, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_FS3, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_E4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_DS4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_CS4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_B3, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_E3, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_DS3, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_E3, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_FS3, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_B3, 125);
  delay(125*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_FS3, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_GS3, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_B3, 125);
  delay(125*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_FS3, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_GS3, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_B3, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_B3, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_CS4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_DS4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_B3, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_FS3, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_GS3, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_FS3, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_B3, 125);
  delay(125*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_B3, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_AS3, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_B3, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_FS3, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_GS3, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_B3, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_E4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_DS4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_E4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_FS4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_B3, 125);
  delay(125*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_AS3, 125);
  delay(125*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_B3, 125);
  delay(125*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_FS3, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_GS3, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_B3, 125);
  delay(125*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_FS3, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_GS3, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_B3, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_CS4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_DS4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_B3, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_E4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_DS4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_E4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_FS4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_B3, 125);
  delay(125*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_B3, 125);
  delay(125*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_FS3, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_GS3, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_B3, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_FS3, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_E4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_DS4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_CS4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_B3, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_E3, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_DS3, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_E3, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_FS3, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_B3, 125);
  delay(125*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_FS3, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_GS3, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_B3, 125);
  delay(125*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_FS3, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_GS3, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_B3, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_B3, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_CS4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_DS4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_B3, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_FS3, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_GS3, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_FS3, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_B3, 125);
  delay(125*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_B3, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_AS3, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_B3, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_FS3, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_GS3, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_B3, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_E4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_DS4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_E4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_FS4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_B3, 125);
  delay(125*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_CS4, 125);
  delay(125*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_FS4, 125);
  delay(125*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_GS4, 125);
  delay(125*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_D4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_DS4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_FS2, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_CS4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_D4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_CS4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_B3, 125);
  delay(125*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_B3, 125);
  delay(125*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_CS4, 125);
  delay(125*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_D4, 125);
  delay(125*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_D4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_CS4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_B3, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_CS4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_DS4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_FS4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_GS4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_DS4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_FS4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_CS4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_DS4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_B3, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_CS4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_B3, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_DS4, 125);
  delay(125*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_FS4, 125);
  delay(125*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_GS4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_DS4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_FS4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_CS4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_DS4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_B3, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_D4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_DS4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_D4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_CS4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_B3, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_CS4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_D4, 125);
  delay(125*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_B3, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_CS4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_DS4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_FS4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_CS4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_D4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_CS4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_B3, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_CS4, 125);
  delay(125*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_B3, 125);
  delay(125*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_CS4, 125);
  delay(125*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_FS4, 125);
  delay(125*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_GS4, 125);
  delay(125*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_D4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_DS4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_FS2, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_CS4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_D4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_CS4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_B3, 125);
  delay(125*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_B3, 125);
  delay(125*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_CS4, 125);
  delay(125*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_D4, 125);
  delay(125*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_D4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_CS4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_B3, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_CS4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_DS4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_FS4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_GS4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_DS4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_FS4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_CS4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_DS4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_B3, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_CS4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_B3, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_DS4, 125);
  delay(125*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_FS4, 125);
  delay(125*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_GS4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_DS4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_FS4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_CS4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_DS4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_B3, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_D4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_DS4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_D4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_CS4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_B3, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_CS4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_D4, 125);
  delay(125*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_B3, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_CS4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_DS4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_FS4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_CS4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_D4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_CS4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_B3, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_CS4, 125);
  delay(125*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_B3, 125);
  delay(125*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_B3, 125);
  delay(125*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_B3, 125);
  delay(125*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_FS3, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_GS3, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_B3, 125);
  delay(125*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_FS3, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_GS3, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_B3, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_CS4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_DS4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_B3, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_E4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_DS4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_E4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_FS4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_B3, 125);
  delay(125*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_B3, 125);
  delay(125*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_FS3, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_GS3, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_B3, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_FS3, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_E4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_DS4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_CS4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_B3, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_E3, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_DS3, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_E3, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_FS3, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_B3, 125);
  delay(125*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_FS3, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_GS3, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_B3, 125);
  delay(125*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_FS3, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_GS3, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_B3, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_B3, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_CS4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_DS4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_B3, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_FS3, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_GS3, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_FS3, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_B3, 125);
  delay(125*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_B3, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_AS3, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_B3, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_FS3, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_GS3, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_B3, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_E4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_DS4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_E4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_FS4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_B3, 125);
  delay(125*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_AS3, 125);
  delay(125*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_B3, 125);
  delay(125*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_FS3, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_GS3, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_B3, 125);
  delay(125*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_FS3, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_GS3, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_B3, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_CS4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_DS4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_B3, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_E4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_DS4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_E4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_FS4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_B3, 125);
  delay(125*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_B3, 125);
  delay(125*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_FS3, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_GS3, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_B3, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_FS3, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_E4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_DS4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_CS4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_B3, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_E3, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_DS3, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_E3, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_FS3, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_B3, 125);
  delay(125*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_FS3, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_GS3, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_B3, 125);
  delay(125*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_FS3, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_GS3, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_B3, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_B3, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_CS4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_DS4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_B3, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_FS3, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_GS3, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_FS3, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_B3, 125);
  delay(125*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_B3, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_AS3, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_B3, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_FS3, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_GS3, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_B3, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_E4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_DS4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_E4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_FS4, 62);
  delay(62*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_B3, 125);
  delay(125*1.3);
  buzzer.noTone();
  
  buzzer.tone(NOTE_CS4, 125);
  delay(125*1.3);
  buzzer.noTone();
}


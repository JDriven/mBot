
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

#include <Servo.h>
#include <Wire.h>
#include <SoftwareSerial.h>
#include "mCore.h"

MeRGBLed rgb;
MeUltrasonic ultr(PORT_3);
MeLineFollower line(PORT_2);
MeBuzzer buzzer;
MeDCMotor MotorL(M1);
MeDCMotor MotorR(M2);
MePort generalDevice;
MePort lightSensor(8);

int defaultSpeed = 200;
int minSpeed = 48;
int maxSpeed = 250;

int turnSpeed = 100; //origineel 100
int moveSpeed = 180; //origineel 100
int correctionSpeed = 150; //origineel 80

void setup()
{
  Serial.begin(115200);
  Serial.println("racebak!");
  disco();
  delay(1000);
  rgb.setNumber(16);
  rgb.clear();

}

void loop() {
  followLineToCross();
  disco();
  left90();
  while (blockAhead()) {
    right90();
    disco();
  }
}

void music() {
  int melody[] = {
  NOTE_G4,//5  
  NOTE_G4,//5
  NOTE_A4,//6
  NOTE_G4,//5
  NOTE_C5,//1.
  NOTE_B4,//7
  0,
  NOTE_G4,//5
  NOTE_G4,//5
  NOTE_A4,//6
  NOTE_G4,//5
  NOTE_D5,//2.
  NOTE_C5,//1.
  0,
  NOTE_G4,//5
  NOTE_G4,//5
  NOTE_G5,//5.
  NOTE_E5,//3.
  NOTE_C5,//1.
  NOTE_B4,//7
  NOTE_A4,//6
  0,
  NOTE_F5,//4.
  NOTE_F5,//4.
  NOTE_E5,//3.
  NOTE_C5,//1.
  NOTE_D5,//2.
  NOTE_C5,//1.
  0,
  };
  
  int noteDurations[] = {
    8,8,4,4,4,4,
    4,
    8,8,4,4,4,4,
    4,
    8,8,4,4,4,4,2,
    8,
    8,8,4,4,4,2,
    4,
  };
  // iterate over the notes of the melody:
  for (int thisNote = 0; thisNote < 8; thisNote++) {

    // to calculate the note duration, take one second
    // divided by the note type.
    //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
    int noteDuration = 1000 / noteDurations[thisNote];
    buzzer.tone(melody[thisNote], noteDuration);

    // to distinguish the notes, set a minimum time between them.
    // the note's duration + 30% seems to work well:
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    // stop the tone playing:
    buzzer.noTone();
  }


}

void win() {
  rgb.setColor(10, 10, 10);
  disco();
  delay(200);
  halt();
  delay(500);
  backward200();
  delay(1000);
  
  donut();
  while(true) {
    music();
    delay(100);
  }
}

void followLineToCross() {
  Serial.println("followLineToCross!");
  rgb.setColor(10, 0, 0);
  forward();
  while(true) {
    if (isDark()) {
      win();
    }
    
    bool left = leftIsBlack();
    bool right = rightIsBlack();
    if (!left && !right) {
      forward100();
      rgb.setColor(20, 0, 0);
      do {
        delay(10);
        left = leftIsBlack();
        right = rightIsBlack();
      } while(!left && !right);
      rgb.setColor(30, 0, 0);
      delay(100);//origineel 100 //TODO lijndetectie
      halt();
      delay(50);
      return;
    } else if (!left) {
      nudgeRight();  
    } else if (!right) {
      nudgeLeft();  
    } else {
      forward();
    }
    delay(10);
  }
}

void left90() {
  Serial.println("left90!");
  rgb.setColor(0, 10, 0);
    
  turnLeft();
  bool left = leftIsBlack();
  bool right = rightIsBlack();
  do {
    delay(10);
    left = leftIsBlack();
    right = rightIsBlack();
  } while(left || right);
  do {
    delay(10);
    left = leftIsBlack();
    right = rightIsBlack();
  } while(!(left && right));
  halt();
  
  if (blockAhead()) {
    backward();
    delay(800);
    halt();
  }
}

void right90() {
  Serial.println("right90!");
  rgb.setColor(0, 10, 0);
    
  turnRight();
  bool left = leftIsBlack();
  bool right = rightIsBlack();
  do {
    delay(10);
    left = leftIsBlack();
    right = rightIsBlack();
  } while(left || right);
  do {
    delay(10);
    left = leftIsBlack();
    right = rightIsBlack();
  } while(!(left && right));
  halt();
  
  if (blockAhead()) {
    backward();
    delay(800);
    halt();
  }
}

boolean isDark() {
  double lightStrength = lightSensor.aRead2();
  Serial.print("LightStrength: ");
  Serial.println(lightStrength);
  return lightStrength < 300.0;
}

bool blockAhead() {
  uint8_t d = ultr.distanceCm(50);
  Serial.print("Distance: ");
  Serial.println(d);
  return d > 0 && d < 25;
}

void  disco()
{
  buzzer.tone(294, 300); 
  delay(30);
  buzzer.tone(330, 300);
  delay(30);
  buzzer.tone(350, 300);
  delay(30);
  buzzer.noTone();
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

void forward()
{
  MotorL.run(-moveSpeed);
  MotorR.run(moveSpeed);
}
void forward100()
{
  MotorL.run(-100);
  MotorR.run(100);
}
void backward()
{
  MotorL.run(turnSpeed); 
  MotorR.run(-turnSpeed);
}
void backward200()
{
  MotorL.run(200); 
  MotorR.run(-200);
}
void turnLeft()
{
  MotorL.run(-turnSpeed/10);
  MotorR.run(turnSpeed);
}
void donut()
{
  MotorL.run(250);
  MotorR.run(250);
}
void turnRight()
{
  MotorL.run(-turnSpeed);
  MotorR.run(turnSpeed/10);
}
void nudgeLeft()
{
//  MotorL.run(-moveSpeed);
//  MotorR.run(moveSpeed + );
  MotorL.run(-correctionSpeed);
  MotorR.run(moveSpeed);
}
void nudgeRight()
{
  MotorL.run(-moveSpeed);
  MotorR.run(correctionSpeed);
}
void halt()
{
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
  uint8_t d = ultr.distanceCm(50);
  Serial.print("Distance: ");
  Serial.println(d);
}

bool leftIsBlack() {
  uint8_t val = line.readSensors();
  bool left = val == S1_IN_S2_IN || val == S1_IN_S2_OUT;
//  Serial.print("LeftIn: ");
//  Serial.println(left);
  return left;
}

bool rightIsBlack() {
  uint8_t val = line.readSensors();
  bool right = val == S1_IN_S2_IN || val == S1_OUT_S2_IN;
//  Serial.print("RightIn: ");
//  Serial.println(right);
  return right;
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



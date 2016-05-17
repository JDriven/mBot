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
MePort lightsensor(8);

int defaultSpeed = 100;
int minSpeed = 48;
int maxSpeed = 250;
int minDistance = 10;

int moveSpeed = defaultSpeed;

void setup()
{
  Serial.begin(115200);
  Serial.println("Game On JDriven!");
  delay(1000);
}

void loop()
{  
  // line follow sensor
  lineFollowDemo();

  // distance sensor
  // distanceDemo();
  
  delay(10);  
}



void  disco()
{
  buzzer.noTone();
}
       
void motorDemo() 
{

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

void rotateLeft() {  
  rgb.setColor(0, 255,0,0);
  rgb.show();

  uint8_t val;
  do {
    MotorL.run(moveSpeed);
    MotorR.run(moveSpeed);
    delay(5);
    val = line.readSensors();
  } while (val != S1_OUT_S2_OUT);
  do {
    MotorL.run(moveSpeed);
    MotorR.run(moveSpeed);
    delay(5);
    val = line.readSensors();
  } while (val != S1_IN_S2_IN);

  delay(60);
  halt();
}

void rotateRight() {  
  rgb.setColor(0, 0,255,0);
  rgb.show();
  
  uint8_t val;
  do {
    MotorL.run(-moveSpeed);
    MotorR.run(-moveSpeed);
    delay(5);
    val = line.readSensors();
  } while (val != S1_OUT_S2_OUT);
  do {
    MotorL.run(-moveSpeed);
    MotorR.run(-moveSpeed);
    delay(5);
    val = line.readSensors();
  } while (val != S1_IN_S2_IN);

  delay(60);
  halt();
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
  uint8_t d = ultr.distanceCm(50);
  if (d>0 && d < minDistance) {
    Serial.print("distance: ");
    Serial.println(d);
    halt();
  }
}

void lineFollowDemo()
{
  rgb.setColor(0, 0,0,255);
  rgb.show();
  
  uint8_t val = line.readSensors();

  bool leftIn = false;
  bool rightIn = false;
  
  switch (val)
  {
    case S1_IN_S2_IN:
      forward();
      break;

    case S1_IN_S2_OUT:
      turnLeft();
      break;

    case S1_OUT_S2_IN:
      turnRight();
      break;

    case S1_OUT_S2_OUT:
      forward();
      delay(350);
      halt();

      determineDirection();
      
      break;
  }
}

void checkFinish()
{
    rgb.setColor(0, 0,0,0);
    rgb.show();
    double lightVal = lightsensor.aRead2();
    Serial.print("lightsensor value: ");
    Serial.println(lightVal);

    if (lightVal < 500) {
      halt();
      playTheMusic();
    }
// if blabla then playmusic
}

void playTheMusic(){
    buzzer.tone(494, 250);
    delay(260);
    buzzer.tone(494, 250);
    delay(260);
    buzzer.tone(523, 250);
    delay(260);
    buzzer.tone(587, 250);
    delay(260);
    buzzer.tone(587, 250);
    delay(260);
    buzzer.tone(523, 250);
    delay(260);
    buzzer.tone(494, 250);
    delay(260);
    buzzer.tone(440, 250);
    delay(260);
    buzzer.tone(392, 250);
    delay(260);
    buzzer.tone(392, 250);
    delay(260);
    buzzer.tone(440, 250);
    delay(260);
    buzzer.tone(494, 250);
    delay(260);
    buzzer.tone(494, 250);
    buzzer.tone(494, 125);
    delay(385);
    buzzer.tone(440, 125);
    delay(135);
    buzzer.tone(440, 500);
}

void determineDirection()
{
  checkFinish();
  rotateLeft();
  bool wall = detectWall();
  // links
  if (wall) {
    rotateRight();
  } else {
    return;
  }

  // rechtdoor
  wall = detectWall();
  if (wall) {
    rotateRight();
  } else {
    return;
  }

  // rechts
  wall = detectWall();
  if (wall) {
    rotateRight();
  } else {
    return;
  }
}

bool detectWall() {
  uint8_t d = ultr.distanceCm(50);
  return d>0 && d < 30;
}


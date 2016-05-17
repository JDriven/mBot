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

int moveSpeed = defaultSpeed;
double lightStrength;


void setup()
{
  Serial.begin(115200);
  Serial.println("Game On JDriven!");
  delay(1000);
}

void loop()
{
  // disco
  disco();

  // motor control
  motorDemo();

  // distance sensor
  distanceDemo();

  // line follow sensor
  lineFollowDemo();

  // light sensor
  lightSensorDemo();
  delay(500);
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
  uint8_t d = ultr.distanceCm(50);
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

void lightSensorDemo() {
   lightStrength = lightSensor.aRead2();
   Serial.print("LightStrength: ");
   Serial.println(lightStrength);
}



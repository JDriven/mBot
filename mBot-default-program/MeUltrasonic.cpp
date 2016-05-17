#include "MeUltrasonic.h"
/*           UltrasonicSenser                 */
// MeUltrasonic::MeUltrasonic(): MePort(0)
// {
// }

const int numReadings = 10;

long readings[numReadings];      // the readings from the analog input
int readIndex = 0;              // the index of the current reading
long total = 0;                  // the running total
double average = 0;                // the average




MeUltrasonic::MeUltrasonic(MEPORT port): MePort(port)
{
  for (int thisReading = 0; thisReading < numReadings; thisReading++) {
    readings[thisReading] = 0;
  }    
}

double MeUltrasonic::distanceCm(uint16_t maxCm)
{
    long distance = measure(maxCm * 55 + 200);
    return (double)distance / 58.0;
}

double MeUltrasonic::distanceInch(uint16_t maxInch)
{
    long distance = measure(maxInch * 145 + 200);
    return (double)(distance / 148.0);
}

double MeUltrasonic::distanceCm(){
  double distance = measureSmooth();
  return distance / 58.0;
}
double MeUltrasonic::distanceInch(){
  return distanceInch(5);
}
long MeUltrasonic::measure(unsigned long timeout)
{
    long duration;
    // MePort::dWrite2(LOW);
    // delayMicroseconds(2);
    // MePort::dWrite2(HIGH);
    // delayMicroseconds(10);
    // MePort::dWrite2(LOW);
    // pinMode(s2, INPUT);
    // duration = pulseIn(s2, HIGH, timeout);
    digitalWrite(s2,LOW);
    delayMicroseconds(2);
    digitalWrite(s2,HIGH);
    delayMicroseconds(10);
    digitalWrite(s2,LOW);
    pinMode(s2,INPUT);
    duration = pulseIn(s2,HIGH);
    return duration;
}

double MeUltrasonic::measureSmooth() {
  // subtract the last reading:
  total = total - readings[readIndex];
  // read from the sensor:
  readings[readIndex] = measure(0);
  // add the reading to the total:
  total = total + readings[readIndex];
  // advance to the next position in the array:
  readIndex = readIndex + 1;

  // if we're at the end of the array...
  if (readIndex >= numReadings) {
    // ...wrap around to the beginning:
    readIndex = 0;
  }

  // calculate the average:
  average = total / numReadings;
  
  return average;
}


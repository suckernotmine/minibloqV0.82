/*******************************************************
    RobotMotor.cpp // Arduino Motor Shield R3 version
    low level motor driver for use with L298 H-Bridge
	This version for boards with Speed and Dir inputs
daobot pin allocation

    Michael Margolis July 20 2013
********************************************************/
#if (ARDUINO >= 100)
 #include <Arduino.h>
#else
 #include <WProgram.h>
#endif

#include "RobotMotor.h"

const int differential = 0; // % faster left motor turns compared to right 

// Bridge 1 drives right motor, 2 drives left motor
/****** motor pin defines *************/
// PWM pins conenct to enable pins and control the speed
// other pins select forward and reverse

// Motor pins for daobot us DFRobot shield:             
const byte M_PWM_PIN[2]  = {5,6}; 
const byte M_DIR_PIN[2]  = {4,7};     
/* end of motor pin defines */

/*
 * L298 input connections using the above defines:
 * Left Motor:
 *  connect E2 to Arduino pin 6
 *  connect M2 to Arduino pin 4
 * Right Motor
 *  connect E1 to Arduino pin 5
 *  connect M1 to Arduino pin 7 
*/


int  motorSpeed[2]  = {0,0}; // motor speed stored here (0-100%)

// tables hold time in ms to rotate robot 360 degrees at various speeds 
// this enables conversion of rotation angle into timed motor movement 
// The speeds are percent of max speed
// Note: low cost motors do not have enough torque at low speeds so
// the robot will not move below this value 
// Interpolation is used to get a time for any speed from MIN_SPEED to 100%

const int MIN_SPEED = 50; // first table entry is 50% speed
const int SPEED_TABLE_INTERVAL = 10; // each table entry is 10% faster speed
const int NBR_SPEEDS =  1 + (100 - MIN_SPEED)/ SPEED_TABLE_INTERVAL;
 
int speedTable[NBR_SPEEDS]  = { 50,  60,   70,   80,   90,  100}; // speeds  
int motorPWM[NBR_SPEEDS]    = { 150, 175,  190, 210,   230, 250}; // pwm value 
int PWM[2]; // the current pwm for each motor

int msPerCM = 40; // how many milliseconds for the robot to move 1 cm


// variable used to determine motor direction when using encoder braking.
int motorState[2]  = {MOTOR_NOT_DRIVING, MOTOR_NOT_DRIVING};

void motorBegin(int motor)
{
  pinMode(M_DIR_PIN[motor], OUTPUT);
  digitalWrite(M_DIR_PIN[motor], HIGH); 
  motorStop(motor);
}

// return the number of milliseconds needed to move the given distance at the current speed
// only used if robot does not have encoders
unsigned long motorDistanceToTime(unsigned long distance)
{
   //Serial.print("d to t "); Serial.println((distance * msPerCM * 10L ) /  (motorSpeed[0] + motorSpeed[1] /2));
   return (distance * msPerCM * 10L ) /  (motorSpeed[0] + motorSpeed[1] /2);
}

int motorGetTargetPWM(int speed)
{
  int PWM = 0; 
   if(speed >= MIN_SPEED)
   {
     int index = (speed - MIN_SPEED) / SPEED_TABLE_INTERVAL ; // index into speed and time tables
     int p0 = motorPWM[index];
     int p1 = motorPWM[index+1];    // pwm for the next higher speed 
     PWM =   map(speed,  speedTable[index],  speedTable[index+1], p0, p1);
   }
   return PWM;
}


// speed range is 0 to 100
void motorSetSpeed(int motor, int speed)
{
   if(speed <= 0)
      speed = 0;
   else if(speed > 100)
      speed = 100;	  
	  
   motorSpeed[motor] = speed;           // save the value
   motorWritePWM( motor, motorGetTargetPWM(speed));
}

int motorGetSpeed(int motor)
{
   return motorSpeed[motor];
}

void motorWritePWM(int motor, int value)
{
   PWM[motor] = value;
   analogWrite(M_PWM_PIN[motor], value);  // write the value   
}

int motorReadPWM(int motor)
{
  return PWM[motor];
}

void motorForward(int motor, int speed)
{
  digitalWrite(M_DIR_PIN[motor], HIGH);
  motorSetSpeed(motor, speed); 
  motorState[motor] = MOTOR_DRIVE_FWD;
}

void motorReverse(int motor, int speed)
{
  digitalWrite(M_DIR_PIN[motor], LOW);
  motorSetSpeed(motor, speed);  
  motorState[motor] = MOTOR_DRIVE_REV;
}

void motorStop(int motor)
{
  analogWrite(M_PWM_PIN[motor], 0);
  motorState[motor] = MOTOR_NOT_DRIVING;
}

void motorBrake(int motor)
{
  // brake not supported, so just stop the motor
  motorStop(motor);
  motorState[motor] = MOTOR_NOT_DRIVING;  
}
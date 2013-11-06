/*******************************************************
    RobotMotor.h
    low level motor driver interface
	
    Copyright Michael Margolis May 8 2012
********************************************************/

/* if you have the 4WD chassis:
      change the line:
        CHASSIS_2WD 
      to:
  	    CHASSIS_4WD
 */
#ifndef ROBOT_MOTOR_H
#define ROBOT_MOTOR_H 

#define CHASSIS_2WD   // change 2WD to 4WD if using the 4WD chassis


/// motor states for encoder braking
enum     { MOTOR_DRIVE_REV=-1, MOTOR_NOT_DRIVING, MOTOR_DRIVE_FWD };

// defines for left and right motors
const int MOTOR_LEFT  = 0;
const int MOTOR_RIGHT = 1;

extern const int MIN_SPEED;
extern int motorState[2];
extern const int NBR_SPEEDS;

void motorBegin(int motor);

// return the amount of time needed to move the given distance at the current speed
// only used if robot does not have encoders
unsigned long motorDistanceMMToTime(unsigned long distanceMM);

// speed range is 0 to 100 percent
int motorGetTargetPWM(int speed);
int  motorGetSpeed(int motor);
void motorSetSpeed(int motor, int speed);
void motorWritePWM(int motor, int value);
int  motorReadPWM(int motor);

void motorForward(int motor, int speed);

void motorReverse(int motor, int speed);

void motorStop(int motor);

void motorBrake(int motor);

#endif
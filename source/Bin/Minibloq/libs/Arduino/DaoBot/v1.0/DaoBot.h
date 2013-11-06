/*
  Robot interface module for minibloq
  Michael Margolis August 9 2013
*/ 

#ifndef ROBOT_H
#define ROBOT_H


#if (ARDUINO >= 100)
 #include <Arduino.h>
#else
 #include <WProgram.h>
 #include <pins_arduino.h>
#endif

// TODO - the following defines are duplicated in board.h !!!

// modes for robot movement 
#define Oblivious 0
#define Watchful  1

// Look direction angles for servo 
#define Look_Left 150
#define Look_Cntr 90
#define Look_Rght 30

#define Left_Bump     1 //  OBST_LEFT_EDGE
#define Right_Bump    2 //  OBST_RIGHT_EDGE
#define Front_Ping    4 //  OBST_FRONT

// mode argument indicates if the function is watchful or oblivious
// if watchful, robot automatically stops movement when obstacle is detected
// robotLookForObstacle can be interrogated to indicate current obstacle

extern void robotBegin(int encoder0Pin, int encoder1Pin);
extern void robotMoveForward(int mode, unsigned long distance);
extern void robotMoveBackward(int mode, unsigned long distance);
extern void robotTimedMoveForward(int mode, unsigned long duration);
extern void robotTimedMoveBackward(int mode, unsigned long duration);
extern void robotMoveRotate(int mode, int angle);
extern void robotMoveRotateCCW(int mode, int angle);
extern void robotSetSpeed(int speed);
extern void robotMoveStop();

extern boolean robotLookForObstacle(int obstacle); // dropdown selects abstracted obstacles

void robotBegin(int encoder0Pin, int encoder1Pin);

#endif 

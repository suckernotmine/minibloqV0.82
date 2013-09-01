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

// mode argument indicates if the function is watchful or oblivious
// if watchful, robot automatically stops movement when obstacle is detected
// robotLookForObstacle can be interrogated to indicate current obstacle
extern void robotBegin(); // to be called by runtime when sketch starts

extern void robotMoveForward(int mode, float cm);
extern void robotMoveBackward(int mode, float cm);
extern void robotTimedMoveForward(int mode, unsigned long duration);
extern void robotTimedMoveBackward(int mode, unsigned long duration);
extern void robotMoveRotate(int mode, float deg);
extern void robotMoveRotateCCW(int mode, float deg);
extern void robotSetSpeed(int speed);
extern void robotMoveStop();

extern boolean robotLookForObstacle(int obstacle); // dropdown selects abstracted obstacles


#endif 

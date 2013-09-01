// SoftServo.h

#ifndef SOFTSERVO_H
#define MOVE_H

#if (ARDUINO >= 100)
 #include <Arduino.h>
#else
 #include <WProgram.h>
#endif

void moveBegin();
void moveLeft();
void moveRight();
void moveForward();
void moveBackward();
void moveRotate(int angle);
void moveStop();
void moveBrake();
void moveSetSpeed(int speed);
int moveGetSpeed();
int moveGetState();
// calls given watch function, returns true of obstacle found
bool delayWatchful(unsigned long interval, bool (*watchFunc)(void));
bool moveForwardWatchful( unsigned long distance, bool (*watchFunc)(void));
bool moveReverseWatchful( unsigned long distance, bool (*watchFunc)(void));
bool moveRotateWatchful( int angle, bool (*watchFunc)(void));
bool moveForwardTimedWatchful( unsigned long durationMS, bool (*watchFunc)(void));
bool moveReverseTimedWatchful( unsigned long durationMS, bool (*watchFunc)(void));
bool ignoreWatch();
void changeMoveState(int newState);


#endif

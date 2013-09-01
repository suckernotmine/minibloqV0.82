// Move.h

#ifndef MOVE_H
#define MOVE_H

#if (ARDUINO >= 100)
 #include <Arduino.h>
#else
 #include <WProgram.h>
#endif

/// move states:
enum        {MOV_LEFT, MOV_RIGHT, MOV_FORWARD, MOV_BACK, MOV_ROTATE, MOV_STOP};


void moveBegin();
void moveSetSpeed(int speed);
int moveGetSpeed();
int moveGetState();
// calls given watch function, returns true of obstacle found
bool delayWatchful(unsigned long interval, bool (*watchFunc)(void));
void changeMoveState(int newState);


#endif

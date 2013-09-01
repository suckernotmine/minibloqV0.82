/*************************************
 * Move: mid level movement functions
 *************************************/
 
#include "Move.h"

const char* states[] = {"Left", "Right", "Forward", "Back", "Rotate", "Stop"};

int moveState = MOV_STOP;   // what robot is doing

int  moveSpeed   = 0;     // move speed stored here (0-100%)
int  speedIncrement = 10; // percent to increase or decrease speed



/*******************************************/

void moveBegin()
{
}

int moveGetState()
{
  return moveState;  
}

// this is the low level movement state.
// it will differ from the command state when the robot is avoiding obstacles
void changeMoveState(int newState)
{
  if(newState != moveState)
  {
    Serial.print("Changing move state from "); 
    Serial.print( states[moveState]);
    Serial.print(" to "); 
    Serial.println(states[newState]);
    moveState = newState;
  } 
}

/************* high level movement functions using encoder **********/

// calls given watch function, returns true of obstacle found
bool delayWatchful(unsigned long interval, bool (*watchFunc)(void))
{
  unsigned long oldCount;  
  unsigned long myDur;
  unsigned long start = millis();
  while(millis() - start <= interval) {
    // todo - service encoder if fitted
    if( watchFunc())
      return true;
  }
  return false;
}
 


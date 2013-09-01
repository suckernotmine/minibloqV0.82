/*
  Robot interface module for minibloq
  This version is for testing Sparki
  Michael Margolis 2013
  
*/ 

#include "Robot.h"
#include "Sparki.cpp"
#include "Move.cpp"
#include "Look.cpp"

#define Left_Bump       OBST_LEFT_EDGE
#define Right_Bump      OBST_RIGHT_EDGE
#define Front_Distance  OBST_FRONT

SparkiClass robot; // create robot object to control a robot 

// private function to resolve watchful movements
// in this version, a distance argument will be converted into duration
bool moveWatchful( unsigned long duration, bool (*watchFunc)(void))
{
   unsigned long startTime = millis();
   while(true) // do until completed movement or aborted by watch function 
   {
     if(watchFunc() == true) // check for obstacles, abort if func returns true
     {
       //fp("stopped by watchFunc\r\n");
       return true;
     }     
     if( millis() - startTime > duration){  
       // fp("stopped by duration\r\n");
        break;
    } 
   }
   return false;
}

// TODO - this should be called by minibloq runtime when system starts
void robotBegin()
{
   robot.begin(); // start the robot up
   // initialize any higher level functions here ...
}

void robotMoveForward(int mode, float cm)
{
   changeMoveState(MOV_FORWARD);
   if( mode == 0 ) // oblivious, define in minibloq.h
   {
      robot.moveForward(cm); 
   }   
   else if ( mode == 1 ) // watchful
   {   
      unsigned long run = (unsigned long)(22.2222222*cm);
      robot.moveForward();
      moveWatchful(run, checkMovement);
   }
}

void robotMoveBackward(int mode, float cm)
{
   changeMoveState(MOV_BACK);
   if( mode == 0 ) // oblivious, define in minibloq.h
   {
      robot.moveBackward(cm); 
   }   
   else if ( mode == 1 ) // watchful
   {   
      unsigned long run = (unsigned long)(22.2222222*cm);
      robot.moveBackward();
      moveWatchful(run, checkMovement);
   }
}

void robotTimedMoveForward(int mode, unsigned long duration)
{
  // todo if needed
}

void robotTimedMoveBackward(int mode, unsigned long duration)
{
  // todo if needed
}

void robotMoveRotate(int mode, float deg)
{

   changeMoveState(MOV_ROTATE);
   if( mode == 0 ) // oblivious, define in minibloq.h
   {
     robot.moveRight(deg);
   }   
   else if ( mode == 1 ) // watchful
   {  
     float turn = 21.388888*deg;
     robot.moveRight();   
     moveWatchful(turn, checkMovement);
   }
}

void robotMoveRotateCCW(int mode, float deg)
{
   changeMoveState(MOV_ROTATE);
   if( mode == 0 ) // oblivious, define in minibloq.h
   {
     robot.moveLeft(deg);
   }   
   else if ( mode == 1 ) // watchful
   {  
     float turn = 21.388888*deg;
     robot.moveLeft();   
     moveWatchful(turn, checkMovement);
   }
}

void robotSetSpeed(int speed)
{
   // todo if needed !!!
}

void robotMoveStop()
{
   changeMoveState(MOV_STOP);
   robot.moveStop();
}

boolean robotLookForObstacle(int obstacle)
{
  // translate obstacle defined in ninibloq.h to that used look.cpp
  if( obstacle == Left_Bump)
     obstacle = OBST_LEFT_EDGE;
  else if(obstacle == Right_Bump) 
    obstacle = OBST_RIGHT_EDGE;
   else if(obstacle == Front_Distance)	
    obstacle = OBST_FRONT; 
 
   //return lookForObstacle(obstacle);
   return false; // TODO - uncomment above !!!
} 
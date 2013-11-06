/*
  Robot interface module for minibloq
  Copyright Michael Margolis 2013
  
*/ 

#include "Robot.h"
#include "Look.cpp"
#include "Move.cpp"
#include "SoftServo.cpp"
#include "Distance.cpp"
#include "RobotMotor.cpp"


#define Left_Bump       OBST_LEFT_EDGE
#define Right_Bump      OBST_RIGHT_EDGE
#define Front_Distance  OBST_FRONT



// the following just contains dummy code to blink the LED

void blink(int mode, int count)
{
int wait = 500; // slow blink if mode is not 0 or 1

   if( mode == 0)
     wait = 100; // short blink on mode 0
   else if( mode == 1)
     wait = 250; // longer blink on mode 1
	  
   pinMode(13, OUTPUT);
   if(count > 10) // avoid long delays due to large count values
     count =10; 
   while(count--)
   {
     digitalWrite(13, HIGH);
     delay(wait);
     digitalWrite(13, LOW);
     delay(wait);
   }	  
}

bool moveWatchful( unsigned long distance, unsigned long duration, bool (*watchFunc)(void))
{
   unsigned long startTime = millis();
   while(true) // do until completed movement or aborted by watch function 
   {
     if(watchFunc() == true) // check for obstacles, abort if func returns true
     {
       //fp("stopped by watchFunc");
       return true;
     }     
     if( millis() - startTime > duration){  
       // fp("stopped by duration\r\n");
        break;
    } 
   }
   return false;
}

void robotMoveForward(int mode, unsigned long distance)
{
  //blink(mode, distance);
   if( mode == 0 ) // oblivious, define in minibloq.h
   {
      moveForwardWatchful(distance, ignoreWatch);
   }   
   else if ( mode == 1 ) // watchful
   {   
      moveForwardWatchful(distance, checkMovement);
   }
}

void robotMoveBackward(int mode, unsigned long distance)
{
  //blink(mode, distance);
   if( mode == 0 ) // oblivious, define in minibloq.h
   {
      moveReverseWatchful(distance, ignoreWatch);
   }   
   else if ( mode == 1 ) // watchful
   {   
      moveReverseWatchful(distance, checkMovement);
   }

}

void robotTimedMoveForward(int mode, unsigned long duration)
{

}

void robotTimedMoveBackward(int mode, unsigned long duration)
{

}

void robotMoveRotate(int mode, int angle)
{
  //blink(mode, distance);
   if( mode == 0 ) // oblivious, define in minibloq.h
   {
      moveRotateWatchful(angle, ignoreWatch);
   }   
   else if ( mode == 1 ) // watchful
   {   
      moveRotateWatchful(angle, checkMovement);
   }
}

void robotMoveRotateCCW(int mode, int angle)
{
  robotMoveRotate(mode, -angle);
}

void robotSetSpeed(int speed)
{
   moveSetSpeed(speed);
}

void robotMoveStop()
{
   moveStop();
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
 
   return lookForObstacle(obstacle);
} 
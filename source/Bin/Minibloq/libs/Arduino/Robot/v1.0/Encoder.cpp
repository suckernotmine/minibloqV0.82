/*************************************
 * Encoder.cpp 
 * Movement functions using wheel encoders
 *************************************/
#include "Encoder.h"
#include "RobotMotor.h"    // Low level motor library

#if (ARDUINO < 100)  
#include <avr/pgmspace.h>
#endif

extern bool moveFaster(int speed);
extern int moveGetSpeed();

/*************************************
 * encoder support functions
 *************************************/
const byte NBR_SAMPLES = 8; // encoders samples to average

volatile unsigned long prevMicros[2]; 

typedef struct  // holds encoder data captured in interrupt
{
  volatile unsigned long encoderCount[2];
  volatile unsigned long  encoderDur[2][NBR_SAMPLES];
  volatile byte  sampleIndex[2]; // index to current samples

} 
encoderData_t;

encoderData_t encoderData; // the raw encoder data
encoderData_t  encoderCache;   // chached copy to be used outside of interrupt routine


bool verbose = true;  // flag to turn on verbose debug info

void encoderDebug()
{
  if(verbose)
  {
    fpl("COUNTS_PER_MM= ", COUNTS_PER_MM);
    fpl("WHEEL_TRAVEL_PER_360= ", WHEEL_TRAVEL_PER_360); 
    fpl(" MM_PER_DEGREE= " , MM_PER_DEGREE);
    
    fpl("kP= ", kP);
    fpl("kD= ", kD);
    fpl("speed = ", moveGetSpeed());
    Serial.println();
  }
}

void verboseDebug(bool mode){
  verbose = mode;
}

void encoderBrake()
{
  long leftCount, rightCount;
  
  const byte BRAKING_SPEED = 50;
  if(motorState[MOTOR_LEFT] == MOTOR_DRIVE_FWD)
    motorReverse(MOTOR_LEFT,BRAKING_SPEED);
  else if(motorState[MOTOR_LEFT] == MOTOR_DRIVE_REV)
    motorForward(MOTOR_LEFT,BRAKING_SPEED); 

  if(motorState[MOTOR_RIGHT] == MOTOR_DRIVE_FWD)
    motorReverse(MOTOR_RIGHT,BRAKING_SPEED);
  else if(motorState[MOTOR_RIGHT] == MOTOR_DRIVE_REV)
    motorForward(MOTOR_RIGHT,BRAKING_SPEED);   

  noInterrupts();
  long oldLeftCount = encoderData.encoderCount[MOTOR_LEFT];
  long oldRightCount = encoderData.encoderCount[MOTOR_RIGHT];
  interrupts(); 

  unsigned long startTime = millis();   
  unsigned long MAX_BRAKING_TIME = (250 * moveGetSpeed()) / 100; 
  
  while(millis() - startTime < MAX_BRAKING_TIME)
  {     
    delay(50);
    noInterrupts();
    leftCount = encoderData.encoderCount[MOTOR_LEFT];
    rightCount = encoderData.encoderCount[MOTOR_RIGHT];
    interrupts(); 
    if( oldLeftCount == leftCount && oldRightCount == rightCount) {
      fp("stop time= "); Serial.println(millis() - startTime); 
      break;
    }
    oldLeftCount = leftCount;
    oldRightCount = rightCount;
  } 
  fpl("exit Brake with counts:",leftCount); fpl(",", rightCount); Serial.println();
}

void encoderMeasureOverrun(long target)
{
  long prevLeftCount = -1;
  long prevRightCount = -1;  


  fp("OVERRUNS:\r\n");
  while(true)
  {
    noInterrupts();
    long leftCount = encoderData.encoderCount[MOTOR_LEFT];
    long rightCount = encoderData.encoderCount[MOTOR_RIGHT];
    interrupts();
    if( prevLeftCount == leftCount && prevRightCount == rightCount) // wait for count to stop channging
    {    
      fpl("target was ", target); 
      fpl(", L/R overruns: ", leftCount - target);
      fpl(", ", rightCount - target); Serial.println();
      return;
    }
/*    
    if(verbose)
    {
      Serial.print("overrun: ");  
      Serial.print(leftCount - target); 
      Serial.print(", "); 
      Serial.println(rightCount - target);
    }
*/    
    prevLeftCount = leftCount;
    prevRightCount = rightCount;
    delay(100); 
  }
}

// temp
long kP = KP;
long kD = KD;

/*
 * these are private functions and should not be called directly
 */
void speedControl(byte motor, byte targetSpeed, unsigned long currentPulseDur)
{
  static int lastError = 0;

  // error is the difference between the target and current pulse duration
  long targetPulseDur = (MAX_SPEED_PULSE_WIDTH * 100) / targetSpeed;
  long error = currentPulseDur - targetPulseDur;
  //error = constrain(error,-1000,1000);
  int speedDelta = (error * kP)/64 + (((error-lastError)*kD) / 64);  
  //Serial.print(">> "); Serial.print(motor); Serial.print(" target= "); Serial.print(targetPulseDur); Serial.print(" , delta="); Serial.println(speedDelta);
  lastError = error;    
  int PWM = motorGetTargetPWM(targetSpeed) + speedDelta;
  PWM = constrain(PWM,0, 255);
  motorWritePWM(motor, PWM);
}

/* Call this with the left and right motors running at the desired direction * 
 * The robot will stop and the function will return under any of the following conditions:
 *  - the robot has moved (at least) the given distance in mm since the function was called
 *  - the robot has been moving for at least the given amount of time in milliseconds
 *  - the given watchFunction has returned true
 */
bool encoderMoveWatchful( unsigned long distance, unsigned long duration, bool (*watchFunc)(void))
{
  unsigned long totalCount  = 0;
  unsigned long oldLeftCount = 0;
  unsigned long oldRightCount =0;  
  unsigned long startTime = millis();

  if(moveGetSpeed() < MIN_SPEED)    
    return true; // ignore speeds too slow to move


  noInterrupts();
  encoderData.encoderCount[MOTOR_LEFT] = 0;
  encoderData.encoderCount[MOTOR_RIGHT] = 0;
  interrupts();  

 /*
   Serial.print("DBg: start of move, prev count was "); 
   Serial.print(leftCount); Serial.print(", ");  Serial.println(rightCount); Serial.println();
 */

  long myLeftDur,myRightDur;
  int myLeftPWM, myRightPWM;
  long error;
  long badReadingsLeft =0;
  long badReadingsRight = 0;
  int speed = moveGetSpeed();
 

  if(verbose == true)  {
    // print headings for CSV output
    fp("elapsedMS, left Count, err, left Dur, left PWM, right dur, right PWM");  
    Serial.println();
  }        

  while(true) // do until completed movement or aborted by watch function 
  {
    noInterrupts();
    memcpy(&encoderCache, &encoderData, sizeof(encoderCache));  // get a copy of the encoder data   
    interrupts();     

    totalCount =  encoderCache.encoderCount[MOTOR_LEFT] + encoderCache.encoderCount[MOTOR_RIGHT]; 
    error = encoderCache.encoderCount[MOTOR_LEFT] -  encoderCache.encoderCount[MOTOR_RIGHT];

    if( encoderCache.encoderCount[MOTOR_LEFT] >= oldLeftCount +NBR_SAMPLES) // wait for a full reading of new samples
    {            
      if( encoderCache.encoderCount[MOTOR_LEFT] >= NBR_SAMPLES)
      {
        long total = 0; 
        for(byte i=0; i < NBR_SAMPLES; i++)
          total += encoderCache.encoderDur[MOTOR_LEFT][i];    
        myLeftDur = total / NBR_SAMPLES;

        if(verbose) {
          // optional test to check for invalid readings      
          for(byte i=0; i < NBR_SAMPLES; i++)        {
            if(encoderCache.encoderDur[MOTOR_LEFT][i] < (myLeftDur / 2) || encoderCache.encoderDur[MOTOR_LEFT][i] > (myLeftDur * 2) )
              badReadingsLeft++;       
          } 
        }
      }
      else
        myLeftDur = encoderCache.encoderDur[MOTOR_LEFT][0]; // todo - more samples   
        
      speedControl(MOTOR_LEFT, speed, myLeftDur );

      if( encoderCache.encoderCount[MOTOR_RIGHT] >= NBR_SAMPLES)
      {
        long total = 0; 
        for(byte i=0; i < NBR_SAMPLES; i++)
          total += encoderCache.encoderDur[MOTOR_RIGHT][i];
        myRightDur = total / NBR_SAMPLES; 

        if(verbose) {
          // optional test to check for invalid readings      
          for(byte i=0; i < NBR_SAMPLES; i++)  {
            if(encoderCache.encoderDur[MOTOR_RIGHT][i] < (myRightDur / 2) || encoderCache.encoderDur[MOTOR_RIGHT][i] > (myRightDur * 2) )
              badReadingsRight++ ;          
          } 
        }

      }
      else
        myRightDur = encoderCache.encoderDur[MOTOR_RIGHT][0]; // todo - more samples  

      speedControl(MOTOR_RIGHT, speed, myRightDur );
      
      if(verbose == true)
      {
        myLeftPWM  = motorReadPWM(MOTOR_LEFT);
        myRightPWM = motorReadPWM(MOTOR_RIGHT);
        
        fpl("", millis()-startTime);
        fpl(",", encoderCache.encoderCount[MOTOR_LEFT]);
        fpl(",",  error);
        fpl(",",  myLeftDur);
        fpl(", ", myLeftPWM); 
        fpl(", ", myRightDur);
        fpl(", ", myRightPWM); 
        Serial.println();
     
      }
      oldLeftCount = encoderCache.encoderCount[MOTOR_LEFT];  
    }   


    if(watchFunc() == true) // check for obstacles, abort if func returns true
    {
      fp("stopped by watchFunc");
      return true;
    }    
    if( totalCount/2 >= distance) {
      fp("stopped by distance\r\n");
      if( badReadingsLeft || badReadingsRight){
        fpl("Bad readings: ", badReadingsLeft); 
        fpl(", ", badReadingsRight);
        Serial.println();
      }
      break;  
    }
    if( millis() - startTime > duration){  
      fp("stopped by duration\r\n");
      break;
    } 
    const unsigned long MAX_UNMOVING_TIME = 750;    
    if( millis() - startTime > MAX_UNMOVING_TIME)
    {
      if(encoderCache.encoderCount[MOTOR_LEFT] < 8 || encoderCache.encoderCount[MOTOR_RIGHT] < 8)
      { 
        if ( moveFaster(5) == false){ // increase the speed 
          fp("stopped because not moving\r\n");
          break;
        }
        fpl("speed increased to ",moveGetSpeed());
        Serial.println();
        startTime = millis(); // reset the start time
        //// ???leftCount = rightCount = 0;
      }
    }
  }
  // show final values
  fpl("Left count= ", encoderCache.encoderCount[MOTOR_LEFT]); 
  fpl(", err= ", error);
  fpl(", L dur= ", myLeftDur);
  fpl(", R dur ", myRightDur); 
  Serial.println();

  //moveBrake();
  return false;

}


/*
 * encoder ISRs
 */

// TODO - make inline? 
void encoderService( byte motor) 
{
  static byte sampleIndex[2];

  long dur = micros() - prevMicros[motor];
  if(dur > BOUNCE_TIME) // ignore bounce
  {
    prevMicros[motor] = micros(); 
    encoderData.encoderDur[motor][sampleIndex[motor]++] = dur; 
    encoderData.encoderCount[motor] = encoderData.encoderCount[motor] +1;  

    if(sampleIndex[motor] >= NBR_SAMPLES)
      sampleIndex[motor] = 0;     
  }
}

void encoderServiceLeft()
{
  encoderService(MOTOR_LEFT);
}

void encoderServiceRight()
{
  encoderService(MOTOR_RIGHT); 
}

// begin function that enables encoder servicing
// see http://arduino.cc/en/Reference/attachInterrupt for interrupt number info
void encoderBegin( int LeftInterrupt, int RightInterrupt)
{
  attachInterrupt(LeftInterrupt, encoderServiceLeft, RISING);
  attachInterrupt(RightInterrupt, encoderServiceRight, RISING);
  fp("Interrupts enabled\r\n");  
}




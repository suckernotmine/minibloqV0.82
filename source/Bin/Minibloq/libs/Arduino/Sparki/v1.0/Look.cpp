/**********************
 * code to look for obstacles
 **********************/
 #include "Look.h"
 #include "Move.h"
 #include "SoftServo.h"
// #include "RobotDefines.h"
 
 
// obstacles constants 
const int OBST_NONE       = 0;  // no obstacle detected
const int OBST_LEFT_EDGE  = 1;  // left edge detected 
const int OBST_RIGHT_EDGE = 2;  // right edge detected
const int OBST_FRONT_EDGE = 3;  // edge detect at both left & right sensors
const int OBST_FRONT      = 4;  // obstacle in front
const int OBST_REAR       = 5;  // obstacle behind 

// defines for locations of sensors
const int SENSE_LEFT  = 0;
const int SENSE_RIGHT = 1;

// defines for directions
const int DIR_LEFT   = 0;
const int DIR_RIGHT  = 1;
const int DIR_CENTER = 2;

// servo defines
const int sweepServoPin = 9;  // pin connected to servo
const int servoDelay    = 500; // time in ms for servo to move

const int  MIN_DISTANCE = 20;     // robot stops when object is nearer (in cm)
const int  CLEAR_DISTANCE = 50;  // distance in cm considered attractive to move
const int  MAX_RANGE = 200;   // the maximum range of the distance sensor

// servo angles             left, right, center 
const int servoAngles[] = {150,  30,   90};

const int pingPin =  8; // digital pin 8

const byte bumpSensorPin[2] = { 10,11};

int lastObstacle = OBST_NONE;  

bool lookStarted = false;

void lookBegin()
{
   
#if (ARDUINO <= 100)
  pinMode(bumpSensorPin[SENSE_LEFT], INPUT);    // initialize sensors
  digitalWrite(bumpSensorPin[SENSE_LEFT], HIGH);    // turn on pull-ups
  pinMode(bumpSensorPin[SENSE_RIGHT], INPUT);    // initialize sensors 
  digitalWrite(bumpSensorPin[SENSE_RIGHT], HIGH);    // initialize sensors 
#else
  pinMode(bumpSensorPin[SENSE_LEFT], INPUT_PULLUP);    // initialize sensors 
  pinMode(bumpSensorPin[SENSE_RIGHT], INPUT_PULLUP);    // initialize sensors 
#endif  
  softServoAttach(sweepServoPin);  /// attaches the servo pin to the servo object 
}

// returns true if the given obstacle is detected
boolean lookForObstacle(int obstacle)
{
/*  
  if(obstacle == OBST_FRONT)
  {
    int d = lookAt(servoAngles[DIR_CENTER]);
   Serial.print("d=");  Serial.print(d); Serial.print ( " is "); Serial.println( d < MIN_DISTANCE);
  }
*/  
  if(lookStarted == false) {
    lookBegin();
	lookStarted = true;	
	Serial.println("look started");
  }
  
/*   
   if(digitalRead(bumpSensorPin[SENSE_LEFT]) == LOW) 
     Serial.println("left obstacle "); 
	if(digitalRead(bumpSensorPin[SENSE_RIGHT]) == LOW) 
     Serial.println("right obstacle "); 
*/  
  switch(obstacle) {
  case  OBST_FRONT_EDGE: 
    return digitalRead(bumpSensorPin[SENSE_LEFT]) == LOW &&
      digitalRead(bumpSensorPin[SENSE_RIGHT]) == LOW; 
  case  OBST_LEFT_EDGE:  
    return digitalRead(bumpSensorPin[SENSE_LEFT]) == LOW; 
  case  OBST_RIGHT_EDGE: 
    return  digitalRead(bumpSensorPin[SENSE_RIGHT]) == LOW; 
  case  OBST_FRONT:  int d = lookAt(servoAngles[DIR_CENTER]); return  d < MIN_DISTANCE;      
  }
  return false; 
}

// returns the distance of objects at the given angle
// this version rotates the robot
int lookAt(int angle)
{
  softServoWrite(angle, servoDelay ); // wait for servo to get into position

  int distance, samples;
  long cume;
  distance = samples = cume = 0;  
  for(int i =0; i < 4; i++)
  {  
    distance = HC_SR04GetDistance(pingPin);
    if(distance > 0)
    {     
      samples++;
      cume+= distance;
    }  
  }    
  if(samples > 0)
    distance = cume / samples;
  else
    distance = 0;  
  //Serial.print(" cm = "); Serial.println(distance);
  if( angle != servoAngles[DIR_CENTER])
  {
 /* 
    SerialDisplay.print("looking at dir "); 
    SerialDisplay.print(angle), SerialDisplay.print(" distance= "); 
    SerialDisplay.println(distance); 
*/	
    softServoWrite(servoAngles[DIR_CENTER], servoDelay/2);    
  } 
  return distance;  
}

// function to check if robot can continue moving in current direction 
// returns true if robot is blocked moving in current direction
// this version only tests for obstacles in front
bool checkMovement()
{
  if(Serial.read() == 'h')
    return true;  // abort on serial command

  bool isBlocked = false; // default return value if no obstacles
  if(moveGetState() == MOV_FORWARD)
  {
    if(lookForObstacle(OBST_FRONT) == true) {    
      Serial.println("Front Blocked"); 
      isBlocked = true;   
    }
    else if(lookForObstacle( OBST_LEFT_EDGE) == true) {    
      Serial.println("Bumped Left Front"); 
      isBlocked = true;   
    }
    else if(lookForObstacle( OBST_RIGHT_EDGE) == true) {    
      Serial.println("Bumped Right Front"); 
      isBlocked = true;   
    }
    else if(lookForObstacle( OBST_FRONT_EDGE) == true) {    
      Serial.println("Bumped Front"); 
      isBlocked = true;   
    }

  }
  return isBlocked; 
}


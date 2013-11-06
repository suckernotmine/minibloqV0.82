// Encoder.h

#ifndef ENCODER_H
#define ENCODER_H

#if (ARDUINO >= 100)
 #include <Arduino.h>
#else
 #include <WProgram.h>
#endif

extern long kP;
extern long kD;

extern void flashprint (const char p[]);
#define fp(string) flashprint(PSTR(string));
#define fpl(string, long) flashprint(PSTR(string)); Serial.print(long);
#define fpln(string, long) flashprint(PSTR(string)); Serial.println(long);

static char _buf[64];
#define printf(...) 						\
	do {							\
		sprintf(_buf, __VA_ARGS__); Serial.write(_buf);	\
	} while (0)
	

// properties about the robot
const float WHEEL_DIAMETER_MM      = 124.0;
const float WHEEL_CIRCUMFERENCE_MM = WHEEL_DIAMETER_MM * 3.1416;
const float WHEEL_TRACK            = 275.0;              //tyre seperation in mm  
const float FRICTION_FACTOR        = 1.2;                //increase movement to counteract rotational friction
const float WHEEL_TRAVEL_PER_360   = WHEEL_TRACK * 3.1416 * FRICTION_FACTOR;  // mm per robot revolution 

const int   COUNTS_PER_MOTOR_REV = 13; // teeth on encoder wheel (13 gives 1 count per mm)
const int   GEARING              = 30;
const int   COUNTS_PER_REV       = COUNTS_PER_MOTOR_REV * GEARING;
const float COUNTS_PER_MM        = COUNTS_PER_REV / WHEEL_CIRCUMFERENCE_MM;
const float COUNTS_PER_CM        = (COUNTS_PER_REV / WHEEL_CIRCUMFERENCE_MM) * 10;
const float MM_PER_DEGREE        = WHEEL_TRAVEL_PER_360 / 360.0; 


const int BOUNCE_TIME            = 25;  // pulse duration must be greater than this to be valid

const long KP = 24; //
const long KD = 4;
const char INTEGRATION_TIME = 20; // min time in MS between samples

const unsigned long MAX_SPEED_PULSE_WIDTH = 800; //  1.25 meter per second (~3 miles per hour)

const unsigned long MAX_TIME     = 4294967295; // just under 50 days
const unsigned long MAX_DISTANCE = 99999999;   // very far

// functions in Encoder module
void encoderBegin( int LeftInterrupt, int RightInterrupt);
void encoderSetSpeed(int speed);
bool moveForwardWatchful( unsigned long distanceCM, bool (*watchFunc)(void));
bool moveReverseWatchful( unsigned long distanceCM, bool (*watchFunc)(void));
bool moveRotateWatchful( int angle, bool (*watchFunc)(void));
bool moveForwardTimedWatchful( unsigned long durationMS, bool (*watchFunc)(void));
bool moveReverseTimedWatchful( unsigned long durationMS, bool (*watchFunc)(void));
bool delayWatchful(unsigned long interval, bool (*watchFunc)(void));
void encoderBrake();

void encoderDebug();

bool encoderMoveWatchful( unsigned long distanceMM, unsigned long duration, bool (*watchFunc)(void));
void encoderMeasureOverrun(long target);
void verboseDebug(bool mode);

#endif

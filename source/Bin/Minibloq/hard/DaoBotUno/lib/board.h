#ifndef board_h
#define board_h

// TODO - this should be included from DaoBot.h !!!

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


extern int lookAt(int angle);
extern boolean lookForObstacle(int obstacle);

extern void robotBegin(int encoder0Pin, int encoder1Pin);

extern void robotMoveForward(int mode, unsigned long distance);
extern void robotMoveBackward(int mode, unsigned long distance);
extern void robotTimedMoveForward(int mode, unsigned long duration);
extern void robotTimedMoveBackward(int mode, unsigned long duration);
extern void robotMoveRotate(int mode, int angle);
extern void robotMoveRotateCCW(int mode, int angle);
extern void robotSetSpeed(int speed);
extern void robotMoveStop();


#define BuzzerPin 12

#define serial0 Serial
#define serial1 Serial1

#define sensor0  A0
#define sensor1  A1
#define sensor2  A2
#define sensor3  A3
#define sensor4  A4
#define sensor5  A5
		
#define PWM3   3
#define PWM5   5
#define PWM6   6
#define PWM9   9
#define PWM10   10
#define PWM11  11
		
#define D0  0
#define D1  1
#define D2  2
#define D3  3
#define D4  4
#define D5  5
#define D6  6
#define D7  7
#define D8  8
#define D9  9
#define D10 10
#define D11 11
#define D12 12
#define D13_LED 13
#define D14 14
#define D15 15
#define D16 16
#define D17 17
#define D18 18

void initBoard();

#endif

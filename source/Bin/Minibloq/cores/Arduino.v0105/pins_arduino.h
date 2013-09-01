/*
  pins_arduino.h - Select pin definition headers for Arduino variants
  Michael Margolis 31 Auguat 2013
  
  The following header file names are derived from the variants directory names in the arduino distribution
  the contents of the headers are exactly as in each variant directory
*/


#ifndef Pins_Variants_h
#define Pins_Variants_h

#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
#include "pins_mega.h"
#elif defined(__AVR_ATmega32U4__)  
#include "pins_leonardo.h" 
#else
#include "pins_standard.h"
#endif

#endif

// hardwareHelper.h

#ifndef _HARDWAREHELPER_h
#define _HARDWAREHELPER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

void checkFlash(); 

#endif


#ifndef CLOCK_H
#define CLOCK_H
#include <Arduino.h>
#include <TimerOne.h>

class Clock {

public:
  int steps;             // how many steps before loop resets
  int currStep;          // the current step in the loop
  long pulseDuration;    // how long, in microseconds, the clock led will be lit
  long stepDuration;     // how long, in microseconds, a single step lasts before the next step begins. Will be variable based on clock input
  long loopStart;        // time when the first step occured on the system clock
  long lastClock;        // time of the last clocked event

  void init(int _steps);
  void advanceClock();
  void detectTempo();

};


#endif

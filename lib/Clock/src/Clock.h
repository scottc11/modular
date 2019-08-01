#ifndef CLOCK_H
#define CLOCK_H
#include <Arduino.h>
#include <TimerOne.h>

class Clock {

public:
  int timesClocked;      // how many times an external clock signal has been detected
  int steps;             // how many steps before loop resets
  int currStep;          // the current step in the loop
  long pulseDuration;    // how long, in microseconds, the clock led will be lit
  long stepDuration;     // how long, in microseconds, a single step lasts before the next step begins. Will be variable based on clock input
  long loopStart;        // time when the first step occured on the system clock
  long loopDuration;     // (stepDuration * steps)
  long oldLoopDuration;  // for tracking tempo changes
  long lastClock;        // time of the last clocked event
  long lastExtClock;     // time of last detected external clock signal


  int PPQN;              // pulses-per-quarter-note

  void init(int _steps);
  void advanceClock();
  void detectTempo();
  bool hasChangedTempo();

};


#endif

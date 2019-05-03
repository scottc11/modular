#include <Clock.h>

void Clock::init(int _steps) {
  this->currStep = 1;
  this->steps = _steps;
  this->pulseDuration = 20000;
  this->stepDuration = 500000; // initialize @ 120 BPM
}

void Clock::advanceClock() {

  this->lastClock = micros();

  if (this->currStep == 1) {
    this->loopStart = this->lastClock;
  }

  // increment this->currStep by 1
  if (this->currStep < steps) { this->currStep += 1; }
  else { this->currStep = 1; }
}


void Clock::detectTempo() {

  long now = micros();
  long newStepDuration = now - this->lastClock;
  this->lastClock = now;

  if (newStepDuration != this->stepDuration) {
    // tell timer to trigger callback at an interval if newStepDuration / PPQ
    Timer1.setPeriod(newStepDuration);
  }

  this->stepDuration = newStepDuration;
}

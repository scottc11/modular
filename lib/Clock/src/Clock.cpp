#include <Clock.h>

void Clock::init(int _steps) {
  this->timesClocked = 0;
  this->currStep = 1;
  this->steps = _steps;
  this->pulseDuration = 20000;
  this->stepDuration = 500000; // initialize @ 120 BPM
  this->lastClock = micros();
  this->lastExtClock = micros();
}

void Clock::advanceClock() {

  this->lastClock = micros();

  if (this->currStep == 1) {
    this->loopStart = this->lastClock;
  }

  // increment this->currStep by 1
  if (this->currStep < this->steps) { this->currStep += 1; }
  else { this->currStep = 1; }
}


// via an external analog clock, detect the duration between clock signals and
// update the timer interupt to the new interval

void Clock::detectTempo() {

  long now = micros();
  long newStepDuration = now - this->lastExtClock;
  this->lastExtClock = now;

  if (this->timesClocked > 2) {
    if (newStepDuration != this->stepDuration) {
      // tell timer to trigger callback at an interval if newStepDuration / PPQ

      this->stepDuration = newStepDuration;
      this->oldLoopDuration = this->loopDuration;
      this->loopDuration = this->stepDuration * this->steps;
      Timer1.setPeriod(newStepDuration);
    }
  }

  this->timesClocked++;
}


// check if the loop duration has increased or decreased
bool Clock::hasChangedTempo() {
  if (this->loopDuration != this->oldLoopDuration) {
    return true;
  } else {
    return false;
  }
}

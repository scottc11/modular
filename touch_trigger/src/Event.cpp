#include<stdio.h>
#include<stdlib.h>
using namespace std;
#include "Event.h"

// the positionRatio should not change once it is initialized. You will only use the
// value to timeStretch events when the loop duration changes.
void Event::calculateRatio(long loopDuration) {
  this->positionRatio = (this->position / loopDuration) * 100;
}

// if we have the positionRatio, and the new loopDuration, we should be able to calculate position
void Event::calculatePosition(long loopDuration) {
  this->position = (this->positionRatio / 100) * loopDuration;
}


void deleteAllEvents(Event *e) {
  Event *store;
  while (e != NULL) {
    store = e->next;
    delete e;
    e = store;
  }
}

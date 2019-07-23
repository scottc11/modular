#ifndef EVENT_H
#define EVENT_H

#include<stdio.h>
#include<stdlib.h>
using namespace std;

class Event {
  public:
  int channel;         // which channel should be used for IO
  long duration;       // the duration of the event in microseconds
  long position;       // the point in time in which the event occured
  bool triggered;      // has the event been triggered
  Event *next;         // pointer to the 'next' Event to occur (linked list)
  Event *prev;         // pointer to the 'previous' Event to occur (linked list)

  Event(int chan, long pos, long dur) {
    this->channel = chan;
    this->position = pos;
    this->duration = dur;
    this->triggered = false;
    this->next = NULL;
    this->prev = NULL;
  }
};

void deleteAllEvents(Event *e);

#endif

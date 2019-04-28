#include<stdio.h>
#include<stdlib.h>
using namespace std;
#include "Event.h"


void deleteAllEvents(Event *e) {
  Event *store;
  while (e != NULL) {
    store = e->next;
    delete e;
    e = store;
  }
}

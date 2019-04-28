#include <Arduino.h>
#include <Adafruit_MCP23017.h>
#include <TimerOne.h>
#include <Adafruit_MPR121.h>
#include <Event.h>
#include <Channel.cpp>

#define IO_ADDR 0x00
#define TOUCH_ADDR 0x5A

Adafruit_MCP23017 io = Adafruit_MCP23017();
Adafruit_MPR121 touch = Adafruit_MPR121();

const bool DEBUG = true;
const int CLOCK_INTERUPT_PIN = 2;
const int CLOCK_LED_PIN = 4;
const int LOOP_START_LED_PIN = 5;


int currentStep = 1;
int steps = 8;                   // how many steps before sequencer loop resets
int clocked = 0;                 // how many times externally clocked
long pulseDuration = 20000;      // how long, in microseconds, the clock led will be lit
long stepDuration = 500000;      // how long, in microseconds, a single step lasts before the next step begins. Will be variable based on clock input
long timeOfLoopStart = 0;        // when the first step occured on the system clock

// RECORDING TOUCH SEQUENCE
Event * current;
Event * HEAD;
Event * QUEUED;

// MICROSECOND RECORDERS
long timeOfLastClock = 0;        //
long timeOfLastTouchA = 0;       // when channel A was last touched in microseconds
long timeOfLastReleaseA = 0;     // when channel A was last released in microseconds
bool triggered = false;          // determin if channel A has already been triggered/set HIGH

// RESET BUTTONS
int newResetButtonState = 0;
int prevResetButtonState = 0;

uint16_t lastTouched = 0;
uint16_t currTouched = 0;

const int CHANNEL_A_LED_PIN = 8;   // via io

// Timer1 Interupt Callback
void advanceClock() {

  timeOfLastClock = micros();  // the current time
  if (currentStep == 1) {
    timeOfLoopStart = timeOfLastClock;
    // Serial.print("timeOfLoopStart: ");Serial.println(timeOfLoopStart);
  }
  // increment currentStep by 1
  if (currentStep < steps) {
    currentStep += 1;
  } else {
    currentStep = 1;
  }
}

volatile long lastPulseInterval = 0;

void detectTempo() {

  long now = micros();
  long newPulseInterval = now - timeOfLastClock;
  timeOfLastClock = now;

  if (newPulseInterval != lastPulseInterval) {
    // tell timer to trigger callback at an interval if newPulseInterval / PPQ
    Timer1.setPeriod(newPulseInterval);
  }

  lastPulseInterval = newPulseInterval;
}

void setup() {
  Serial.begin(9600);
  // Connect MPR121 touch sensors
  if (!touch.begin(TOUCH_ADDR)) { Serial.println("MPR121 not found, check wiring?"); while (1); }

  io.begin(IO_ADDR);

  // create interupt to detect external clock tempo
  attachInterrupt(digitalPinToInterrupt(CLOCK_INTERUPT_PIN), detectTempo, FALLING);

  Timer1.initialize(stepDuration); // initialize @ 120 BPM
  Timer1.attachInterrupt(advanceClock);



  pinMode(CLOCK_LED_PIN, OUTPUT);
  digitalWrite(CLOCK_LED_PIN, LOW);
  pinMode(LOOP_START_LED_PIN, OUTPUT);
  digitalWrite(LOOP_START_LED_PIN, LOW);

  io.pinMode(CHANNEL_A_LED_PIN, OUTPUT);
  io.digitalWrite(CHANNEL_A_LED_PIN, LOW);
  Serial.println("::: Setup Complete :::");
}



// ------------------------------------
// LOOP
// ------------------------------------
void loop() {

  long now = micros();                       // used throughout loop() function
  currTouched = touch.touched();             // Get the currently touched pads
  newResetButtonState = io.digitalRead(7);   // detect reset buttons

  // --------- HANDLE RESET BUTTON ---------
  if (newResetButtonState != prevResetButtonState) {
    if (newResetButtonState == HIGH) {
      Event *tmp;
      while (HEAD != NULL) {
        tmp = HEAD;
        HEAD = HEAD->next;
        delete tmp;
      }
      QUEUED = NULL;
      if (DEBUG && HEAD == NULL) { Serial.println("all events deleted"); }
    }
    prevResetButtonState = newResetButtonState;
  }

  // ------ CLOCK INDICATORS -----
  // indicate tempo and loop start position via LEDs
  if (now - pulseDuration < timeOfLastClock) {
    digitalWrite(CLOCK_LED_PIN, HIGH);
    if (currentStep == 1) {digitalWrite(LOOP_START_LED_PIN, HIGH);}
  } else {
    digitalWrite(CLOCK_LED_PIN, LOW);
    if (currentStep == 1) {digitalWrite(LOOP_START_LED_PIN, LOW);}
  }




  // ------ HANDLE TOUCH EVENTS -----
  for (uint8_t i=0; i<4; i++) {

    // if it *is* touched and *wasnt* touched before
    if ( (currTouched & _BV(i) ) && !( lastTouched & _BV(i) ) ) {
      io.digitalWrite(CHANNEL_A_LED_PIN, HIGH);

      // get the current time of touch relative to timeOfLoopStart
      timeOfLastTouchA = now - timeOfLoopStart;

      Serial.print("timeOfLastTouchA: ");Serial.println(timeOfLastTouchA);
    }

    //  if it *was* touched and now *isnt*
    if (!(currTouched & _BV(i)) && (lastTouched & _BV(i)) ) {
      io.digitalWrite(CHANNEL_A_LED_PIN, LOW);

      timeOfLastReleaseA = now - timeOfLoopStart;
      long position = timeOfLastTouchA;
      long duration = timeOfLastReleaseA - timeOfLastTouchA;

      // create new event
      Event *newEvent = new Event(1, position, duration);

      if (HEAD == NULL) {
        Serial.println("head is NULL");
        HEAD = newEvent;
        QUEUED = HEAD;
      }

      else if (HEAD->next == NULL) {
        HEAD->next = newEvent;
      }

      // iterate through all events until there are no more remaining
      if (HEAD->next != NULL) {

        Event * cur = HEAD;
        while (cur != NULL) {

          // if greater than cur->next->position
          if (newEvent->position > cur->position && cur->next == NULL) {
            cur->next = newEvent;
            Serial.println(" <-put after last event in loop-> ");
            break;
          }

          // greater than cur->position and less than cur->next->position
          else if (newEvent->position > cur->position && newEvent->position < cur->next->position) {
            newEvent->next = cur->next;
            cur->next = newEvent;
            Serial.println(" <-put in between two events-> ");
            break; // we can break out of loop now that the new events location in linked list has been determined
          }

          // if less than head node, mark as head
          else if (newEvent->position < cur->position) {
            newEvent->next = cur;
            HEAD = newEvent; // ie. HEAD
            Serial.println(" <-set event as head-> ");
            break;
          }

          cur = cur->next;  // set the iterator pointer to the next event in linked list
        }
      }
    }
  }

  // HANDLE THE QUEUED EVENT
  if (QUEUED) {
    long loopStart = now - timeOfLoopStart;

    if (!QUEUED->triggered) { // ifnext event has not yet been triggered
      if (loopStart >= QUEUED->position && loopStart < QUEUED->position + QUEUED->duration) {
        io.digitalWrite(CHANNEL_A_LED_PIN, HIGH);
        QUEUED->triggered = true;
      }
    }

    else if (loopStart > QUEUED->position + QUEUED->duration) {
      io.digitalWrite(CHANNEL_A_LED_PIN, LOW);
      QUEUED->triggered = false;

      // if there IS an event after QUEUED, set QUEUED to that event
      if (QUEUED->next) { QUEUED = QUEUED->next; }

      // if there are no more events, set the QUEUED event equal to the HEAD (ie. first event in loop)
      else { QUEUED = HEAD; }
    }
  }

  // reset touch sensors state
  lastTouched = currTouched;

}

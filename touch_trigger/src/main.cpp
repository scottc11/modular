#include <Arduino.h>
#include <Adafruit_MCP23017.h>
#include <TimerOne.h>
#include <Adafruit_MPR121.h>
#include <Encoder.h>
#include <Button.h>
#include <Clock.h>
#include <Event.h>
#include <Channel.cpp>

// To use interrupts, you must include the AVR interrupt header.
#include <avr/io.h>
#include <avr/interrupt.h>

#define IO_ADDR 0x00
#define TOUCH_ADDR 0x5A

const bool DEBUG = true;
const int CLOCK_INTERUPT_PIN = 2;
const int CLOCK_LED_PIN = 4;
const int LOOP_START_LED_PIN = 5;
const int RESET_PIN = 7;
const int CHANNEL_A_LED_PIN = 8;   // via io
const int ENCODER_PIN_A = 10;
const int ENCODER_PIN_B = 11;

Adafruit_MCP23017 io = Adafruit_MCP23017();
Adafruit_MPR121 touch = Adafruit_MPR121();
Button resetBtn = Button(RESET_PIN);
Clock clock;
Encoder encoder(ENCODER_PIN_A, ENCODER_PIN_B);

// RECORDING TOUCH SEQUENCE
Event * current;                 // variable not being used
Event * HEAD;                    // I believe this stands for the first event in the loop.
Event * QUEUED;

// MICROSECOND RECORDERS
long timeOfLastTouchA = 0;       // when channel A was last touched in microseconds
long timeOfLastReleaseA = 0;     // when channel A was last released in microseconds
bool triggered = false;          // determin if channel A has already been triggered/set HIGH

uint16_t lastTouched = 0;
uint16_t currTouched = 0;
long encoderPosition = 0;

void logClock() {
  clock.detectTempo();
}

void advanceClock() {
  clock.advanceClock();
}

void setup() {
  Serial.begin(9600);
  // Connect MPR121 touch sensors
  if (!touch.begin(TOUCH_ADDR)) { Serial.println("MPR121 not found, check wiring?"); while (1); }

  io.begin(IO_ADDR);

  clock.init(8);
  // create interupt to detect external clock tempo
  attachInterrupt(digitalPinToInterrupt(CLOCK_INTERUPT_PIN), logClock, RISING);

  // using interupts, advance the clock instance forward
  Timer1.initialize(clock.stepDuration);
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

  long now = micros();                                // used throughout loop() function
  currTouched = touch.touched();                      // Get the currently touched pads
  resetBtn.newState = io.digitalRead(resetBtn.pin);   // detect reset buttons
  long newEncoderPos = encoder.read();                // read the position of the encoder

  if (newEncoderPos != encoderPosition) {
    Serial.print("encoder changed!: ");Serial.println(newEncoderPos);
    encoderPosition = newEncoderPos;
  }


  // --------- HANDLE RESET BUTTON ---------
  if (resetBtn.changedState()) {
    if (resetBtn.isPressed() == HIGH) {
      Event *tmp;
      while (HEAD != NULL) {   // while there are events
        tmp = HEAD;            // set the first event to a temp variable
        HEAD = HEAD->next;     // assign the head event to the event that occurs after it
        delete tmp;            // delete the event
      }
      QUEUED = NULL;
      if (DEBUG && HEAD == NULL) { Serial.println("all events deleted"); }
    }
    resetBtn.updateState();    // this method should probably be called in btn.changedState() method
  }

  // ------ CLOCK INDICATORS -----
  // indicate tempo and loop start position via LEDs
  if (now - clock.pulseDuration < clock.lastClock) {
    digitalWrite(CLOCK_LED_PIN, HIGH);
    if (clock.currStep == 1) {
      // log all events
      if (HEAD != NULL && HEAD->next != NULL) {
        Event * iterator = HEAD;
        while (iterator != NULL) {
          Serial.print("[ ");Serial.print(iterator->position);Serial.print(" ]");
          iterator = iterator->next;
        }
        Serial.println("");
      }
      digitalWrite(LOOP_START_LED_PIN, HIGH);
    }
  } else {
    digitalWrite(CLOCK_LED_PIN, LOW);
    if (clock.currStep == 1) {digitalWrite(LOOP_START_LED_PIN, LOW);}
  }


  // BUG: there is a bug when you create an event with a duration that overlaps the end and start of the loop.
  // If you only have this one event, the code will continue to run as expected, but adding any events after
  // this will cause the application to crash (it officially crashes once the loop resets).

  // ------ HANDLE TOUCH EVENTS -----
  for (uint8_t i=0; i<4; i++) {

    // if it *is* touched and *wasnt* touched before
    if ( (currTouched & _BV(i) ) && !( lastTouched & _BV(i) ) ) {

      io.digitalWrite(CHANNEL_A_LED_PIN, HIGH);         // turn channel LED on

      timeOfLastTouchA = now - clock.loopStart;         // get the current time of touch relative to clock.loopStart

      // Serial.print("timeOfLastTouchA: ");Serial.println(timeOfLastTouchA);
    }

    //  if it *was* touched and now *isnt*
    if (!(currTouched & _BV(i)) && (lastTouched & _BV(i)) ) {
      io.digitalWrite(CHANNEL_A_LED_PIN, LOW);

      timeOfLastReleaseA = now - clock.loopStart;
      long position = timeOfLastTouchA;
      long duration = timeOfLastReleaseA - timeOfLastTouchA;   // calulate duration of event based on touchOn and touchOff

      // create new event
      Event *newEvent = new Event(1, position, duration);
      newEvent->calculateRatio(clock.loopDuration);

      // handle adding the first two events to the event list.
      // should be refractored.
      if (HEAD == NULL) {
        Serial.println("head is NULL");
        HEAD = newEvent;
        QUEUED = HEAD;
      }

      else if (HEAD->next == NULL) {
        HEAD->next = newEvent;
      }

      // -------- ADD + ORDER --------
      // iterate through all events and place the new event in the list based on the events
      // position. ie. Add the new event to the event list and order.
      if (HEAD->next != NULL) {

        Event * cur = HEAD;
        while (cur != NULL) {

          // if greater than cur->next->position
          if (newEvent->position > cur->position && cur->next == NULL) {
            cur->next = newEvent;
            // Serial.println(" <-put after last event in loop-> ");
            break;
          }

          // greater than cur->position and less than cur->next->position
          else if (newEvent->position > cur->position && newEvent->position < cur->next->position) {
            newEvent->next = cur->next;
            cur->next = newEvent;
            // Serial.println(" <-put in between two events-> ");
            break; // we can break out of loop now that the new events location in linked list has been determined
          }

          // if less than head node, mark as head
          else if (newEvent->position < cur->position) {
            newEvent->next = cur;
            HEAD = newEvent; // ie. HEAD
            // Serial.println(" <-set event as head-> ");
            break;
          }

          cur = cur->next;  // set the iterator pointer to the next event in linked list
        }
      }
    }
  }

  // HANDLE THE QUEUED EVENT
  if (QUEUED) {

    if (clock.hasChangedTempo()) {
      Serial.print(clock.loopDuration);Serial.print(" ::::: ");Serial.println(clock.oldLoopDuration);
      // QUEUED->calculatePosition(clock.loopDuration);
    }

    long loopStart = now - clock.loopStart;

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

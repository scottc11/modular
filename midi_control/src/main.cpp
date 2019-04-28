#include <Arduino.h>
#include <MIDI.h>
#include <Wire.h>
#include <Adafruit_MCP23017.h>
#include <TimerOne.h>

#define ENCODER_DO_NOT_USE_INTERRUPTS // so Encoder library doesn't use interupts
#include <Encoder.h>


// Creates and binds the MIDI interface to the default hardware Serial port
MIDI_CREATE_DEFAULT_INSTANCE();

// MIDI clock, start and stop byte definitions - based on MIDI 1.0 Standards.
#define MIDI_CLOCK 0xF8
#define MIDI_START 0xFA
#define MIDI_STOP  0xFC
#define PPQ 24

#define IO_ADDR 0x00

const int MIDI_START_PIN = 8;
const int MIDI_STOP_PIN = 9;
const int MIDI_CHANNEL = 1;
const bool DEBUG = false;

const int CHANNEL_SELECT_PIN = 11;
const int NUM_TRIG_INPUTS = 4;         // how many trigger inputs the software will accept

Adafruit_MCP23017 io = Adafruit_MCP23017();

Encoder encoder(8, 12);

// Trigger Input Pins
int trigger_input_pins[] = {4, 5, 6, 7};
int channel_leds[] = {0, 1, 2, 3};

int triggerMIDIValues[] = {21, 37, 53, 77};

int counter[] = {0, 0, 0, 0}; // for encoder ?

// button states
byte newInputStates = 0;
byte oldInputStates = 0;

long oldEncoderPos = -999;
int newEndcoderPushState = 0;
int oldEndcoderPushState = 0;


int selected_channel = 0;        // what trigger the encoder will be acting on.

bool started = false;  // tell MIDI slave to advance

int startNewState = 0;
int startOldState = 0;

int stopNewState = 0;
int stopOldState = 0;

volatile long timeOfFirstClock = 0;
volatile long timeOfLastClock = 0;
volatile long timesClocked = 0;
volatile long lastPulseInterval = 0;
volatile long minimumPulseInterval = 200000;
int bpm = 0;


// Start the slave MIDI device
void startClock() {
  if (DEBUG) { Serial.println("------ START CLOCK -------"); }
  started = true;
  timesClocked = 0; // reset the number of trigger 'timesClocked' the circadian has sent
  Serial.write(MIDI_START);
}


// Stop the slave MIDI device
void stopClock() {
  if (DEBUG) { Serial.println("------ STOP CLOCK -------"); }
  Serial.write(MIDI_STOP);
  started = false;
  timesClocked = 0; // reset the number of trigger 'timesClocked' the circadian has sent
}





void detectTempo() {

  if (timesClocked % 4 == 1) {

    long now = micros();
    long newPulseInterval = now - timeOfLastClock;
    timeOfLastClock = now;

    if (newPulseInterval != lastPulseInterval) {
      // tell timer to trigger callback at an interval if newPulseInterval / PPQ
      Timer1.setPeriod(newPulseInterval / PPQ);
    }

    lastPulseInterval = newPulseInterval;
  }

  timesClocked++;
}

// Interupt Callback Function
void sendClockPulse() {
  Serial.write(MIDI_CLOCK);
}



void setup() {
  // put your setup code here, to run once:
  if (!DEBUG) {
    Serial.begin(31250);
    MIDI.begin(MIDI_CHANNEL_OMNI);  // Listen to all incoming messages
  } else {
    Serial.begin(9600);
  }


  // DIGITAL PIN 2 is used for detecting tempo via an interrupt
  attachInterrupt(digitalPinToInterrupt(2), detectTempo, FALLING);


  Timer1.initialize(500000); // initialize @ 120 BPM
  Timer1.attachInterrupt(sendClockPulse);

  io.begin(IO_ADDR);

  // init IO LEDS
  for (uint8_t i = 0; i < NUM_TRIG_INPUTS; i++) {
    io.pinMode(channel_leds[i], OUTPUT);

    if (i == selected_channel) {
      io.digitalWrite(channel_leds[i], HIGH);
    } else {
      io.digitalWrite(channel_leds[i], LOW);
    }
  }

  // MIDI start stop buttons
  io.pinMode(MIDI_START_PIN, INPUT);
  io.pinMode(MIDI_STOP_PIN, INPUT);

  // INIT TRIGGER INPUTS
  for (uint8_t i = 0; i < NUM_TRIG_INPUTS; i++) {
    pinMode(trigger_input_pins[i], INPUT);
  }
}





void loop() {

  if (io.digitalRead(MIDI_START_PIN) == HIGH && !started) {
    startClock();
  }

  if (io.digitalRead(MIDI_STOP_PIN) == HIGH && started) {
    stopClock();
  }

  long newEncoderPos = encoder.read();

  newEndcoderPushState = digitalRead(CHANNEL_SELECT_PIN);

  if (newEndcoderPushState != oldEndcoderPushState) {

    // if the encoder button is currently pressed down
    if (newEndcoderPushState == HIGH) {
      if (selected_channel == 3) {
        selected_channel = 0;
      } else {
        selected_channel += 1;
      }

      for (uint8_t i = 0; i < NUM_TRIG_INPUTS; i++) {
        if (i == selected_channel) {
          io.digitalWrite(i, HIGH);
        } else {
          io.digitalWrite(i, LOW);
        }
      }
      if (DEBUG) { Serial.println(selected_channel); }
    }

    // if the encoder button is released
    else {
      // do nothing
    }

    oldEndcoderPushState = newEndcoderPushState;
  }

  // ENCODER ONE
  if (newEncoderPos != oldEncoderPos ) {
    if (DEBUG) { Serial.println(newEncoderPos); }

    if (newEncoderPos % 4 == 0) {
      if (oldEncoderPos > newEncoderPos) {
        counter[selected_channel] += 1;
        triggerMIDIValues[selected_channel] += 1;
      } else {
        counter[selected_channel] -= 1;
        triggerMIDIValues[selected_channel] -= 1;
      }
    }
    oldEncoderPos = newEncoderPos;
  }




  // get state of each input
  for (uint8_t i=0; i < NUM_TRIG_INPUTS; i++) {
    uint8_t state = digitalRead(trigger_input_pins[i]);
    bitWrite(newInputStates, i, state);
  }

  if ( newInputStates != oldInputStates ) {

    for (uint8_t i=0; i < NUM_TRIG_INPUTS; i++) {

      if (bitRead(newInputStates, i) != bitRead(oldInputStates, i)) {
        if (bitRead(newInputStates, i) == LOW) { // LOW means a trigger / gate has been detected
          if (DEBUG) {
            Serial.print("channel: "); Serial.print(i);
            Serial.print(" -- MIDI note: "); Serial.print(triggerMIDIValues[i]);
            Serial.println(" ");
          }
          MIDI.sendNoteOn(triggerMIDIValues[i], 127, MIDI_CHANNEL);
        } else {
          if (DEBUG) { Serial.println("LOW"); }
          MIDI.sendNoteOff(triggerMIDIValues[i], 0, MIDI_CHANNEL);
        }
      }
    }

    oldInputStates = newInputStates;
  }

}

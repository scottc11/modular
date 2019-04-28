#include <main.h>
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_MCP23017.h>
#include "Adafruit_MCP23008.h"
#include <Adafruit_MPR121.h>
#include <Channel.h>

// DACs
MCP492X dacA(DAC_A_SS_PIN);
MCP492X dacB(DAC_B_SS_PIN);


// MPR121 objects
Adafruit_MPR121 touchA = Adafruit_MPR121();
Adafruit_MPR121 touchB = Adafruit_MPR121();
Adafruit_MPR121 touchC = Adafruit_MPR121();
Adafruit_MPR121 touchD = Adafruit_MPR121();

// IO Expanders
Adafruit_MCP23017 ioA = Adafruit_MCP23017(); // IO LEDS A + B
Adafruit_MCP23017 ioB = Adafruit_MCP23017(); // IO LEDS C + D
Adafruit_MCP23017 ioZ = Adafruit_MCP23017(); // IO Mode Toggle switches, gate LEDS, and Degree switches
Adafruit_MCP23017 ioX = Adafruit_MCP23017(); // IO Octave LEDS A + B and all Octave toggle switches
Adafruit_MCP23008 ioY; // IO Octave LEDS C + D

Channel channelA = Channel(CHANNEL_A, V_IN_PIN_A, GATE_PIN_A, GATE_LED_PIN_A, CHANNEL_A_DAC_CHAN, OCT_UP_PIN_A, OCT_DOWN_PIN_A, OCT_LED_PINS_A);
Channel channelB = Channel(CHANNEL_B, V_IN_PIN_B, GATE_PIN_B, GATE_LED_PIN_B, CHANNEL_B_DAC_CHAN, OCT_UP_PIN_B, OCT_DOWN_PIN_B, OCT_LED_PINS_B);
Channel channelC = Channel(CHANNEL_C, V_IN_PIN_C, GATE_PIN_C, GATE_LED_PIN_C, CHANNEL_C_DAC_CHAN, OCT_UP_PIN_C, OCT_DOWN_PIN_C, OCT_LED_PINS_C);
Channel channelD = Channel(CHANNEL_D, V_IN_PIN_D, GATE_PIN_D, GATE_LED_PIN_D, CHANNEL_D_DAC_CHAN, OCT_UP_PIN_D, OCT_DOWN_PIN_D, OCT_LED_PINS_D);

// Keeps track of the last pins touched on MPR121
uint16_t lastTouchedA = 0;
uint16_t lastTouchedB = 0;
uint16_t lastTouchedC = 0;
uint16_t lastTouchedD = 0;

uint16_t currTouchedA = 0;
uint16_t currTouchedB = 0;
uint16_t currTouchedC = 0;
uint16_t currTouchedD = 0;

// Needed specifically for monophonic notes when toggleing tonic switches
int lastTouchedIndex[] = {0, 0, 0, 0};


// VOLTAGE OUTPUT
float voutA = 0;
float voutB = 0;
float voutC = 0;
float voutD = 0;

// for octave toggle switches
byte newOctaveStates = 0;
byte oldOctaveStates = 0;

// for degree toggle switches
byte newDegreeStates = 0;
byte oldDegreeStates = 0;

// byte array to determine which mode each channel is currently in
byte newModeStates = 0;
byte oldModeStates = 0;

// the octave index to access the value of an octave in arr OCTAVE_VALUES
int octaveValues[] = {0, 0, 0, 0};

// array of bytes to determine which scale degrees are active at a given time.
// 1 == scale degree is active, 0 == scale degree is inactive
byte activeDegrees[] = {0, 0, 0, 0}; // DELETE THIS
int activeDegreeValues[4][8];
int activeThresholds[4][8];

// array of ints to represent the number of degrees are active (to quantize voltage input)
int activeCount[] = {0, 0, 0, 0};




// ==============================================================================================================
// SETUP
// ==============================================================================================================

void setup() {
  Serial.begin(9600);
  Serial.println("Setting Up...");
  // Connect MPR121 touch sensors
  if (!touchA.begin(TOUCH_A_ADDR)) { Serial.println("MPR121 A not found, check wiring?"); while (1); }
  Serial.println("MPR121 A Found!");
  if (!touchB.begin(TOUCH_B_ADDR)) { Serial.println("MPR121 B not found, check wiring?"); while (1); }
  Serial.println("MPR121 B Found!");
  if (!touchC.begin(TOUCH_C_ADDR)) { Serial.println("MPR121 C not found, check wiring?"); while (1); }
  Serial.println("MPR121 C Found!");
  if (!touchD.begin(TOUCH_D_ADDR)) { Serial.println("MPR121 D not found, check wiring?"); while (1); }
  Serial.println("MPR121 D Found!");


  ioA.begin(IO_A_ADDR);
  ioB.begin(IO_B_ADDR);
  ioZ.begin(IO_S_ADDR);
  ioX.begin(IO_X_ADDR);
  ioY.begin(IO_Y_ADDR);

  // init DACs
  dacA.begin();
  dacB.begin();

  channelA.setIO(&ioA, &ioZ, &ioX, &ioX, &dacA, &ioY, false);
  channelB.setIO(&ioA, &ioZ, &ioX, &ioX, &dacA, &ioY, false);
  channelC.setIO(&ioB, &ioZ, &ioX, &ioX, &dacB, &ioY, true);
  channelD.setIO(&ioB, &ioZ, &ioX, &ioX, &dacB, &ioY, true);

  // set pin modes for IO expanders
  initIOPins(ioA, ioB, ioX, ioY, ioZ);

}

// ==============================================================================================================
// LOOP
// ==============================================================================================================
void loop() {

  for (uint8_t i = 0; i < 4; i++) {
    uint8_t state = ioZ.digitalRead(MODE_PINS[i]);
    bitWrite(newModeStates, i, state);
  }


  // Get the currently touched pads
  // .touched() will return 16 bits (one byte), with each bit (from 0 - 12) representing the corrosponding touch pad
  currTouchedA = touchA.touched();
  currTouchedB = touchB.touched();
  currTouchedC = touchC.touched();
  currTouchedD = touchD.touched();

  // write octave switch states
  for (uint8_t i=0; i < 8; i++) {
    uint8_t state = ioX.digitalRead(OCT_SWITCH_PINS[i]);
    bitWrite(newOctaveStates, i, state);
  }

  if ( newOctaveStates != oldOctaveStates ) {
    channelA.handleOctaveSwitchEvent();
    channelB.handleOctaveSwitchEvent();
    channelC.handleOctaveSwitchEvent();
    channelD.handleOctaveSwitchEvent();
    oldOctaveStates = newOctaveStates;
  }


  // ------------ SCALE DEGREE SWITCHES ------------------
  // write degree switch states of each scale step

  for (uint8_t i=0; i < 8; i++) {
    uint8_t state = ioZ.digitalRead(DRG_SWITCH_PINS[i]);
    bitWrite(newDegreeStates, i, state);
  }

  if ( newDegreeStates != oldDegreeStates ) {
    channelA.handleDegreeToggleEvent();
    channelB.handleDegreeToggleEvent();
    channelC.handleDegreeToggleEvent();
    channelD.handleDegreeToggleEvent();
    oldDegreeStates = newDegreeStates;
  }

  // ------------------ END ------------------



  // Iterate over touch sensors
  for (uint8_t i=0; i<12; i++) {

    // if it *is* touched and *wasnt* touched before
    if ( (currTouchedA & _BV(i) ) && !( lastTouchedA & _BV(i) ) ) {
      channelA.handleTouchEvent(i);
    }

    //  if it *was* touched and now *isnt*
    if (!(currTouchedA & _BV(i)) && (lastTouchedA & _BV(i)) ) {
      channelA.handleReleaseEvent(i);
    }

    if ( (currTouchedB & _BV(i) ) && !( lastTouchedB & _BV(i) ) ) {
      channelB.handleTouchEvent(i);
    }

    if (!(currTouchedB & _BV(i)) && (lastTouchedB & _BV(i)) ) {
      channelB.handleReleaseEvent(i);
    }

    if ( (currTouchedC & _BV(i) ) && !( lastTouchedC & _BV(i) ) ) {
      channelC.handleTouchEvent(i);
    }

    if (!(currTouchedC & _BV(i)) && (lastTouchedC & _BV(i)) ) {
      channelC.handleReleaseEvent(i);
    }

    if ( (currTouchedD & _BV(i) ) && !( lastTouchedD & _BV(i) ) ) {
      channelD.handleTouchEvent(i);
    }

    if (!(currTouchedD & _BV(i)) && (lastTouchedD & _BV(i)) ) {
      channelD.handleReleaseEvent(i);
    }
  }

  // reset touch sensors state
  lastTouchedA = currTouchedA;
  lastTouchedB = currTouchedB;
  lastTouchedC = currTouchedC;
  lastTouchedD = currTouchedD;


  // update channel modes
  if (newModeStates != oldModeStates) {
    channelA.updateChannelMode();
    channelB.updateChannelMode();
    channelC.updateChannelMode();
    channelD.updateChannelMode();
    oldModeStates = newModeStates;
  }


  // quantize all voltage inputs and update all voltage outputs
  channelA.handleQuantization();
  channelB.handleQuantization();
  channelC.handleQuantization();
  channelD.handleQuantization();

}

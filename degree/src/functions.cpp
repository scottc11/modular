#include <main.h>
#include <Arduino.h>
#include <Adafruit_MCP23017.h>
#include <Adafruit_MCP23008.h>

extern int octaveValues[];

void initIOPins(
  Adafruit_MCP23017 ioA,
  Adafruit_MCP23017 ioB,
  Adafruit_MCP23017 ioX,
  Adafruit_MCP23008 ioY,
  Adafruit_MCP23017 ioZ ) {

  for(int i=0; i < LENGTH; i++) {
    ioA.pinMode(LED_PINS_A[i], OUTPUT);
    ioA.pinMode(LED_PINS_B[i], OUTPUT);
    ioB.pinMode(LED_PINS_C[i], OUTPUT);
    ioB.pinMode(LED_PINS_D[i], OUTPUT);
    ioX.pinMode(OCT_SWITCH_PINS[i], INPUT);
    ioX.pullUp(OCT_SWITCH_PINS[i], HIGH);
    ioZ.pinMode(DRG_SWITCH_PINS[i], INPUT);
    ioZ.pullUp(DRG_SWITCH_PINS[i], HIGH);
  }

  // set Octave LED pinouts
  for (size_t i = 0; i < 4; i++) {
    ioX.pinMode(OCT_LED_PINS_A[i], OUTPUT);
    ioX.pinMode(OCT_LED_PINS_B[i], OUTPUT);
    ioY.pinMode(OCT_LED_PINS_C[i], OUTPUT);
    ioY.pinMode(OCT_LED_PINS_D[i], OUTPUT);
    ioX.digitalWrite(OCT_LED_PINS_A[i], LOW);
    ioX.digitalWrite(OCT_LED_PINS_B[i], LOW);
    ioY.digitalWrite(OCT_LED_PINS_C[i], LOW);
    ioY.digitalWrite(OCT_LED_PINS_D[i], LOW);
  }

  // set pinmodes for gate/triggers
  pinMode(GATE_PIN_A, OUTPUT);
  pinMode(GATE_PIN_B, OUTPUT);
  pinMode(GATE_PIN_C, OUTPUT);
  pinMode(GATE_PIN_D, OUTPUT);
  digitalWrite(GATE_PIN_A, LOW);
  digitalWrite(GATE_PIN_B, LOW);
  digitalWrite(GATE_PIN_C, LOW);
  digitalWrite(GATE_PIN_D, LOW);

  // Set pinmodes for Gate LEDs
  ioZ.pinMode(GATE_LED_PIN_A, OUTPUT);
  ioZ.pinMode(GATE_LED_PIN_B, OUTPUT);
  ioZ.pinMode(GATE_LED_PIN_C, OUTPUT);
  ioZ.pinMode(GATE_LED_PIN_D, OUTPUT);
  ioZ.digitalWrite(GATE_LED_PIN_A, LOW);
  ioZ.digitalWrite(GATE_LED_PIN_B, LOW);
  ioZ.digitalWrite(GATE_LED_PIN_C, LOW);
  ioZ.digitalWrite(GATE_LED_PIN_D, LOW);

  // Set pinmodes for mode switchs
  ioZ.pinMode(MODE_PINS[CHANNEL_A], INPUT);
  ioZ.pinMode(MODE_PINS[CHANNEL_B], INPUT);
  ioZ.pinMode(MODE_PINS[CHANNEL_C], INPUT);
  ioZ.pinMode(MODE_PINS[CHANNEL_D], INPUT);
  ioZ.pullUp(MODE_PINS[CHANNEL_A], HIGH);
  ioZ.pullUp(MODE_PINS[CHANNEL_B], HIGH);
  ioZ.pullUp(MODE_PINS[CHANNEL_C], HIGH);
  ioZ.pullUp(MODE_PINS[CHANNEL_D], HIGH);

  // INIT
  ioX.digitalWrite(OCT_LED_PINS_A[octaveValues[CHANNEL_A]], HIGH);
  ioX.digitalWrite(OCT_LED_PINS_B[octaveValues[CHANNEL_B]], HIGH);
  ioY.digitalWrite(OCT_LED_PINS_C[octaveValues[CHANNEL_C]], HIGH);
  ioY.digitalWrite(OCT_LED_PINS_D[octaveValues[CHANNEL_D]], HIGH);
}

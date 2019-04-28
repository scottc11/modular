#ifndef MAIN_H
#define MAIN_H
#include <Arduino.h>
#include <Adafruit_MCP23017.h>
#include <Adafruit_MCP23008.h>
#include <MCP492X.h>

#define DEBUG false


// Adafruit_MPR121 I2C addresses
#define TOUCH_A_ADDR 0x5A
#define TOUCH_B_ADDR 0x5B
#define TOUCH_C_ADDR 0x5C
#define TOUCH_D_ADDR 0x5D

// io I2C addresses
#define IO_A_ADDR 0x00
#define IO_B_ADDR 4
#define IO_S_ADDR 6
#define IO_X_ADDR 1
#define IO_Y_ADDR 2


// SPI pins
#define SCK_PIN 13
#define SDI_PIN 11

// define slave select (ss) pins for SPI
#define DAC_A_SS_PIN 9
#define DAC_B_SS_PIN 10

// channel modes are determined by INT value
const int MONOPHONIC_MODE = 0;
const int QUANTIZER_MODE = 1;
const int LOOP_MODE = 2;

const int ACTIVE = 1;         // degree active == 1, inactive == 0 OR != 1;
const int LENGTH = 8;         // length of arrays

const int CHANNEL_A = 0;
const int CHANNEL_B = 1;
const int CHANNEL_C = 2;
const int CHANNEL_D = 3;


const int V_IN_PIN_A = A0;
const int V_IN_PIN_B = A1;
const int V_IN_PIN_C = A2;
const int V_IN_PIN_D = A3;


const int GATE_PIN_A = 4; // via Atemega328
const int GATE_PIN_B = 5; // via Atemega328
const int GATE_PIN_C = 6; // via Atemega328
const int GATE_PIN_D = 7; // via Atemega328

const int GATE_LED_PIN_A = 15; // via ioZ
const int GATE_LED_PIN_B = 14; // via ioZ
const int GATE_LED_PIN_C = 13; // via ioZ
const int GATE_LED_PIN_D = 12; // via ioZ

// LED pins for MCP23017
const int LED_PINS_A[] = {15, 14, 13, 12, 11, 10, 9, 8};
const int LED_PINS_B[] = {0, 1, 2, 3, 4, 5, 6, 7};
const int LED_PINS_C[] = {15, 14, 13, 12, 11, 10, 9, 8};
const int LED_PINS_D[] = {0, 1, 2, 3, 4, 5, 6, 7};

const int LED_PINS[4][8] = {
  {15, 14, 13, 12, 11, 10, 9, 8},
  {0, 1, 2, 3, 4, 5, 6, 7},
  {15, 14, 13, 12, 11, 10, 9, 8},
  {0, 1, 2, 3, 4, 5, 6, 7}
};


const float QUANTIZED_VOLTAGES[8][2] = {
    { 0, 136.5 },      // I
    { 136.5, 204.75 }, // maj2, n/a
    { 204.75, 273 },   // min3, maj3
    { 341.25, 409.5 }, // per4, aug4
    { 409.5, 477.75 }, // dim5, per5
    { 546, 614.25 },   // min6, maj6
    { 682.5, 750.75 }, // min7, maj7
    { 750.75, 819 }    // VIII
  };

const int OCT_LED_PINS_A[] = {3, 2, 1, 0};
const int OCT_LED_PINS_B[] = {7, 6, 5, 4};
const int OCT_LED_PINS_C[] = {3, 2, 1, 0};
const int OCT_LED_PINS_D[] = {7, 6, 5, 4};

const int OCT_UP_PIN_A = 14; // via ioX
const int OCT_DOWN_PIN_A = 15; // via ioX
const int OCT_UP_PIN_B = 12; // via ioX
const int OCT_DOWN_PIN_B = 13; // via ioX
const int OCT_UP_PIN_C = 10; // via ioX
const int OCT_DOWN_PIN_C = 11; // via ioX
const int OCT_UP_PIN_D = 9; // via ioX
const int OCT_DOWN_PIN_D = 8; // via ioX

const int OCT_SWITCH_PINS[] = {
  OCT_UP_PIN_A,
  OCT_DOWN_PIN_A,
  OCT_UP_PIN_B,
  OCT_DOWN_PIN_B,
  OCT_UP_PIN_C,
  OCT_DOWN_PIN_C,
  OCT_UP_PIN_D,
  OCT_DOWN_PIN_D
};

const int OCT_UP_PINS[] = {14, 12, 10, 9}; // via ioX
const int OCT_DOWN_PINS[] = {15, 13, 11, 8}; // via ioX

const float OCTAVE_VALUES[] = {0.0, 819.0, 1638.0, 2457.0, 3276.0};

const int OCT_MAX = 3;
const int OCT_MIN = 0;

const int DRG_SWITCH_PINS[] = {7, 6, 5, 4, 3, 2, 1, 0}; // *** you will need to address these to pin numbers, and also remember to activate nternal pull-up resistors via

const int CHANNEL_A_DAC_CHAN = 0;
const int CHANNEL_B_DAC_CHAN = 1;
const int CHANNEL_C_DAC_CHAN = 0;
const int CHANNEL_D_DAC_CHAN = 1;

const int MODE_PINS[] = {8, 9, 10, 11}; // via ioZ


// GLOBAL VARIABLES



// FUNCTIONS -------------------------------
void updateVoltageOutput(int channel, int index);
void megaUpdater(int channel, Adafruit_MCP23017 io);

void initIOPins(
  Adafruit_MCP23017 ioA,
  Adafruit_MCP23017 ioB,
  Adafruit_MCP23017 ioX,
  Adafruit_MCP23008 ioY,
  Adafruit_MCP23017 ioZ
);

#endif

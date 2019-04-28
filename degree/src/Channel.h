#ifndef CHANNEL_H
#define CHANNEL_H

#include <Arduino.h>
#include <Adafruit_MCP23017.h>
#include <Adafruit_MCP23008.h>
#include <Wire.h>
#include "main.h"


class Channel {

public:
  // attributes
  int channel;                    // channel number for ID and array locations
  int voltage_input_pin;          // analog pin to read external voltage input
  float voltage_out;              // voltage to output using DAC
  uint16_t curr_touched;          // currently touched degrees
  uint16_t last_touched;          // last touched degrees
  int last_touched_index;         // Needed specifically for monophonic notes when toggleing tonic switches
  int octave;                     // an index to access the value of an octave in array OCTAVE_VALUES
  byte active_degrees;            // 8 bits to determine which scale degrees are presently active/inactive (active = 1, inactive= 0)
  int active_degree_count;        // number of degrees which are active (to quantize voltage input)
  float active_degree_values[8];  // array to hold currently active scale degree values to output to DAC (ex. {136.5, 341.25, 682.50, 819.0, 0, 0, 0, 0} )
  int voltage_input_map[8];       // holds values between 0 and 1023 in order to map analogRead(voltage_input_pin) to the active_degree_values array

  int mode;                       // value of 0, 1, or 2. 0 == monophonic mode, 1 == quantizer mode, 2 == loop mode

  int octave_up_pin;              // digital pin #
  int octave_down_pin;            // digital pin #
  int *octave_led_pins_ptr;       // pointer to const array of digital pin numbers for channels octave LEDs

  int gate_pin;                   // digital pin # for 5V gate
  int gate_led_pin;               // digital pin # for gate LED indication
  int dac_channel;                // which channel to use on the dual DAC

  Adafruit_MCP23017 *degreeLedIO; // pointer for all 8 degree LED IO IC
  Adafruit_MCP23017 *gateLedIO;   // pointer for Gate LED IO IC
  Adafruit_MCP23017 *octLedIO;    // pointer for octave LED IO IC
  Adafruit_MCP23017 *octSwitchIO; // pointer for octave toggle switch IO IC
  Adafruit_MCP23008 *tmp__octLedIO; // TEMPORARY due to hardware contraints
  bool temp_bool;                   // TEMPORARY due to hardware contraints
  MCP492X *dac;                   // pointer for dual DAC IC


  // CONSTRUCTOR
  Channel(int _channel, int v_in_pin, int _gate_pin, int _gate_led_pin,
          int dac_chan, int oct_up_pin, int oct_dwn_pin,
          int *oct_led_pins) {
    channel = _channel;
    voltage_input_pin = v_in_pin;
    gate_pin = _gate_pin;
    gate_led_pin = _gate_led_pin;
    octave_up_pin = oct_up_pin;
    octave_down_pin = oct_dwn_pin;
    octave_led_pins_ptr = oct_led_pins;
    voltage_out = 0.0;
    last_touched_index = 0;
    octave = 0;
    active_degree_count = 0;
    dac_channel = dac_chan;
  }

  // methods
  void setGateHigh();
  void setGateLow();
  void setVoltageOutput(uint8_t degree);
  void setActiveDegrees(uint8_t degree);
  void incrementOctaveValue();
  void decrementOctaveValue();
  void handleTouchEvent(uint8_t touchedIndex);
  void handleReleaseEvent(uint8_t touchedIndex);
  void handleOctaveSwitchEvent();
  void handleDegreeToggleEvent();
  void handleQuantization();
  void updateChannelMode();
  void updateVoltageInputMap();
  void updateLEDs();
  void setActiveQuantizerValues();
  void updateOctaveLED();

  void setIO(
    Adafruit_MCP23017 *degree_led_io_ptr,
    Adafruit_MCP23017 *gate_led_io_addr,
    Adafruit_MCP23017 *oct_IO_ptr,
    Adafruit_MCP23017 *oct_switch_IO_ptr,
    MCP492X *dac_addr,
    Adafruit_MCP23008 *tmp_io,
    bool _temp_bool
  );
};

#endif

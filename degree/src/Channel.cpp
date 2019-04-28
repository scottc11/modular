#include "Channel.h"

extern byte newDegreeStates;
extern byte newModeStates;

void Channel::setIO(Adafruit_MCP23017 *degree_led_io_ptr,
                    Adafruit_MCP23017 *gate_led_io_addr,
                    Adafruit_MCP23017 *oct_IO_ptr,
                    Adafruit_MCP23017 *oct_switch_IO_ptr,
                    MCP492X *dac_addr,
                    Adafruit_MCP23008 *tmp_io,
                    bool _temp_bool ) {

  degreeLedIO = degree_led_io_ptr;
  gateLedIO = gate_led_io_addr;
  octLedIO = oct_IO_ptr;
  octSwitchIO = oct_switch_IO_ptr;
  dac = dac_addr;

  // temporary hack
  tmp__octLedIO = tmp_io;
  temp_bool = _temp_bool;
}

void Channel::handleDegreeToggleEvent() {

  this->setActiveQuantizerValues();

  switch (mode) {
    case MONOPHONIC_MODE:
      this->setVoltageOutput(this->last_touched_index);
      break;
    case QUANTIZER_MODE:
      break;
  }
}


void Channel::handleTouchEvent(uint8_t touchedIndex) {
  if (DEBUG == true) { Serial.print(touchedIndex); Serial.print(" touched :: "); Serial.println(touchedIndex, BIN); }

  if (touchedIndex < LENGTH) {
    switch (mode) {

      case MONOPHONIC_MODE:
        last_touched_index = touchedIndex;
        this->setGateHigh();
        this->setVoltageOutput(touchedIndex);
        this->updateLEDs();
        break;

      case QUANTIZER_MODE:
        // order of function execution matters
        this->setActiveDegrees(touchedIndex);
        this->updateLEDs();
        this->setActiveQuantizerValues();
        this->updateVoltageInputMap();
        break;
    }
  }
}

void Channel::handleReleaseEvent(uint8_t touchedIndex) {
  if (DEBUG == true) { Serial.print(touchedIndex); Serial.print(" released :: "); Serial.println(touchedIndex, BIN); }
  if (touchedIndex < LENGTH) {
    switch (mode) {
      case MONOPHONIC_MODE:
        this->setGateLow();
        break;
      default:
        this->setGateLow();
        break;
    }
  }
}

void Channel::updateLEDs() {

  for (uint8_t i = 0; i < LENGTH; i++) {  // iterate over all degrees

    switch (mode) {
      case MONOPHONIC_MODE:
        if (i == last_touched_index) {
          degreeLedIO->digitalWrite(LED_PINS[channel][i], HIGH);
        } else {
          degreeLedIO->digitalWrite(LED_PINS[channel][i], LOW);
        }
        break;

      case QUANTIZER_MODE:
        // toggle LED on/off based on state of degree
        degreeLedIO->digitalWrite(LED_PINS[channel][i], bitRead(active_degrees, i));
        break;
    }
  }
}



void Channel::setActiveQuantizerValues() {

  if (DEBUG) { Serial.print("DAC VALUES: "); }

  this->active_degree_count = 0; // reset the active count to zero

  for (uint8_t i = 0; i < LENGTH; i++) {  // iterate over all degrees
    // set the value (to output to DAC) of each active degree to an array, in order of lowest to highest
    if (bitRead(active_degrees, i) == ACTIVE) {

      uint8_t switchState = !bitRead(newDegreeStates, i);

      active_degree_values[active_degree_count] = QUANTIZED_VOLTAGES[i][switchState]; // the 'active_degree_count' is used here as an index value to assign the voltage value

      if (DEBUG) {Serial.print(active_degree_values[active_degree_count]);Serial.print(" - ");}

      active_degree_count += 1; // for each active degree, increment active_degree_count by 1
    }
  }
  if (DEBUG) {Serial.println("  ");}
}



void Channel::updateVoltageInputMap() {

  int min_threshold = 1023 / this->active_degree_count; // divide max voltage input value by num of active degrees

  // for each active degree, multiply the min_threshold by an array index and assign to voltage_input_map
  if (DEBUG) { Serial.print("VOLTAGE IN MAP: "); }
  for (int i=0; i < this->active_degree_count; i++) {
    int threshold = min_threshold * (i + 1); // can't multiply by zero!
    this->voltage_input_map[i] = threshold;
    if (DEBUG) {Serial.print(threshold); Serial.print(" - ");}
  }
  if (DEBUG) { Serial.println(""); }
}


// =====================================
// ------------ OCTAVE -----------------
// =====================================


void Channel::handleOctaveSwitchEvent() {
  uint8_t upSwitchState = octSwitchIO->digitalRead(octave_up_pin);
  uint8_t downSwitchState = octSwitchIO->digitalRead(octave_down_pin);

  // octave switches are by default pulled HIGH, so activating them will pull them LOW
  if (upSwitchState == LOW) {
    this->incrementOctaveValue();
  }
  else if (downSwitchState == LOW) {
    this->decrementOctaveValue();
  }
}

void Channel::incrementOctaveValue() {
  if (octave < OCT_MAX) {
    octave += 1;
    this->updateOctaveLED();
    this->setVoltageOutput(last_touched_index);
    if (DEBUG) {Serial.print("octave value: ");Serial.println(octave);}
  }
}

void Channel::decrementOctaveValue() {
  if (octave > OCT_MIN) {
    octave -= 1;
    this->updateOctaveLED();
    this->setVoltageOutput(last_touched_index);
    if (DEBUG) {Serial.print("octave value: ");Serial.println(octave);}
  }
}

void Channel::updateOctaveLED() {
  // there are 4 digital pins representing each octave LED. Iterate over all pins and set
  // the pin equivelent of the current octave value to HIGH, and the rest to LOW
  for (int i = 0; i < 4; i++) {
    if (i == octave) {
      if (!this->temp_bool) {
        octLedIO->digitalWrite(octave_led_pins_ptr[i], HIGH);
      } else {
        tmp__octLedIO->digitalWrite(octave_led_pins_ptr[i], HIGH);
      }
    } else {
      if (!this->temp_bool) {
        octLedIO->digitalWrite(octave_led_pins_ptr[i], LOW);
      } else {
        tmp__octLedIO->digitalWrite(octave_led_pins_ptr[i], LOW);
      }

    }
  }
}

// =====================================
// ------------ GATE ------------------

void Channel::setGateHigh() {
  digitalWrite(gate_pin, HIGH);
  gateLedIO->digitalWrite(gate_led_pin, HIGH);
}

void Channel::setGateLow() {
  digitalWrite(gate_pin, LOW);
  gateLedIO->digitalWrite(gate_led_pin, LOW);
}


// =====================================
// ------------ QUANTIZATION ------------------
// read the voltage on analog pin, use active_degree_count to determine how many degrees are active,
// if the voltage input is less than the
// divide the maximum possible value from analog input pin by the number active_degree_count
// map this value to an array[8]
// map voltage input values (num between 0 and 1023)

void Channel::handleQuantization() {

  if (this->mode == QUANTIZER_MODE) {

    int vIn = analogRead(this->voltage_input_pin);

    for (uint8_t i=0; i < this->active_degree_count; i++) {

      // if less than value @ index, set voltage_out to the mapped value in active_degree_values
      if (vIn < this->voltage_input_map[i]) {
        this->voltage_out = this->active_degree_values[i] + OCTAVE_VALUES[this->octave];
        break;
      }
    }

    dac->analogWrite(this->dac_channel, this->voltage_out);
  }
}


void Channel::setVoltageOutput(uint8_t degree) {
  voltage_out = 0.0f;
  uint8_t state = !bitRead(newDegreeStates, degree);
  voltage_out = QUANTIZED_VOLTAGES[degree][state] + OCTAVE_VALUES[octave];
  dac->analogWrite(dac_channel, voltage_out);
  if (DEBUG) {Serial.print("VOLTAGE OUT: "); Serial.println(voltage_out);}
}


// when a channels degree is touched, toggle the active/inactive status of the touched degree
void Channel::setActiveDegrees(uint8_t degree) {
  bitWrite(this->active_degrees, degree, !bitRead(this->active_degrees, degree));
  if (DEBUG) {Serial.print("ACTIVE DEGREES: "); Serial.println(this->active_degrees, BIN);}
}


void Channel::updateChannelMode() {
  uint8_t pinValue = bitRead(newModeStates, channel);
  if (pinValue == HIGH) {
    mode = QUANTIZER_MODE;
  } else {
    mode = MONOPHONIC_MODE;
    this->setVoltageOutput(this->last_touched_index);
  }
  this->updateLEDs();
  if (DEBUG) {Serial.print("CHANNEL MODE: "); Serial.println(mode);}
}

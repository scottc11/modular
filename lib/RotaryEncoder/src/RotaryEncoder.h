#ifndef ROTARYENCODER_H
#define ROTARYENCODER_H

#include <Arduino.h>

class RotaryEncoder {

private:
  int m_pinA;                     // Digital pin
  int m_pinB;                     // Digital pin
  int m_encoderALast;             // for bouncing
  int m_debounce;                 // number of milliseconds for debouncing
  unsigned long m_lastEventTime;  // timestamp
  int m_encoderPosition;                 // current position of the encoder
  int m_max;                      // maximum value
  int m_min;                      // minimum value

public:

  void init(
    int pinA,
    int pinB,
    int debounce,
    int start_pos = 0,
    int min_pos = 0,
    int max_pos = 100
  );

  // setters / getters
  void setShaftPosition(int position);
  void setMinValue(int min);
  void setMaxValue(int max);

  // returns value of m_encoderPosition
  int shaftPosition();

  // call this in main loop() to track changes in position
  int trackShaftPosition();
};


#endif

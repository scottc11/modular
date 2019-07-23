#include <RotaryEncoder.h>

void RotaryEncoder::init(int pinA, int pinB, int debounce, int start_pos = 0, int min_pos = 0, int max_pos = 100) {
  m_pinA = pinA;
  m_pinB = pinB;
  m_debounce = debounce;
  m_encoderPosition = start_pos;
  m_max = max_pos;
  m_min = min_pos;
}

int RotaryEncoder::trackShaftPosition() {
  bool pinAValue = digitalRead(m_pinA);
  unsigned long current_time = millis();
  unsigned long time_between_events = current_time - m_lastEventTime;

  if ((m_encoderALast == HIGH) && (pinAValue == LOW) && time_between_events > m_debounce ) {
    if (digitalRead(m_pinB) == LOW) {
      if (m_encoderPosition > m_min) { m_encoderPosition--; }
      else {
        if (m_encoderPosition < m_max) { m_encoderPosition++; }
      }
      m_lastEventTime = current_time;
    }
  }
  m_encoderALast = pinAValue;
  return m_encoderPosition;
}


int RotaryEncoder::shaftPosition() {
  return m_encoderPosition;
}


void RotaryEncoder::setMinValue(int min) {
  m_min = min;
}

void RotaryEncoder::setMaxValue(int max) {
  m_max = max;
}

void RotaryEncoder::setShaftPosition(int position) {
  m_encoderPosition = position;
}

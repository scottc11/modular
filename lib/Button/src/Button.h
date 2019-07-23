#ifndef BUTTON_H
#define BUTTON_H

class Button {
public:
  int pin;
  int newState;
  int prevState;

  Button(int _pin) {
    this->pin = _pin;
    this->newState = 0;
    this->prevState = 0;
  }

  bool changedState();
  bool isPressed();     // if the pin being read is HIGH, returns true
  void updateState();

};


#endif

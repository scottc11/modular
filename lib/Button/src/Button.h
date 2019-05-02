#ifndef BUTTON_H
#define BUTTON_H

class Button {
public:
  int newState;
  int oldState;

  Button() {
    this->newState = 0;
    this->oldState = 0;
  }

};

#endif

#include <Button.h>

bool Button::changedState() {
  if (this->newState != this->prevState) {
    return true;
  } else {
    return false;
  }
}

bool Button::isPressed() {
  if (this->newState == 1) {
    return true;
  } else {
    return false;
  }
}

void Button::updateState() {
  this->prevState = this->newState;
}

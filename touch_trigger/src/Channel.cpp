

class Channel {
  public:
  int channel;         // which channel should be used for IO
  int resetPin;        // digital pin for reset button


  Channel(int chan) {
    this->channel = chan;
  }
  void clearEvents();
};


// CLEAR ALL EVENTS
// void Channel::clearEvents() {
//
// }

// QUANTIZE EVENTS

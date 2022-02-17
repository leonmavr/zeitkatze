#include "terminal_setter.hpp"
#include <termios.h> // tcgetattr(), tcsetattr()

void TerminalSetter::SetTerminal() {
  // see below why this is necessary
  tcgetattr(1, &tio_before_);
  // NOTE: by passing tio_before_ here, the terminal ignores SIGINTs
  if (tcgetattr(1, &tio_after_) == 0) {
    //  disable ICANON - if set, terminal is uppercase only
    tio_after_.c_lflag &= ~ICANON;
    // minimum number of characters for noncanonical read
    tio_after_.c_cc[VMIN] = 0;
    // timeout in deciseconds for noncanonical read
    tio_after_.c_cc[VTIME] = 0;
    // set it immediately
    tcsetattr(1, TCSANOW, &tio_after_);
  }
}

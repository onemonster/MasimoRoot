#include <iostream>
#include <csignal>
#include <zconf.h>
#include "SerialPort.h"
#include "IAP.h"

using namespace std;

volatile sig_atomic_t quit = 0;

void signal_handler(int sig) {
  signal(sig, signal_handler);
  quit = 1;
}

int main() {
  signal(SIGINT, signal_handler);
  signal(SIGTERM, signal_handler);
#ifdef SIGBREAK
  signal(SIGBREAK, signal_handler);
#endif

  Socket *s = new SerialPort("/dev/ttyUSB0", B921600);
  IAP::MessageParser parser{};

  if (s->open()) {
    return -1;
  }

  while (!quit) {
    s->write(IAP::CommandBuilder::keepAlive());
    sleep(1);
    s->read(5000);
    unsigned char b = 0;
    while (s->getBufferSize() > 0) {
      s->popByte(&b);
      parser.addByte(b);
      if (parser.is_valid()) {
        parser.build()->record();
      }
    }
  }
  s->close();

  return 0;
}

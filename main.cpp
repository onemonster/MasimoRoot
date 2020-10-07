#include <iostream>
#include <csignal>
#include <zconf.h>
#include "SerialPort.h"

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

  SerialPort s("/dev/ttyUSB0", B921600);
  if (s.open() < 0) {
    return -1;
  }

  string keep_alive = "\xC3\x01\x11\x11\xC6";

  while (!quit) {
    printf("write\n");
    int n = s.write(keep_alive);
    printf("n: %d\n", n);
    sleep(1);
    s.read(5000);
    unsigned char b = 0;
    for (int i = 0; i < 5; i++) {
      s.readByte(&b);
      printf("0x%02X ", b);
    }
    printf("\n");
  }
  s.close();

  return 0;
}

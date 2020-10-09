#include <iostream>
#include <csignal>
#include <zconf.h>
#include "SerialPort.h"
#include "IAP.h"
#include "response_message/ProtocolRevisionResponse.h"

#define P16(a, b) ((unsigned short)(((unsigned short)(a) << 8U) | ((unsigned char)(b))))

using namespace std;

volatile sig_atomic_t quit = 0;

void signal_handler(int sig) {
  signal(sig, signal_handler);
  quit = 1;
}

void print_bits(unsigned short octet) {
  int z = USHRT_MAX / 2 + 1, oct = octet;

  while (z > 0) {
    if (oct & z)
      write(1, "1", 1);
    else
      write(1, "0", 1);
    z >>= 1;
  }
  printf("\n");
}

int main() {
//  auto command = IAP::CommandBuilder::requestDataGroup(SerialNumbers);
//  for (uint8_t c : command) printf("%x ", c);
//  return 0;
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
    int r = (int)random() % 4;
    DataGroup dg;
    if (r == 0) dg = SerialNumbers;
    if (r == 1) dg = Versions;
    if (r == 2) dg = SpO2;
    if (r == 3) dg = SpHb;
    auto command = IAP::CommandBuilder::requestDataGroup(dg);
    s->write(IAP::CommandBuilder::keepAlive());
    s->write(command);
//    s->write(IAP::CommandBuilder::keepAlive());
    sleep(1);
    s->read(5000);
    unsigned char b = 0;
    while (s->getBufferSize() > 0) {
      s->popByte(&b);
      parser.addByte(b);
      if (parser.isValid()) {
        if (auto response = parser.build()) {
          response->record();
          delete response;
        }
      }
    }
  }
  s->close();

  return 0;
}

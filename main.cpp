#include <iostream>
#include <csignal>
#include <zconf.h>
#include <cstring>
#include <fcntl.h>
#include "SerialPort.h"
#include "IAPProtocol.h"
#include "response_message/ProtocolRevisionResponse.h"
#include "IAPClient.h"

#define P16(a, b) ((unsigned short)(((unsigned short)(a) << 8U) | ((unsigned char)(b))))

using namespace std;

volatile sig_atomic_t quit = 0;
volatile sig_atomic_t gsig = 2;

void signal_handler(int sig) {
  signal(sig, signal_handler);
  gsig = sig;
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
  signal(SIGINT, signal_handler);
  signal(SIGTERM, signal_handler);
#ifdef SIGBREAK
  signal(SIGBREAK, signal_handler);
#endif

  auto s = new SerialPort("/dev/ttyUSB0", B921600);
  auto iap = new IAPClient(s);

  printf("main pid: %d\n", getpid());
  iap->startRead();
  iap->startKeepAlive();

  iap->command(IAP::CommandBuilder::requestPeriodicDataGroupDelivery(DG_SpHb));
  sleep(3);
  iap->command(IAP::CommandBuilder::requestPeriodicDataGroupDelivery(DG_SpO2));
  sleep(3);
  iap->command(IAP::CommandBuilder::cancelAllPeriodicDataGroupDelivery());
  while (!quit) {
//    int r = (int) random() % 4;
//    DataGroup dg;
//    if (r == 0) dg = SerialNumbers;
//    if (r == 1) dg = Versions;
//    if (r == 2) dg = SpO2;
//    if (r == 3) dg = SpHb;
//    auto command = IAP::CommandBuilder::requestDataGroup(dg);
//    iap->command(command);
    sleep(1);
  }
  iap->command(IAP::CommandBuilder::cancelAllPeriodicDataGroupDelivery());
  printf("main pid: %d\n", getpid());
  iap->killKeepAlive(gsig);
  iap->killRead(gsig);
  return 0;
}

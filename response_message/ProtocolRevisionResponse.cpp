#include <cstdio>
#include "ProtocolRevisionResponse.h"

ProtocolRevisionResponse::ProtocolRevisionResponse(unsigned short version) {
  version_ = version;
}

void ProtocolRevisionResponse::record() {
  unsigned char v1 = ((unsigned int)version_ >> 12U) & 0x0FU;
  unsigned char v2 = ((unsigned int)version_ >> 8U) & 0x0FU;
  unsigned char v3 = ((unsigned int)version_ >> 4U) & 0x0FU;
  unsigned char v4 = version_ & 0x0FU;

  printf("[ProtocolRevision] V%d.%d.%d.%d\n", v1, v2, v3, v4);
}


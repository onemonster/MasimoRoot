#include <cstdio>
#include "ProtocolRevisionResponse.h"

ProtocolRevisionResponse::ProtocolRevisionResponse(uint16_t version) {
  version_ = version;
}

void ProtocolRevisionResponse::record() {
  uint8_t v1 = ((uint32_t)version_ >> 12U) & 0x0FU;
  uint8_t v2 = ((uint32_t)version_ >> 8U) & 0x0FU;
  uint8_t v3 = ((uint32_t)version_ >> 4U) & 0x0FU;
  uint8_t v4 = version_ & 0x0FU;

  printf("[ProtocolRevision] V%d.%d.%d.%d\n", v1, v2, v3, v4);
}


#include "NakResponse.h"
#include <cstdio>

NakResponse::NakResponse(uint8_t nak_type) {
  nak_type_ = nak_type;
}

void NakResponse::record() {
  switch (nak_type_) {
    case BAD_PACKET:
      printf("[NAK] Bad Packet - bad LEN, CSUM, or EOM\n");
      break;
    case INVALID_COMMAND:
      printf("[NAK] Invalid Command – byte d1 is not a recognized command,"
             "or the following bytes are invalid for this command\n");
      break;
    case REQUEST_NOT_SUPPORTED:
      printf("[NAK] Request is not supported by the instrument\n");
      break;
    case WAVEFORM_REQUEST_NOT_SUPPORTED:
      printf("[NAK] Waveform Request is not supported by the instrument\n");
      break;
    default:
      printf("[NAK] Unknown error\n");
  }
}

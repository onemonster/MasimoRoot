#pragma once

#include "ResponseMessage.h"

class NakResponse : public ResponseMessage {
public:
  explicit NakResponse(unsigned char nak_type);
  void record() override;
private:
  unsigned char nak_type_;
  enum NakType {
    BAD_PACKET = 0x00,
    INVALID_COMMAND = 0x01,
    REQUEST_NOT_SUPPORTED = 0x02,
    WAVEFORM_REQUEST_NOT_SUPPORTED = 0x0D
  };
};

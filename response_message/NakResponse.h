#pragma once

#include <cstdint>
#include "ResponseMessage.h"

class NakResponse : public ResponseMessage {
public:
  explicit NakResponse(uint8_t nak_type);

  ~NakResponse() override = default;

  void record() override;

private:
  uint8_t nak_type_;
  enum NakType {
    BAD_PACKET = 0x00,
    INVALID_COMMAND = 0x01,
    REQUEST_NOT_SUPPORTED = 0x02,
    WAVEFORM_REQUEST_NOT_SUPPORTED = 0x0D
  };
};

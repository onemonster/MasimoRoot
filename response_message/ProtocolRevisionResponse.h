#pragma once

#include <cstdint>
#include "ResponseMessage.h"

class ProtocolRevisionResponse : public ResponseMessage {
public:
  explicit ProtocolRevisionResponse(uint16_t version);
  void record() override;

private:
  uint16_t version_;
};

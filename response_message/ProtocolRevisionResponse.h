#pragma once

#include "ResponseMessage.h"

class ProtocolRevisionResponse : public ResponseMessage {
public:
  explicit ProtocolRevisionResponse(unsigned short version);
  void record() override;

private:
  unsigned short version_;
};

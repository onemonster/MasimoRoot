#pragma once

#include <cstdint>
#include <vector>
#include "ResponseMessage.h"

class UnknownResponse : public ResponseMessage {
public:
  explicit UnknownResponse(std::vector<uint8_t> payload);

  ~UnknownResponse() override = default;

  void record() override;

private:
  std::vector<uint8_t> payload_;
};

#pragma once

#include "ResponseMessage.h"

class AckResponse : public ResponseMessage {
public:
  void record() override;
};

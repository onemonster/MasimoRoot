#pragma once

class ResponseMessage {
public:
  virtual void record() = 0;

  virtual ~ResponseMessage() = default;
};

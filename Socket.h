#pragma once

#include <string>

class Socket {
public:
  virtual int read(size_t max_len) = 0;

  virtual int popByte(unsigned char *b) = 0;

  virtual int write(std::string s) = 0;

  virtual size_t getBufferSize() = 0;

  virtual int open() = 0;

  virtual int close() = 0;
};

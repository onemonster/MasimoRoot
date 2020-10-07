#pragma once

#include <string>

class Socket {
public:
  virtual int read(size_t max_len) = 0;

  virtual int readByte(unsigned char *b) = 0;

  virtual int write(std::string s) = 0;
};

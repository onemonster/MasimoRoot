#pragma once

#include "Socket.h"
#include <deque>
#include <shared_mutex>
#include <termios.h>

class SerialPort : public virtual Socket {
public:
  /**
   *
   * @param path
   * @param baud_rate
   * @param data_bits
   * @param stop_bit 1: one stop bit, 2: two stop bits (Not implemented)
   * @param parity 0: no parity, 1: odd parity, 2: even parity
   */
  SerialPort(
      const std::string &path,
      speed_t baud_rate,
      unsigned char data_bits = 8,
      unsigned char stop_bit = 1,
      unsigned char parity = 0
  );

  int open() override;

  int close() override;

  int read(size_t max_len) override;

  int popByte(unsigned char *b) override;

  int write(std::string s) override;

  size_t getBufferSize() override;

private:
  std::string path_;
  speed_t baud_rate_;
  unsigned char data_bits_ = 8;
  unsigned char stop_bit_ = 1;
  unsigned char parity_ = 0;

  int fd_;
  std::deque<unsigned char> buffer_;
  std::shared_mutex lock_;

  int clearBuffer();
};

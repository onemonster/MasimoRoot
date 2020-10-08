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
      uint8_t data_bits = 8,
      uint8_t stop_bit = 1,
      uint8_t parity = 0
  );

  int open() override;

  int close() override;

  int read(size_t max_len) override;

  int popByte(uint8_t *b) override;

  int write(std::string s) override;

  size_t getBufferSize() override;

private:
  std::string path_;
  speed_t baud_rate_;
  uint8_t data_bits_ = 8;
  uint8_t stop_bit_ = 1;
  uint8_t parity_ = 0;

  int fd_;
  std::deque<uint8_t> buffer_;
  std::shared_mutex lock_;

  int clearBuffer();
};

#pragma once

#include "Socket.h"

class IAPClient {
public:
  explicit IAPClient(Socket *socket);

  ~IAPClient();

  int command(std::string message);

  void startKeepAlive();

  int killKeepAlive(int sig) const;

  void startRead();

  int killRead(int sig) const;

private:
  Socket *socket_;
  pid_t keep_alive_pid_, read_pid_;
};

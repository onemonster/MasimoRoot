#pragma once

#include "Socket.h"

class Root {
public:
  Root(Socket *socket);

private:
  Socket *socket_;
};

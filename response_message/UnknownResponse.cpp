#include "UnknownResponse.h"

#include <utility>
#include <cstdio>

using namespace std;

UnknownResponse::UnknownResponse(vector<uint8_t> payload) {
  payload_ = move(payload);
}

void UnknownResponse::record() {
  printf("[Unknown] ");
  for (uint8_t x : payload_) printf("%x ", x);
  printf("\n");
}

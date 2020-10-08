#include "IAP.h"
#include "response_message/AckResponse.h"
#include "response_message/NakResponse.h"
#include "response_message/ProtocolRevisionResponse.h"
#include <numeric>

using namespace std;
using namespace IAP;

#define PACK16(a, b) ((unsigned short)(((unsigned short)(a) << 8U) | ((unsigned char)(b))))

bool MessageParser::isValid() {
  // Check is SOM and EOM encountered
  if (!started_ || !ended_) return false;

  // Check length
  if (length_ != payload_.size()) return false;

  // Checksum
  auto lambda = [](int a, int b) { return a + b; };
  auto data_check_sum = accumulate(payload_.begin(), payload_.end(), 0, lambda);
  if (check_sum_ != data_check_sum) return false;

  return true;
}

void MessageParser::addByte(unsigned char b) {
  auto getCheckSum = [&]() {
    unsigned int sum = 0;
    for (unsigned char payload_byte : payload_)
      sum += payload_byte;
    return sum & 0xFFU;
  };
  switch (state_) {
    case START_STATE:
      if (b != SOM) return;
      state_ = CHECK_LEN_MARK_STATE;
      started_ = true;
      break;
    case CHECK_LEN_MARK_STATE:
      state_ = LENGTH_STATE;
      if (b == LEN_MARK) {
        marked_flag_ = true;
        break;
      } else if (b > 250) {
        reset();
        return;
      }
    case LENGTH_STATE:
      if (marked_flag_) {
        length_ = b;
        length_ << 8u;
        marked_flag_ = false;
      } else {
        length_ = length_ | b;
        state_ = PAYLOAD_STATE;
        if (length_ > 4096) {
          reset();
          return;
        }
      }
      break;
    case PAYLOAD_STATE:
      payload_.push_back(b);
      data_count_++;
      if (length_ == data_count_) {
        state_ = CHECK_SUM_STATE;
      }
      break;
    case CHECK_SUM_STATE:
      state_ = END_STATE;
      if (b != getCheckSum()) {
        reset();
        return;
      }
      check_sum_ = b;
      break;
    case END_STATE:
      if (b == EOM) ended_ = true;
      else reset();
  }
}

ResponseMessage *MessageParser::build() {
  if (!isValid()) return nullptr;
  switch (payload_[0]) {
    case ACK: return new AckResponse();
    case NAK: return new NakResponse(payload_[1]);
    case ProtocolRevision: return new ProtocolRevisionResponse(PACK16(payload_[1], payload_[2]));
    default:
      printf("TODO: Create response class");
  }
  return nullptr;
}

void MessageParser::reset() {
  state_ = START_STATE;
  started_ = false;
  ended_ = false;
  marked_flag_ = false;
  length_ = 0;
  data_count_ = 0;
  check_sum_ = 0;
  payload_.clear();
}

string CommandBuilder::ack() {
  return "\xC3\x01\x10\x10\xC6";
}

string CommandBuilder::keepAlive() {
  return "\xC3\x01\x11\x11\xC6";
}

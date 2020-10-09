#include "IAP.h"
#include "response_message/AckResponse.h"
#include "response_message/NakResponse.h"
#include "response_message/ProtocolRevisionResponse.h"
#include "response_message/DataGroupResponse.h"
#include <numeric>

using namespace std;
using namespace IAP;

#define P16(a, b) ((uint16_t)(((uint16_t)(a) << 8U) | ((uint8_t)(b))))

bool MessageParser::isValid() {
  // Check is SOM and EOM encountered
  if (!started_ || !ended_) return false;

  // Check length
//  if (length_ != payload_.size()) return false;

  // Checksum
//  auto lambda = [](uint8_t a, uint8_t b) { return uint8_t(a + b); };
//  uint8_t data_check_sum = accumulate(payload_.begin(), payload_.end(), 0, lambda);
//  if (check_sum_ != data_check_sum) return false;

  return true;
}

void MessageParser::addByte(uint8_t b) {
  auto getCheckSum = [&]() {
    uint32_t sum = 0;
    for (uint8_t payload_byte : payload_)
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
  ResponseMessage *p;
  if (!isValid()) return nullptr;
  switch (payload_[0]) {
    case ACK:
      p = new AckResponse();
      break;
    case NAK:
      p = new NakResponse(payload_[1]);
      break;
    case ProtocolRevision:
      p = new ProtocolRevisionResponse(P16(payload_[1], payload_[2]));
      break;
    case DataGroup:
      p = new DataGroupResponse(payload_);
      break;
    default:
      printf("TODO: Create response class");
      p = nullptr;
  }
  reset();
  return p;
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

string CommandBuilder::requestDataGroup(DataGroup group_id) {
  string command, payload;
  command += SOM;
  payload += (uint8_t)RequestDataGroup;
  payload += (uint8_t)group_id;
  command += (uint8_t)payload.size();
  command += payload;
  uint8_t checksum = 0;
  for (uint8_t p : payload) checksum += p;
  command += checksum;
  command += EOM;
  return command;
}

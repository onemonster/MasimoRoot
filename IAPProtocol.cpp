#include "IAPProtocol.h"
#include "response_message/AckResponse.h"
#include "response_message/NakResponse.h"
#include "response_message/ProtocolRevisionResponse.h"
#include "response_message/DataGroupResponse.h"
#include "response_message/UnknownResponse.h"
#include "response_message/ChannelWaveformResponse.h"
#include "response_message/ChannelWaveformDataResponse.h"
#include <numeric>

using namespace std;
using namespace IAP;

#define P16(a, b) ((uint16_t)(((uint16_t)(a) << 8U) | ((uint8_t)(b))))
#define P32(a, b, c, d) (((uint32_t)P16((a), (b))) << 16U | P16((c), (d)))

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
//  printf("\n");
//  for (auto c : payload_) {
//    printf("0x%02x ", c);
//  }
//  printf("\n");
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
    case ChannelWaveformResponse:
    {
      uint32_t channel_id = P32(payload_[1], payload_[2], payload_[3], payload_[4]);
      if (channel_id == CH_EEG) {
        p = new class ChannelWaveformResponse(channel_id, P16(payload_[5], payload_[6]), payload_[7],
            payload_[8], P16(payload_[9], payload_[10]), P16(payload_[11], payload_[12]),
            payload_[13], P16(payload_[14], payload_[15]), P16(payload_[16], payload_[17]));
      } else {
        p = new class ChannelWaveformResponse(channel_id, P16(payload_[5], payload_[6]), payload_[7]);
      }
      break;
    }
    case ChannelWaveformData: {
      auto waveform_payload_start = payload_.begin() + 4;
      auto waveform_payload_end = payload_.end();
      auto waveform_payload = vector(waveform_payload_start, waveform_payload_end);
      p = new ChannelWaveformDataResponse(payload_[1], payload_[2], payload_[3], waveform_payload);
      break;
    }
    default:
      p = new UnknownResponse(payload_);
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
  payload += (uint8_t) RequestDataGroup;
  payload += (uint8_t) group_id;
  command += (uint8_t) payload.size();
  command += payload;
  uint8_t checksum = 0;
  for (uint8_t p : payload) checksum += p;
  command += checksum;
  command += EOM;
  return command;
}

string CommandBuilder::requestPeriodicDataGroupDelivery(DataGroup group_id) {
  string command, payload;
  command += SOM;
  payload += (uint8_t) RequestPeriodicDataGroupDelivery;
  payload += (uint8_t) group_id;
  payload += (uint8_t) 0x01U;
  command += (uint8_t) payload.size();
  command += payload;
  uint8_t checksum = 0;
  for (uint8_t p : payload) checksum += p;
  command += checksum;
  command += EOM;
  return command;
}

string CommandBuilder::cancelAllPeriodicDataGroupDelivery() {
  return "\xC3\x01\x22\x22\xC6";
}

string CommandBuilder::cancelSpecificPeriodicGroupDelivery(DataGroup group_id) {
  string command, payload;
  command += SOM;
  payload += (uint8_t) CancelSpecificPeriodicDataGroupDelivery;
  payload += (uint8_t) group_id;
  command += (uint8_t) payload.size();
  command += payload;
  uint8_t checksum = 0;
  for (uint8_t p : payload) checksum += p;
  command += checksum;
  command += EOM;
  return command;
}

string CommandBuilder::channelWaveformRequest(ChannelId channel_id, uint16_t waveform_bitmask) {
  string command, payload;
  command += SOM;
  payload += (uint8_t) ChannelWaveformRequest;
  payload += (uint8_t) ((uint32_t) channel_id >> 24U);
  payload += (uint8_t) ((uint32_t) channel_id >> 16U);
  payload += (uint8_t) ((uint32_t) channel_id >> 8U);
  payload += (uint8_t) ((uint32_t) channel_id);
  payload += (uint8_t) ((uint16_t) waveform_bitmask >> 8U);
  payload += (uint8_t) ((uint16_t) waveform_bitmask);
  command += (uint8_t) payload.size();
  command += payload;
  uint8_t checksum = 0;
  for (uint8_t p : payload) checksum += p;
  command += checksum;
  command += EOM;
  return command;
}

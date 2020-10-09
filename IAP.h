#pragma once

#include <vector>
#include <string>
#include "response_message/ResponseMessage.h"
#include "response_message/DataGroup.h"

namespace IAP {
  const uint8_t SOM = 0xC3; // Start of Message
  const uint8_t EOM = 0xC6; // End of Message
  const uint8_t LEN_MARK = 0xFF; // Length is expressed in two bytes

  class MessageParser {
  public:
    bool isValid();

    void addByte(uint8_t b);

    ResponseMessage *build();

    void reset();

  private:
    enum ParseState {
      START_STATE = 0,
      CHECK_LEN_MARK_STATE,
      LENGTH_STATE,
      PAYLOAD_STATE,
      CHECK_SUM_STATE,
      END_STATE
    };

    enum ResponseCode {
      ACK = 0x01,
      NAK = 0x02,
      ProtocolRevision = 0x10,
      DataGroup = 0x20,
      ChannelWaveformResponse = 0x35,
      PoweringDownNotification = 0x60,
      ChannelWaveformData = 0xE0
    };

    std::vector<uint8_t> payload_;
    ParseState state_ = START_STATE;
    bool started_ = false;
    bool ended_ = false;
    bool marked_flag_ = false;
    uint16_t length_ = 0;
    uint16_t data_count_ = 0;
    uint8_t check_sum_ = 0;
  };

  namespace CommandBuilder {
    enum CommandCode {
      RequestProtocolRevision = 0x10,
      KeepAliveRequest = 0x11,
      RequestDataGroup = 0x20,
      RequestPeriodicDataGroupDelivery = 0x21,
      CancelAllPeriodicDataGroupDelivery = 0x22,
      CancelSpecificPeriodicDataGroupDelivery = 0x23,
      ChannelWaveformRequest = 0x35
    };

    std::string ack();

    std::string keepAlive(); // Must call every two seconds

    std::string requestDataGroup(DataGroup group_id);

    std::string requestPeriodicDataGroupDelivery(DataGroup group_id);

    std::string cancelAllPeriodicDataGroupDelivery();

    std::string cancelSpecificPeriodicGroupDelivery(DataGroup group_id);

    std::string channelWaveformRequest(uint32_t channel_id, uint16_t waveform_bitmask);
  }
}

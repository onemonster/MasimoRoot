#pragma once

#include <vector>
#include <string>
#include "response_message/ResponseMessage.h"

namespace IAP {
  const unsigned char SOM = 0xC3; // Start of Message
  const unsigned char EOM = 0xC6; // End of Message
  const unsigned char LEN_MARK = 0xFF; // Length is expressed in two bytes

  class MessageParser {
  public:
    bool isValid();

    void addByte(unsigned char b);

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

    std::vector<unsigned char> payload_;
    ParseState state_ = START_STATE;
    bool started_ = false;
    bool ended_ = false;
    bool marked_flag_ = false;
    unsigned short length_ = 0;
    unsigned short data_count_ = 0;
    unsigned char check_sum_ = 0;
  };

  namespace CommandBuilder {
    std::string ack();

    std::string keepAlive(); // Must call every two seconds
    std::string requestDataGroup(unsigned char group_id);

    std::string requestPeriodicDataGroupDelivery(unsigned char group_id);

    std::string cancelAllPeriodicDataGroupDelivery();

    std::string cancelSpecificPeriodicGroupDelivery(unsigned char group_id);

    std::string channelWaveformRequest(unsigned int channel_id, unsigned short waveform_bitmask);
  }
}

#pragma once

#include <cstdint>
#include <vector>
#include "ResponseMessage.h"

class ChannelWaveformDataResponse : public ResponseMessage {
public:
  ChannelWaveformDataResponse(uint8_t streaming_handle,
                              uint8_t ordinal_number,
                              uint8_t points_per_waveform,
                              std::vector<uint8_t> waveform_payload);
  ~ChannelWaveformDataResponse() override = default;

  void record() override;

  void record(uint32_t channel_id, uint16_t waveform_bitmask, uint8_t previous_ordinal_number);

  uint8_t get_handle();

  uint8_t get_ordinal_number();

private:
  uint8_t streaming_handle_;
  uint8_t ordinal_number_;
  uint8_t points_per_waveform_;
  std::vector<uint8_t> waveform_payload_;
};

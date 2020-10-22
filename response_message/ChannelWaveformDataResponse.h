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

private:
  uint8_t streaming_handle_;
  uint8_t ordinal_number_;
  uint8_t points_per_waveform_;
  std::vector<uint8_t> waveform_payload_;
};

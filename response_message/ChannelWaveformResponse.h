#pragma once

#include <cstdint>
#include "ResponseMessage.h"

class ChannelWaveformResponse : public ResponseMessage {
public:
  ChannelWaveformResponse(uint32_t channel_id, uint16_t waveform_bitmask, uint8_t streaming_handle);
  ChannelWaveformResponse(uint32_t channel_id, uint16_t waveform_bitmask, uint8_t streaming_handle,
                          uint8_t mmps, uint16_t num_of_pixels_x, uint16_t screen_size_mm_x,
                          uint8_t mvpmm, uint16_t num_of_pixels_y, uint16_t screen_size_mm_y);
  ~ChannelWaveformResponse() override = default;

  void record() override;

private:
  uint32_t channel_id_;
  uint16_t waveform_bitmask_;
  uint8_t streaming_handle_;

  // Only for EEG, page 13 of 63
  uint8_t mmps_ = 0;              // Millimeters per second: how much sample in mm to be generated per seconds.
  uint16_t num_of_pixels_x_ = 0;  // Number of screen pixels on X-axis
  uint16_t screen_size_mm_x_ = 0; // Screen X-axis length in millimeters.
  uint8_t mvpmm_ = 0;             // Millimeters per second: how much sample in mm to be generated per seconds.
  uint16_t num_of_pixels_y_ = 0;  // Number of screen pixels on Y-axis
  uint16_t screen_size_mm_y_ = 0; // Screen y-axis length in millimeters.
};

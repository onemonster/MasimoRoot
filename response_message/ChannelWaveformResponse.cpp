#include <cstdio>
#include "ChannelWaveformResponse.h"
#include "ChannelId.h"

ChannelWaveformResponse::ChannelWaveformResponse(uint32_t channel_id, uint16_t waveform_bitmask,
                                                 uint8_t streaming_handle) {
  channel_id_ = channel_id;
  waveform_bitmask_ = waveform_bitmask;
  streaming_handle_ = streaming_handle;
}

ChannelWaveformResponse::ChannelWaveformResponse(uint32_t channel_id, uint16_t waveform_bitmask,
                                                 uint8_t streaming_handle, uint8_t mmps, uint16_t num_of_pixels_x,
                                                 uint16_t screen_size_mm_x, uint8_t mvpmm, uint16_t num_of_pixels_y,
                                                 uint16_t screen_size_mm_y) {
  channel_id_ = channel_id;
  waveform_bitmask_ = waveform_bitmask;
  streaming_handle_ = streaming_handle;
  mmps_ = mmps;
  num_of_pixels_x_ = num_of_pixels_x;
  screen_size_mm_x_ = screen_size_mm_x;
  mvpmm_ = mvpmm;
  num_of_pixels_y_ = num_of_pixels_y;
  screen_size_mm_y_ = screen_size_mm_y;
}

void ChannelWaveformResponse::record() {
  if (streaming_handle_ == 0xFF) {
    printf("[ChannelWaveform][Invalid] Channel Id: 0x%x, Waveform Bitmask: 0x%x\n",
           channel_id_, waveform_bitmask_);
  } else {
    if (channel_id_ == CH_EEG) {
      printf("[ChannelWaveform] Channel Id: 0x%x, Waveform Bitmask: 0x%x, Streaming Handle: 0x%x\n",
             channel_id_, waveform_bitmask_, streaming_handle_);
      printf("|-%dmm/sec %dμV/mm\n", mmps_, mvpmm_);
      printf("|-[Number of pixels (x,y)] (%5d, %5d)\n", num_of_pixels_x_, num_of_pixels_y_);
      printf("|-[Screensize in mm (x,y)] (%5d, %5d)\n", screen_size_mm_x_, screen_size_mm_y_);
    }
    else {
      printf("[ChannelWaveform] Channel Id: 0x%x, Waveform Bitmask: 0x%x, Streaming Handle: 0x%x\n",
             channel_id_, waveform_bitmask_, streaming_handle_);
    }
  }
}

std::pair<uint32_t, uint16_t> ChannelWaveformResponse::get_id() {
  return { channel_id_, waveform_bitmask_ };
}

uint8_t ChannelWaveformResponse::get_handle() {
  return streaming_handle_;
}

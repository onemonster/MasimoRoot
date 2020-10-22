#include "ChannelWaveformDataResponse.h"

#include <utility>
#include <cstdio>

using namespace std;

ChannelWaveformDataResponse::ChannelWaveformDataResponse(uint8_t streaming_handle, uint8_t ordinal_number,
                                                         uint8_t points_per_waveform,
                                                         vector<uint8_t> waveform_payload) {
  streaming_handle_ = streaming_handle;
  ordinal_number_ = ordinal_number;
  points_per_waveform_ = points_per_waveform;
  waveform_payload_ = move(waveform_payload);
}

void ChannelWaveformDataResponse::record() {
  if (ordinal_number_ == 0xFF) {
    printf("[WaveformData][%03d][End of Stream]\n", streaming_handle_);
  } else {
    printf("[WaveformData][%03d][%03d] %d payload size: %d \n",
           streaming_handle_, ordinal_number_, points_per_waveform_, (int)waveform_payload_.size());
    int waveform_count = (int)waveform_payload_.size() / points_per_waveform_;
    for (int i = 0; i < waveform_count; i++) {
      printf("waveform %d:", i + 1);
      for (int j = 0; j < points_per_waveform_; j++) {
        printf("0x%02x ", waveform_payload_[i * points_per_waveform_ + j]);
      }
      printf("\n");
    }
  }
}

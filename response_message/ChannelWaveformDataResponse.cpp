#include "ChannelWaveformDataResponse.h"
#include "ChannelId.h"

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
    printf("[h:%03d][End of Stream]\n", streaming_handle_);
  } else {
    printf("[h:%03d][o:%03d] points per waveform: %d, payload size: %d \n",
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

void ChannelWaveformDataResponse::record(uint32_t channel_id, uint16_t waveform_bitmask, uint8_t previous_ordinal) {
  int difference = (ordinal_number_ - ((previous_ordinal + 1) % 251)) % 250;
  if (difference) printf("Missed %d ChannelWaveformDataResponses\n", difference);
  switch (channel_id) {
    case CH_Optical:
      printf("[Optical]");
      break;
    case CH_Acoustic:
      printf("[Acoustic   ]");
      break;
    case CH_EEG:
      printf("[EEG        ]");
      break;
    case CH_Capnography:
      printf("[Capnography]");
      break;
    case CH_RegoxA:
      printf("[RegoxA     ]");
      break;
    case CH_RegoxB:
      printf("[RegoxB     ]");
      break;
    case CH_NIBP:
      printf("[NIBP       ]");
      break;
    case CH_Thermometer:
      printf("[Thermometer]");
      break;
  }
  printf("[0x%04x]", waveform_bitmask);
  record();
}

uint8_t ChannelWaveformDataResponse::get_handle() {
  return streaming_handle_;
}

uint8_t ChannelWaveformDataResponse::get_ordinal_number() {
  return ordinal_number_;
}

#pragma once

#define WF_CANCEL  0x0000

// Capnography Channel Waveform
#define WF_CAP_CO2 0x0001
#define WF_CAP_O2  0x0002
#define WF_CAP_AA1 0x0004

// EEG Channel Waveform Bitmask
#define WF_EEG_REF 0x0001
#define WF_EEG_BIP 0x0002

// Rainbow Channel Waveform Bitmask
#define WF_RBW_PTH 0x0001
#define WF_RBW_ACU 0x0002
#define WF_RBW_SIQ 0x0004

enum ChannelId {
  CH_Optical = 0x00000001,
  CH_Acoustic = 0x00000002,
  CH_EEG = 0x00000004,
  CH_Capnography = 0x00000008,
  CH_RegoxA = 0x00000010,
  CH_RegoxB = 0x00000020,
  CH_NIBP = 0x00000040,
  CH_Thermometer = 0x00000080,
};

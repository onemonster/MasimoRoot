#pragma once

enum DataGroup {
  DG_SerialNumbers = 0x01,
  DG_Versions = 0x02,
  DG_SpO2 = 0x0A,
  DG_SpHb = 0x10,
  DG_DeviceInfo = 0x29,
  DG_ChannelOptical = 0x2A,
  DG_ChannelAcoustic = 0x2B,
  DG_ChannelEEG = 0x2C,
  DG_ChannelCapnography = 0x2D,
};

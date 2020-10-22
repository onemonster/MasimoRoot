#pragma once

#include <cstdint>
#include <vector>
#include <string>
#include "ResponseMessage.h"

class DataGroupResponse : public ResponseMessage {
public:
  explicit DataGroupResponse(std::vector<uint8_t> payload);
  ~DataGroupResponse() override = default;

  void record() override;

private:

  // Table 2-1, page 28 of 63
  enum OpticalChannelParameter {
    OCP_FUNCTIONAL_SpO2 = 0x00000001,
    OCP_PR              = 0x00000002, // Pulse Rate
    OCP_PERFUSION_INDEX = 0x00000004,
    OCP_SpCO            = 0x00000008,
    OCP_SpMet           = 0x00000010,
    OCP_SpHb            = 0x00000040,
    OCP_SpOC            = 0x00002000, // O2 Content
    OCP_PVi             = 0x00004000, // Pleth Variablility Index
    OCP_RRp             = 0x01000000,
    OCP_ORi             = 0x02000000,
    OCP_RPVi            = 0x04000000, // Rainbow Pleth Variablility Index
  };

  enum EEGChannelParamter {
    ECP_PSI  = 0x00000001,
    ECP_EMG  = 0x00000002,
    ECP_SUP  = 0x00000004,
    ECP_SEFL = 0x00000010,
    ECP_SEFR = 0x00000020,
    ECP_ARTF = 0x00000040,
  };

  enum EEGChannelException {
    ECE_NoCableConnected                 = 0x00000001,
    ECE_IncompatibleCable                = 0x00000002,
    ECE_CableLifeExpired                 = 0x00000004,
    ECE_DefectiveCable                   = 0x00000008,
    ECE_NoSensorConnected                = 0x00000010,
    ECE_IncompatibleSensor               = 0x00000020,
    ECE_SensorLifeExpired                = 0x00000040,
    ECE_DefectiveSensor                  = 0x00000080,
    ECE_SensorOffPatient                 = 0x00000100,
    ECE_NoTapeConnected                  = 0x00000200,
    ECE_IncompatibleTape                 = 0x00000400,
    ECE_DefectiveTape                    = 0x00000800,
    ECE_TapeLifeExpired                  = 0x00001000,
    ECE_DemoMode                         = 0x00002000,
    ECE_PatientInterferenceDetected      = 0x00004000,
    ECE_ImpedanceValueIndeterminate      = 0x00008000,
    ECE_HighImpedanceDetected            = 0x00010000,
    ECE_GelBridgingDetected              = 0x00020000,
    ECE_ImproperSensorConnectionDetected = 0x00040000,
    ECE_ReplaceSensor24Hours             = 0x00080000,
  };

  // Table 2-4, page 30 of 63
  enum SpO2Exception {
    SpO2_SUCCESS = 0x0000,
    SpO2_LOW_SIGNAL_IQ = 0x0001,
    SpO2_INVALID_FUNCTIONAL_SPO2 = 0x0004,
    SpO2_STARTUP_STATE = 0x0010
  };

  // Table 2-9, page 31 of 63
  enum SpHbException {
    SpHb_SUCCESS = 0x0000,
    SpHb_LOW_SPHB_CONFIDENCE = 0x0001,
    SpHb_LOW_SPHB_PERFUSION_INDEX = 0x0002,
    SpHb_INVALID_SPHB = 0x0004,
    SpHb_STARTUP_STATE = 0x0010
  };

  // Table 2-10, page 31 of 63
  enum SpHbPrecisionMode {
    NEAREST_0_DOT_1 = 0x00,
    NEAREST_0_DOT_5 = 0x01,
    WHOLE_NUMBER = 0x02,
  };

  // Table 9-7, page 51 of 63
  enum Dimension {
    MDC_DIM_DIMLESS = 512,
    MDC_DIM_PERCENT = 544,           // (%)
    MDC_DIM_X_G_PER_DL = 2112,       // (g/dL)
    MDC_DIM_HZ = 2496,
    MDC_DIM_BEAT_PER_MIN = 2720,
    MDC_DIM_RESP_PER_MIN = 2784,
    MDC_DIM_KILO_PASCAL = 3843,      // (KPA)
    MDC_DIM_MMHG = 3872,             // (mmhg)
    MDC_DIM_FAHR = 4416,
    MDC_DIM_MILLI_MOLE_PER_L = 4722, // (mmol/L)
    MDC_DIM_DEGC = 6048,
    MDC_DIM_MILLI_L_PER_DL = 6418,
  };

  int ri_; // record index
  std::vector<uint8_t> p_;

  static std::string valueFormatted(int32_t value, uint16_t dimension, uint16_t divisor, int precision);

  int Scan08(int8_t *dest); // returns index
  int Scan08(uint8_t *dest); // returns index
  int Scan16(int16_t *dest); // returns index
  int Scan16(uint16_t *dest); // returns index
  int Scan32(int32_t *dest); // returns index
  int Scan32(uint32_t *dest); // returns index
  int Scan48(int64_t *dest); // returns index
  int Scan48(uint64_t *dest); // returns index
  int Scan64(int64_t *dest); // returns index
  int Scan64(uint64_t *dest); // returns index
};

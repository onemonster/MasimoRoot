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

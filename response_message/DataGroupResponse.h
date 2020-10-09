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

  enum SpO2Exception {
    SUCCESS = 0x0000,
    LOW_SIGNAL_IQ = 0x0001,
    INVALID_FUNCTIONAL_SPO2 = 0x0004,
    STARTUP_STATE = 0x0010
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

private:
  int ri_; // record index
  std::vector<uint8_t> p_;

  std::string valueFormatted(int32_t value, uint16_t dimension);

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

#include "DataGroupResponse.h"
#include "DataGroup.h"
#include <utility>
#include <cstdio>
#include <iostream>

using namespace std;

#define P16(a, b) ((uint16_t)(((uint16_t)(a) << 8U) | ((uint8_t)(b))))
#define P32(a, b, c, d) (((uint32_t)P16((a), (b))) << 16U | P16((c), (d)))
#define P48(a, b, c, d, e, f) (((uint64_t)P16((a),(b))) << 32U | P32((c),(d),(e),(f)))
#define P64(a, b, c, d, e, f, g, h) (((uint64_t)P32((a),(b),(c),(d))) << 32U | P32((e),(f),(g),(h)))

DataGroupResponse::DataGroupResponse(std::vector<uint8_t> payload) {
  p_ = std::move(payload);
}

void DataGroupResponse::record() {
  ri_ = 2;

  // etc
  uint64_t serial_number;
  uint16_t i_v, dsp_v, mcu_v;

  uint16_t par_exceptions;
  // Table 1-12
  int32_t par_value;
  uint16_t par_divisor;
  uint16_t par_alarm_status;
  uint16_t par_dimension;
  uint8_t high_alarm_limit_configuration;
  int32_t high_alarm_limit;
  uint8_t low_alarm_limit_configuration;
  int32_t low_alarm_limit;

  switch (p_[1]) {
    case SerialNumbers:
      Scan48(&serial_number);
      cout<<"[Serial Number] "<<serial_number<<endl;
      break;
    case Versions:
      Scan16(&i_v); Scan16(&dsp_v); Scan16(&mcu_v);
      printf("[Versions] I(V.%d) DSP(V.%d) MCU(V.%d)\n", i_v, dsp_v, mcu_v);
      break;
    case SpO2:
      Scan32(&par_value);
      Scan16(&par_divisor);
      Scan16(&par_exceptions);
      Scan16(&par_alarm_status);
      Scan08(&high_alarm_limit_configuration);
      Scan32(&high_alarm_limit);
      Scan08(&low_alarm_limit_configuration);
      Scan32(&low_alarm_limit);
      Scan16(&par_dimension);
      switch (par_exceptions) {
        case SUCCESS: break;
        case LOW_SIGNAL_IQ: printf("[Exception][SpO2] Low Signal IQ\n"); break;
        case INVALID_FUNCTIONAL_SPO2: printf("[Exception][SpO2] Invalid Functional SpO2\n"); break;
        case STARTUP_STATE: printf("[Exception][SpO2] Startup State\n"); break;
        default: printf("[Exception][SpO2] Unknown = %x\n", par_exceptions); break;
      }
      cout<<"[SpO2] "<<valueFormatted(par_value, par_dimension)<<endl;
      break;
    default:
      cout<<"[DataGroup] Undefined"<<endl;
  }
}

string DataGroupResponse::valueFormatted(int32_t value, uint16_t dimension) {
  switch (dimension) {
    case MDC_DIM_DIMLESS: return to_string(value);
    case MDC_DIM_PERCENT: return to_string(value) + "%";
    case MDC_DIM_X_G_PER_DL: return to_string(value) + "g/dL";
    case MDC_DIM_HZ: return to_string(value) + "Hz";
    case MDC_DIM_BEAT_PER_MIN: return to_string(value) + "BPM";
    case MDC_DIM_RESP_PER_MIN: return to_string(value) + "RPM";
    case MDC_DIM_KILO_PASCAL: return to_string(value) + "KPA";
    case MDC_DIM_MMHG: return to_string(value) + "mmhg";
    case MDC_DIM_FAHR: return to_string(value) + "°F";
    case MDC_DIM_MILLI_MOLE_PER_L: return to_string(value) + "mmol/L";
    case MDC_DIM_DEGC: return to_string(value) + "°C";
    case MDC_DIM_MILLI_L_PER_DL:return to_string(value) + "mL/dL";
    default: printf("[DEBUG] Unknown Dimension");
  }
  return to_string(value);
}

int DataGroupResponse::Scan08(int8_t *dest) {
  if (ri_ + 1 > p_.size()) { printf("Scan08 out of bounds.\n"); return -1; }
  *dest = p_[ri_];
  return ri_ += 1;
}

int DataGroupResponse::Scan08(uint8_t *dest) {
  if (ri_ + 1 > p_.size()) { printf("Scan08 out of bounds.\n"); return -1; }
  *dest = p_[ri_];
  return ri_ += 1;
}

int DataGroupResponse::Scan16(int16_t *dest) {
  if (ri_ + 2 > p_.size()) { printf("Scan16 out of bounds.\n"); return -1; }
  *dest = P16(p_[ri_], p_[ri_ + 1]);
  return ri_ += 2;
}

int DataGroupResponse::Scan16(uint16_t *dest) {
  if (ri_ + 2 > p_.size()) printf("Scan16 out of bounds.\n");
  *dest = P16(p_[ri_], p_[ri_ + 1]);
  return ri_ += 2;
}

int DataGroupResponse::Scan32(int32_t *dest) {
  if (ri_ + 4 > p_.size()) { printf("Scan32 out of bounds.\n"); return -1; }
  *dest = P32(p_[ri_], p_[ri_ + 1], p_[ri_ + 2], p_[ri_ + 3]);
  return ri_ += 4;
}

int DataGroupResponse::Scan32(uint32_t *dest) {
  if (ri_ + 4 > p_.size()) { printf("Scan32 out of bounds.\n"); return -1; }
  *dest = P32(p_[ri_], p_[ri_ + 1], p_[ri_ + 2], p_[ri_ + 3]);
  return ri_ += 4;
}

int DataGroupResponse::Scan48(int64_t *dest) {
  if (ri_ + 6 > p_.size()) { printf("Scan48 out of bounds.\n"); return -1; }
  *dest = P48(p_[ri_], p_[ri_ + 1], p_[ri_ + 2], p_[ri_ + 3], p_[ri_ + 4], p_[ri_ + 5]);
  return ri_ += 6;
}

int DataGroupResponse::Scan48(uint64_t *dest) {
  if (ri_ + 6 > p_.size()) { printf("Scan48 out of bounds.\n"); return -1; }
  *dest = P48(p_[ri_], p_[ri_ + 1], p_[ri_ + 2], p_[ri_ + 3], p_[ri_ + 4], p_[ri_ + 5]);
  return ri_ += 6;
}

int DataGroupResponse::Scan64(int64_t *dest) {
  if (ri_ + 8 > p_.size()) { printf("Scan64 out of bounds.\n"); return -1; }
  *dest = P64(p_[ri_], p_[ri_ + 1], p_[ri_ + 2], p_[ri_ + 3], p_[ri_ + 4], p_[ri_ + 5], p_[ri_ + 6], p_[ri_ + 7]);
  return ri_ += 8;
}

int DataGroupResponse::Scan64(uint64_t *dest) {
  if (ri_ + 8 > p_.size()) { printf("Scan64 out of bounds.\n"); return -1; }
  *dest = P64(p_[ri_], p_[ri_ + 1], p_[ri_ + 2], p_[ri_ + 3], p_[ri_ + 4], p_[ri_ + 5], p_[ri_ + 6], p_[ri_ + 7]);
  return ri_ += 8;
}

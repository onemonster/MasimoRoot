#include "DataGroupResponse.h"
#include "DataGroup.h"
#include <utility>
#include <cstdio>
#include <iostream>
#include <sstream>

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

  uint16_t par_exceptions;
  // Table 1-12, page 27 of 63
  int32_t par_value;
  uint16_t par_divisor;
  uint16_t par_alarm_status;
  uint8_t high_alarm_limit_configuration;
  int32_t high_alarm_limit;
  uint8_t low_alarm_limit_configuration;
  int32_t low_alarm_limit;

  // Table 9-7, page 51 of 63
  uint16_t dimension;

  switch (p_[1]) {
    case DG_SerialNumbers:
    {
      uint64_t serial_number;
      Scan48(&serial_number);
      cout<<"[Serial Number] "<<serial_number<<endl;
      break;
    }
    case DG_Versions:
    {
      uint16_t i_v, dsp_v, mcu_v;
      Scan16(&i_v); Scan16(&dsp_v); Scan16(&mcu_v);
      printf("[Versions] I(V.%d) DSP(V.%d) MCU(V.%d)\n", i_v, dsp_v, mcu_v);
      break;
    }
    case DG_SpO2:
      Scan32(&par_value); Scan16(&par_divisor);
      Scan16(&par_exceptions);
      Scan16(&par_alarm_status);
      Scan08(&high_alarm_limit_configuration); Scan32(&high_alarm_limit);
      Scan08(&low_alarm_limit_configuration);  Scan32(&low_alarm_limit);
      Scan16(&dimension);
      switch (par_exceptions) {
        case SpO2_SUCCESS: break;
        case SpO2_LOW_SIGNAL_IQ: printf("[Exception][SpO2] Low Signal IQ\n"); break;
        case SpO2_INVALID_FUNCTIONAL_SPO2: printf("[Exception][SpO2] Invalid Functional SpO2\n"); break;
        case SpO2_STARTUP_STATE: printf("[Exception][SpO2] Startup State\n"); break;
        default: printf("[Exception][SpO2] Unknown = %x\n", par_exceptions); break;
      }
      cout<<"[SpO2] "<<valueFormatted(par_value, dimension, par_divisor, 0)<<endl;
      break;
    case DG_SpHb:
    {
      // Table 2-10, page 31 of 63
      uint8_t sphb_precision_mode;
      Scan32(&par_value); Scan16(&par_divisor);
      Scan16(&par_exceptions);
      Scan16(&par_alarm_status);
      Scan08(&high_alarm_limit_configuration); Scan32(&high_alarm_limit);
      Scan08(&low_alarm_limit_configuration);  Scan32(&low_alarm_limit);
      Scan08(&sphb_precision_mode);
      Scan16(&dimension);
      switch (par_exceptions) {
        case SpHb_SUCCESS: break;
        case SpHb_LOW_SPHB_CONFIDENCE: printf("[Exception][SpHb] Low SpHb Confidence\n"); break;
        case SpHb_LOW_SPHB_PERFUSION_INDEX: printf("[Exception][SpHb] Low SpHb Perfusion Index\n"); break;
        case SpHb_INVALID_SPHB: printf("[Exception][SpHb] Invalid SpHb\n"); break;
        case SpHb_STARTUP_STATE: printf("[Exception][SpHb] Startup State\n"); break;
        default: printf("[Exception][SpHb] Unknown = %x\n", par_exceptions); break;
      }
      cout << "[SpHb] "<<valueFormatted(par_value, dimension, par_divisor, 1)<<endl;
      break;
    }
    case DG_DeviceInfo:
    {
      uint8_t sv[12], type; // software version
      for (unsigned char & i : sv) Scan08(&i);
      Scan08(&type);
      uint64_t serial_number;
      Scan48(&serial_number);
      uint32_t alarm_channels, system_fault, monitoring_channels;
      uint16_t alarm_status;
      Scan32(&alarm_channels);
      Scan16(&alarm_status);
      Scan32(&system_fault);
      Scan32(&monitoring_channels);
      printf("[DeviceInfo] %x\n", alarm_channels);
      break;
    }
    default:
      printf("[DataGroup Unknown] %x\n", p_[1]);
  }
}

string DataGroupResponse::valueFormatted(int32_t value, uint16_t dimension, uint16_t divisor, int precision) {
  ostringstream out;
  out.precision(precision);
  if (divisor) out<<fixed<<((double )value / divisor);
  else out<<value;
  string value_string = out.str();
  switch (dimension) {
    case MDC_DIM_DIMLESS: return value_string;
    case MDC_DIM_PERCENT: return value_string + "%";
    case MDC_DIM_X_G_PER_DL: return value_string + "g/dL";
    case MDC_DIM_HZ: return value_string + "Hz";
    case MDC_DIM_BEAT_PER_MIN: return value_string + "BPM";
    case MDC_DIM_RESP_PER_MIN: return value_string + "RPM";
    case MDC_DIM_KILO_PASCAL: return value_string + "KPA";
    case MDC_DIM_MMHG: return value_string + "mmhg";
    case MDC_DIM_FAHR: return value_string + "°F";
    case MDC_DIM_MILLI_MOLE_PER_L: return value_string + "mmol/L";
    case MDC_DIM_DEGC: return value_string + "°C";
    case MDC_DIM_MILLI_L_PER_DL:return value_string + "ml/dl";
    default: printf("[DEBUG] Unknown Dimension");
  }
  return value_string;
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

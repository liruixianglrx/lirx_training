#include "ipublisher.h"

#include <cstring>
#include <iostream>
#include <string>

#include "../XyMarketData.h"

template <typename DataType>
IPublisher<DataType>::IPublisher() {}

template <typename DataType>
IPublisher<DataType>::~IPublisher() {}

template <typename DataType>
void IPublisher<DataType>::TranlateData(unsigned char* raw_data) {
  m_data = reinterpret_cast<DataType*>(raw_data);
  m_process_index = 0;
  if constexpr (std::is_same<DataType, XyMarketData>()) {
    m_size = 10;
  } else {
    m_size = -1;
  }
}

template <typename DataType>
std::string IPublisher<DataType>::ParseOneRow() {
  m_process_index++;
  static std::string prefix(25, '.');
  if constexpr (std::is_same<DataType, XyMarketData>()) {
    switch (m_process_index) {
      case 1:
        return prefix + "\nwind_code is: " + std::string(m_data->wind_code);
      case 2:
        return "code is : " + std::string(m_data->code);
        // TODO: code has bug? first two bits were showing
      case 3:
        return "action_day is: " + std::to_string(m_data->action_day);
      case 4:
        return "trading_day is: " + std::to_string(m_data->trading_day);
      case 5:
        return "update_time is: " + std::to_string(m_data->update_time);
      case 6:
        return "status is: " + std::to_string(m_data->status);
      case 7:
        return "pre_close is: " + std::to_string(m_data->pre_close);
      case 8:
        return "open is: " + std::to_string(m_data->open);
      case 9:
        return "high is: " + std::to_string(m_data->high);
      case 10:
        return "low is: " + std::to_string(m_data->low);
      case 11:
        return "last_price is: " + std::to_string(m_data->last_price);
      case 12:
        return GetVectorReturn("ask_price", m_data->ask_price, 5);
      case 13:
        return GetVectorReturn("ask_vol", m_data->ask_vol, 5);
      case 14:
        return GetVectorReturn("bid_price", m_data->bid_price, 5);
      case 15:
        return GetVectorReturn("bid_volume", m_data->bid_volume, 5);
      case 16:
        return "num_trades is: " + std::to_string(m_data->num_trades);
      case 17:
        return "volume is: " + std::to_string(m_data->volume);
      case 18:
        return "turnover is: " + std::to_string(m_data->turnover);
      case 19:
        return "total_bid_vol is: " + std::to_string(m_data->total_bid_vol);
      case 20:
        return "total_ask_vol is: " + std::to_string(m_data->total_ask_vol);
      case 21:
        return "weighted_avg_bid_price is: " +
               std::to_string(m_data->weighted_avg_bid_price);
      case 22:
        return "weighted_avg_ask_price is: " +
               std::to_string(m_data->weighted_avg_ask_price);
      case 23:
        return "iopv is: " + std::to_string(m_data->iopv);
      case 24:
        return "yield_to_maturity is: " +
               std::to_string(m_data->yield_to_maturity);
      case 25:
        return "high_limited is: " + std::to_string(m_data->high_limited);
      case 26:
        return "low_limited is: " + std::to_string(m_data->low_limited);
      case 27:
        return "prefix is: " + std::string(m_data->prefix);
      case 28:
        return "syl1 is: " + std::to_string(m_data->syl1);
      case 29:
        return "syl2 is: " + std::to_string(m_data->syl2);
      case 30:
        return "sd2 is: " + std::to_string(m_data->sd2);

      default:
        break;
    }
  }
  return nullptr;
}

template <typename DataType>
std::string IPublisher<DataType>::GetVectorReturn(const char* prefix,
                                                  uint32_t* arr, int times) {
  std::string result, sprefix(prefix);
  for (int i = 0; i < times; i++) {
    result += sprefix + char('1' + i) + " is: " + std::to_string(arr[i]) + "\n";
  }
  result.pop_back();
  return result;
}

template class IPublisher<XyMarketData>;
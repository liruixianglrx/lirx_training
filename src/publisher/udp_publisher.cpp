#include "udp_publisher.h"

template <typename DataType>
UDPPublisher<DataType>::UDPPublisher(char* remote_ip, int remote_port, int port,
                                     std::string stock_code, int date,
                                     std::string data_type, int speed)
    : IPublisher<DataType>(stock_code, date, data_type, speed) {}

template <typename DataType>
UDPPublisher<DataType>::~UDPPublisher() {}

template <typename DataType>
void UDPPublisher<DataType>::EstablishConnection(char* remote_ip,
                                                 int remote_port, int port) {
  m_connection = new UDPConnection(remote_ip, remote_port, port);
}

template <typename DataType>
std::string UDPPublisher<DataType>::GetSendData() {
  std::string ans;
  if constexpr (std::is_same<DataType, XyMarketData>()) {
    ans += std::string(IPublisher<DataType>::m_data->wind_code);
    ans += "\n";
    ans += std::string(IPublisher<DataType>::m_data->code);
    ans += "\n";
    ans += std::to_string(IPublisher<DataType>::m_data->action_day);
    ans += "\n";
    ans += std::to_string(IPublisher<DataType>::m_data->trading_day);
    ans += "\n";
    ans += std::to_string(IPublisher<DataType>::m_data->update_time);
    ans += "\n";
    ans += std::to_string(IPublisher<DataType>::m_data->status);
    ans += "\n";
    ans += std::to_string(IPublisher<DataType>::m_data->pre_close);
    ans += "\n";
    ans += std::to_string(IPublisher<DataType>::m_data->open);
    ans += "\n";
    ans += std::to_string(IPublisher<DataType>::m_data->high);
    ans += "\n";
    ans += std::to_string(IPublisher<DataType>::m_data->low);
    ans += "\n";
    ans += std::to_string(IPublisher<DataType>::m_data->last_price);
    ans += "\n";
    GetVectorReturn(ans, IPublisher<DataType>::m_data->ask_price, 5);
    // ans += "\n";
    GetVectorReturn(ans, IPublisher<DataType>::m_data->ask_vol, 5);
    // ans += "\n";
    GetVectorReturn(ans, IPublisher<DataType>::m_data->bid_price, 5);
    // ans += "\n";
    GetVectorReturn(ans, IPublisher<DataType>::m_data->bid_volume, 5);
    // ans += "\n";
    ans += std::to_string(IPublisher<DataType>::m_data->num_trades);
    ans += "\n";
    ans += std::to_string(IPublisher<DataType>::m_data->volume);
    ans += "\n";
    ans += std::to_string(IPublisher<DataType>::m_data->turnover);
    ans += "\n";
    ans += std::to_string(IPublisher<DataType>::m_data->total_bid_vol);
    ans += "\n";
    ans += std::to_string(IPublisher<DataType>::m_data->total_ask_vol);
    ans += "\n";
    ans += std::to_string(IPublisher<DataType>::m_data->weighted_avg_bid_price);
    ans += "\n";
    ans += std::to_string(IPublisher<DataType>::m_data->weighted_avg_ask_price);
    ans += "\n";
    ans += std::to_string(IPublisher<DataType>::m_data->iopv);
    ans += "\n";
    ans += std::to_string(IPublisher<DataType>::m_data->yield_to_maturity);
    ans += "\n";
    ans += std::to_string(IPublisher<DataType>::m_data->high_limited);
    ans += "\n";
    ans += std::to_string(IPublisher<DataType>::m_data->low_limited);
    ans += "\n";
    ans += std::string(IPublisher<DataType>::m_data->prefix);
    ans += "\n";
    ans += std::to_string(IPublisher<DataType>::m_data->syl1);
    ans += "\n";
    ans += std::to_string(IPublisher<DataType>::m_data->syl2);
    ans += "\n";
    ans += std::to_string(IPublisher<DataType>::m_data->sd2);
    ans += "\n";
  }

  return ans;
}

template <typename DataType>
void UDPPublisher<DataType>::SendData(char* buffer) {
  m_connection->SendData(buffer);
}

template <typename DataType>
void UDPPublisher<DataType>::GetVectorReturn(std::string& ans, uint32_t* arr,
                                             int times) {
  for (int k = 0; k < times; k++) {
    ans += std::to_string(arr[k]) + "\n";
  }
}

template class UDPPublisher<XyMarketData>;

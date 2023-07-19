#include "udp_publisher.h"

template <typename DataType>
TCPPublisher<DataType>::TCPPublisher(char* remote_ip, int remote_port, int port,
                                     std::string stock_code, int date,
                                     std::string data_type, int speed)
    : IPublisher<DataType>(remote_ip, remote_port, port, stock_code, date,
                           data_type, speed) {}

template <typename DataType>
TCPPublisher<DataType>::~TCPPublisher() {}

template <typename DataType>
void TCPPublisher<DataType>::EstablishConnection(char* remote_ip,
                                                 int remote_port, int port) {
  m_connection = new TCPConnection(remote_ip, remote_port, port);
  m_connection->establishConnection(true);
}

template <typename DataType>
std::string TCPPublisher<DataType>::GetSendData() {
  std::string ans;
  if constexpr (std::is_same<DataType, XyMarketData>()) {
    ans +=
        "wind_code is: " + std::string(IPublisher<DataType>::m_data->wind_code);
    ans += "\n";
    ans += "code is : " + std::string(IPublisher<DataType>::m_data->code);
    ans += "\n";
    ans += "action_day is: " +
           std::to_string(IPublisher<DataType>::m_data->action_day);
    ans += "\n";
    ans += "trading_day is: " +
           std::to_string(IPublisher<DataType>::m_data->trading_day);
    ans += "\n";
    ans += "update_time is: " +
           std::to_string(IPublisher<DataType>::m_data->update_time);
    ans += "\n";
    ans += "status is: " + std::to_string(IPublisher<DataType>::m_data->status);
    ans += "\n";
    ans += "pre_close is: " +
           std::to_string(IPublisher<DataType>::m_data->pre_close);
    ans += "\n";
    ans += "open is: " + std::to_string(IPublisher<DataType>::m_data->open);
    ans += "\n";
    ans += "high is: " + std::to_string(IPublisher<DataType>::m_data->high);
    ans += "\n";
    ans += "low is: " + std::to_string(IPublisher<DataType>::m_data->low);
    ans += "\n";
    ans += "last_price is: " +
           std::to_string(IPublisher<DataType>::m_data->last_price);
    ans += "\n";
    GetVectorReturn(ans, IPublisher<DataType>::m_data->ask_price, 5);
    ans += "\n";
    GetVectorReturn(ans, IPublisher<DataType>::m_data->ask_vol, 5);
    ans += "\n";
    GetVectorReturn(ans, IPublisher<DataType>::m_data->bid_price, 5);
    ans += "\n";
    GetVectorReturn(ans, IPublisher<DataType>::m_data->bid_volume, 5);
    ans += "\n";
    ans += "num_trades is: " +
           std::to_string(IPublisher<DataType>::m_data->num_trades);
    ans += "\n";
    ans += "volume is: " + std::to_string(IPublisher<DataType>::m_data->volume);
    ans += "\n";
    ans += "turnover is: " +
           std::to_string(IPublisher<DataType>::m_data->turnover);
    ans += "\n";
    ans += "total_bid_vol is: " +
           std::to_string(IPublisher<DataType>::m_data->total_bid_vol);
    ans += "\n";
    ans += "total_ask_vol is: " +
           std::to_string(IPublisher<DataType>::m_data->total_ask_vol);
    ans += "\n";
    ans += "weighted_avg_bid_price is: " +
           std::to_string(IPublisher<DataType>::m_data->weighted_avg_bid_price);
    ans += "\n";
    ans += "weighted_avg_ask_price is: " +
           std::to_string(IPublisher<DataType>::m_data->weighted_avg_ask_price);
    ans += "\n";
    ans += "iopv is: " + std::to_string(IPublisher<DataType>::m_data->iopv);
    ans += "\n";
    ans += "yield_to_maturity is: " +
           std::to_string(IPublisher<DataType>::m_data->yield_to_maturity);
    ans += "\n";
    ans += "high_limited is: " +
           std::to_string(IPublisher<DataType>::m_data->high_limited);
    ans += "\n";
    ans += "low_limited is: " +
           std::to_string(IPublisher<DataType>::m_data->low_limited);
    ans += "\n";
    ans += "prefix is: " + std::string(IPublisher<DataType>::m_data->prefix);
    ans += "\n";
    ans += "syl1 is: " + std::to_string(IPublisher<DataType>::m_data->syl1);
    ans += "\n";
    ans += "syl2 is: " + std::to_string(IPublisher<DataType>::m_data->syl2);
    ans += "\n";
    ans += "sd2 is: " + std::to_string(IPublisher<DataType>::m_data->sd2);
    ans += "\n";
  }

  return ans;
}

template <typename DataType>
void TCPPublisher<DataType>::SendData(char* buffer) {
  m_connection->sendData(buffer);
}

template <typename DataType>
void TCPPublisher<DataType>::GetVectorReturn(std::string& ans, uint32_t* arr,
                                             int times) {
  for (int i = 0; i < times; i++) {
    ans += std::to_string(arr[i]) + "\n";
  }
}

template class TCPPublisher<XyMarketData>;

#include "tcp_publisher.h"

#define RED "\033[31m"   /* Red */
#define WHITE "\033[37m" /* White */

template <typename DataType>
TCPPublisher<DataType>::TCPPublisher(std::string stock_code, int date,
                                     std::string data_type, int speed)
    : IPublisher<DataType>(stock_code, date, data_type, speed) {}

template <typename DataType>
TCPPublisher<DataType>::TCPPublisher(TCPConnection* tcp_connection,
                                     int send_index, std::string stock_code,
                                     int date, std::string data_type, int speed)
    : IPublisher<DataType>(stock_code, date, data_type, speed),
      m_send_index(send_index) {
  m_connection = tcp_connection;
}

template <typename DataType>
TCPPublisher<DataType>::~TCPPublisher() {}

template <typename DataType>
void TCPPublisher<DataType>::EstablishConnection(char* remote_ip,
                                                 int remote_port, int port) {
  m_connection = new TCPConnection(remote_ip, remote_port, port);
  m_connection->EstablishConnection(true);
}

template <typename DataType>
std::string TCPPublisher<DataType>::GetSendData() {
  std::string ans;
  GetRowDiffs();
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
    if (m_diffs[0] != 0) {
      ans += RED;
    }
    ans += "open is: " + std::to_string(IPublisher<DataType>::m_data->open) +
           +"(" + std::to_string(m_diffs[0]) + ")" + WHITE;
    ans += "\n";
    if (m_diffs[1] != 0) {
      ans += RED;
    }
    ans += "high is: " + std::to_string(IPublisher<DataType>::m_data->high) +
           "(" + std::to_string(m_diffs[1]) + ")" + WHITE;
    ans += "\n";
    if (m_diffs[2] != 0) {
      ans += RED;
    }
    ans += "low is: " + std::to_string(IPublisher<DataType>::m_data->low) +
           "(" + std::to_string(m_diffs[2]) + ")" + WHITE;
    ans += "\n";
    if (m_diffs[3] != 0) {
      ans += RED;
    }
    ans += "last_price is: " +
           std::to_string(IPublisher<DataType>::m_data->last_price) + "(" +
           std::to_string(m_diffs[3]) + ")" + WHITE;
    ans += "\n";

    if (m_diffs[4] != 0) {
      ans += RED;
    }
    GetVectorReturn("ask_price", ans, IPublisher<DataType>::m_data->ask_price,
                    5);
    ans += WHITE;

    if (m_diffs[5] != 0) {
      ans += RED;
    }
    GetVectorReturn("ask_vol", ans, IPublisher<DataType>::m_data->ask_vol, 5);
    ans += WHITE;

    if (m_diffs[6] != 0) {
      ans += RED;
    }
    GetVectorReturn("bid_price", ans, IPublisher<DataType>::m_data->bid_price,
                    5);
    ans += WHITE;
    if (m_diffs[7] != 0) {
      ans += RED;
    }
    GetVectorReturn("bid_volume", ans, IPublisher<DataType>::m_data->bid_volume,
                    5);
    ans += WHITE;
    if (m_diffs[8] != 0) {
      ans += RED;
    }
    ans += "num_trades is: " +
           std::to_string(IPublisher<DataType>::m_data->num_trades) + "(" +
           std::to_string(m_diffs[8]) + ")" + WHITE;
    ans += "\n";
    if (m_diffs[9] != 0) {
      ans += RED;
    }
    ans +=
        "volume is: " + std::to_string(IPublisher<DataType>::m_data->volume) +
        "(" + std::to_string(m_diffs[9]) + ")" + WHITE;
    ans += "\n";

    if (m_diffs[10] != 0) {
      ans += RED;
    }
    ans += "turnover is: " +
           std::to_string(IPublisher<DataType>::m_data->turnover) + "(" +
           std::to_string(m_diffs[10]) + ")" + WHITE;
    ans += "\n";
    if (m_diffs[11] != 0) {
      ans += RED;
    }
    ans += "total_bid_vol is: " +
           std::to_string(IPublisher<DataType>::m_data->total_bid_vol) + "(" +
           std::to_string(m_diffs[11]) + ")" + WHITE;
    ans += "\n";

    if (m_diffs[12] != 0) {
      ans += RED;
    }
    ans += "total_ask_vol is: " +
           std::to_string(IPublisher<DataType>::m_data->total_ask_vol) + "(" +
           std::to_string(m_diffs[12]) + ")" + WHITE;
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
  m_connection->SendData(buffer, m_send_index);
}

template <typename DataType>
void TCPPublisher<DataType>::GetVectorReturn(const char* prefix,
                                             std::string& ans, uint32_t* arr,
                                             int times) {
  std::string sprefix(prefix);
  for (int k = 0; k < times; k++) {
    ans += sprefix + char('1' + k) + " is: " + std::to_string(arr[k]) + "\n";
  }
}

template <typename DataType>
void TCPPublisher<DataType>::GetRowDiffs() {
  m_diffs.clear();
  if constexpr (std::is_same<DataType, XyMarketData>()) {
    m_diffs.push_back(IPublisher<DataType>::m_data->open -
                      IPublisher<DataType>::m_pre_data->open);
    m_diffs.push_back(IPublisher<DataType>::m_data->high -
                      IPublisher<DataType>::m_pre_data->high);
    m_diffs.push_back(IPublisher<DataType>::m_data->low -
                      IPublisher<DataType>::m_pre_data->low);
    m_diffs.push_back(IPublisher<DataType>::m_data->last_price -
                      IPublisher<DataType>::m_pre_data->last_price);
    m_diffs.push_back(IPublisher<DataType>::m_data->ask_price[0] -
                      IPublisher<DataType>::m_pre_data->ask_price[0]);
    m_diffs.push_back(IPublisher<DataType>::m_data->ask_vol[0] -
                      IPublisher<DataType>::m_pre_data->ask_vol[0]);
    m_diffs.push_back(IPublisher<DataType>::m_data->bid_price[0] -
                      IPublisher<DataType>::m_pre_data->bid_price[0]);
    m_diffs.push_back(IPublisher<DataType>::m_data->bid_volume[0] -
                      IPublisher<DataType>::m_pre_data->bid_volume[0]);
    m_diffs.push_back(IPublisher<DataType>::m_data->num_trades -
                      IPublisher<DataType>::m_pre_data->num_trades);
    m_diffs.push_back(IPublisher<DataType>::m_data->volume -
                      IPublisher<DataType>::m_pre_data->volume);
    m_diffs.push_back(IPublisher<DataType>::m_data->turnover -
                      IPublisher<DataType>::m_pre_data->turnover);
    m_diffs.push_back(IPublisher<DataType>::m_data->total_bid_vol -
                      IPublisher<DataType>::m_pre_data->total_bid_vol);
    m_diffs.push_back(IPublisher<DataType>::m_data->total_ask_vol -
                      IPublisher<DataType>::m_pre_data->total_ask_vol);
  }
}

template class TCPPublisher<XyMarketData>;

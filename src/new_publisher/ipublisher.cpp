#include "udp_publisher.h"

template <typename DataType>
IPublisher<DataType>::IPublisher(std::string stock_code, int date,
                                 std::string data_type, int speed)
    : m_speed(speed) {
  // this->m_connection = new UDPConnection(remote_ip, remote_port, port);
  this->m_data_source = new DataSource(stock_code, date, data_type);
  m_time = 80000000;
}

template <typename DataType>
IPublisher<DataType>::~IPublisher() {}

template <typename DataType>
void IPublisher<DataType>::Start() {
  while (m_time <= 180000000) {
    if (m_data_source->HasNewData(m_time)) {
      unsigned char *raw_data = m_data_source->getData(m_time);  // delete

      m_data = reinterpret_cast<DataType *>(raw_data);

      strcpy(m_send_buffer, GetSendData().c_str());

      SendData(m_send_buffer);
    }

    m_pre_data = m_data;

    // AddTime(m_speed);

    if (m_demo) {
      AddTime(m_speed * 1000);
      std::this_thread::sleep_for(std::chrono::milliseconds(m_speed));
    } else {
      AddTime(m_speed);
      std::this_thread::sleep_for(std::chrono::milliseconds(m_speed));
    }
  }

  strcpy(m_send_buffer, "publish over");
  SendData(m_send_buffer);
}

// template <typename DataType>
// void IPublisher<DataType>::ConvertDataType(unsigned char *raw_data) {
//   m_data = reinterpret_cast<DataType *>(raw_data);
// }

template <typename DataType>
void IPublisher<DataType>::AddTime(int time) {
  int h1 = time / 10000000;
  time = time % 10000000;
  int min1 = time / 100000;
  time = time % 100000;
  int s1 = time / 1000;
  int ms1 = time % 1000;

  int h2 = m_time / 10000000;
  m_time = m_time % 10000000;
  int min2 = m_time / 100000;
  m_time = m_time % 100000;
  int s2 = m_time / 1000;
  int ms2 = m_time % 1000;

  ms1 += ms2;
  s1 += s2;
  min1 += min2;
  h1 += h2;

  s1 = s1 + (ms1 / 1000);
  ms1 = ms1 % 1000;

  min1 = min1 + (s1 / 60);
  s1 = s1 % 60;

  h1 = h1 + (min1 / 60);
  min1 = min1 % 60;

  m_time = h1 * 10000000 + min1 * 100000 + s1 * 1000 + ms1;
}

template class IPublisher<XyMarketData>;

#include "udp_publisher.h"

template <typename DataType>
IPublisher<DataType>::IPublisher(std::string stock_code, int date,
                                 std::string data_type, int speed)
    : m_speed(speed) {
  // 数据初始化
  this->m_data_source = new DataSource(stock_code, date, data_type);
  m_pre_data = reinterpret_cast<DataType *>(m_data_source->GetDataByIndex(0));

  // 当前时刻从8：00：00开始
  m_time = 80000000;
}

template <typename DataType>
IPublisher<DataType>::~IPublisher() {}

template <typename DataType>
void IPublisher<DataType>::Start() {
  // 如果当前时刻小于18:00:00，查询数据是否存在更新
  while (m_time <= 180000000) {
    if (m_data_source->HasNewData(m_time)) {
      unsigned char *raw_data = m_data_source->GetData(m_time);  // delete

      // 将数据转为所查询的类型
      m_data = reinterpret_cast<DataType *>(raw_data);

      // 获得要发送的内容，暂存至buffer
      strcpy(m_send_buffer, GetSendData().c_str());

      // 更新先前数据
      m_pre_data = m_data;

      SendData(m_send_buffer);
    }

    // 如果是demo模式，认为系统时间更新为默认时间的1000倍
    if (m_demo) {
      AddTime(m_speed * 1000);
      std::this_thread::sleep_for(std::chrono::milliseconds(m_speed));
    } else {
      AddTime(m_speed);
      std::this_thread::sleep_for(std::chrono::milliseconds(m_speed));
    }
  }

  // 超过18:00:00，发布结束
  strcpy(m_send_buffer, "\033[34mpublish over\033[37m");
  SendData(m_send_buffer);
}

// 增加当前时刻
template <typename DataType>
void IPublisher<DataType>::AddTime(int time) {
  // 获得新增的小时，分钟，秒，毫秒
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

  // 进位处理
  s1 = s1 + (ms1 / 1000);
  ms1 = ms1 % 1000;

  min1 = min1 + (s1 / 60);
  s1 = s1 % 60;

  h1 = h1 + (min1 / 60);
  min1 = min1 % 60;

  // 整理得到新时刻
  m_time = h1 * 10000000 + min1 * 100000 + s1 * 1000 + ms1;
}

template class IPublisher<XyMarketData>;

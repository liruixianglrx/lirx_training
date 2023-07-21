#ifndef LIRX_TRAINING_PUBLISHER_TCP_PUBLISHER_H
#define LIRX_TRAINING_PUBLISHER_TCP_PUBLISHER_H

#include <unistd.h>

#include "../connection/tcp_connection.h"
#include "ipublisher.h"

template <typename DataType>
class TCPPublisher : public IPublisher<DataType> {
 public:
  // 以股票代码,日期,数据类型,更新速度为参数进行初始化
  TCPPublisher(std::string stock_code, int date, std::string data_type,
               int speed);

  // 类似上一个构造函数，复用已经构建好的TCP-connection
  TCPPublisher(TCPConnection* tcp_connection, int send_index,
               std::string stock_code, int date, std::string data_type,
               int speed);
  ~TCPPublisher();

  TCPConnection* GetConnection() { return m_connection; }

  // 建立tcp连接
  virtual void EstablishConnection(char* remote_ip, int remote_port, int port);

 private:
  TCPConnection* m_connection;

  // 发送要使用的连接序号
  int m_send_index;

  // 新旧时刻数据的各个字段的差值
  std::vector<int> m_diffs;

  // 获得各个字段的差值
  void GetRowDiffs();

  // 获得vector类型字段的差值，e.g. ask_price[10]
  void GetVectorReturn(const char* prefix, std::string& ans, uint32_t* arr,
                       int times);

  // 将要发送的内容进行正确的格式转换
  virtual std::string GetSendData();

  // 发送数据
  virtual void SendData(char* buffer);
};

#endif  // LIRX_TRAINING_PUBLISHER_TCP_PUBLISHER_H
#ifndef LIRX_TRAINING_PUBLISHER_IPUBLISHER_H
#define LIRX_TRAINING_PUBLISHER_IPUBLISHER_H

#include <unistd.h>

#include <thread>

#include "../data_source/data_source.h"

template <typename DataType>
class IPublisher {
 public:
  IPublisher(std::string stock_code, int date, std::string data_type,
             int speed);
  ~IPublisher();

  // 将publisher设为demo模式，方便成果展示
  // 正式使用可以去掉这个字段
  void SetDemo(bool demo) { m_demo = demo; }

  // 正式开始发布行情数据
  void Start();

  // 根据传输方式的不同，建立连接
  virtual void EstablishConnection(char* remote_ip, int remote_port, int port) {
  }

 protected:
  // m_pre_data,上一个时刻的数据
  // m_data,当前时刻的数据
  DataType *m_pre_data, *m_data;

 private:
  // 将要发送的内容进行正确的格式转换
  virtual std::string GetSendData() { return nullptr; }

  // 发送数据
  virtual void SendData(char* buffer) {}

  // 增加当前时刻
  void AddTime(int time);

  // 数据更新速度
  int m_speed;
  // 当前时刻
  int m_time;
  DataSource* m_data_source;
  char m_send_buffer[1024];

  // 是否开启demo模式
  bool m_demo;
};

#endif  // LIRX_TRAINING_PUBLISHER_IPUBLISHER_H
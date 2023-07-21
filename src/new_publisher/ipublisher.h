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

  void SetDemo(bool demo) { m_demo = demo; }
  void Start();
  // DataType *m_pre_data, *m_data;
  // void ConvertDataType(unsigned char* raw_data);
  virtual void EstablishConnection(char* remote_ip, int remote_port, int port) {
  }

  int GetEntryNum() { return m_data_source->m_entry_num; };

 protected:
  DataType *m_pre_data, *m_data;

 private:
  // UDPConnection* m_connection;

  virtual std::string GetSendData() { return nullptr; }

  virtual void SendData(char* buffer) {}

  void AddTime(int time);

  int m_speed;
  int m_time;
  DataSource* m_data_source;
  char m_send_buffer[1024];

  bool m_demo;
};

#endif  // LIRX_TRAINING_PUBLISHER_IPUBLISHER_H
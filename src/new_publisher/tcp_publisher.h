#ifndef LIRX_TRAINING_PUBLISHER_TCP_PUBLISHER_H
#define LIRX_TRAINING_PUBLISHER_TCP_PUBLISHER_H

#include <unistd.h>

#include "../connection/tcp_connection.h"
#include "ipublisher.h"

template <typename DataType>
class TCPPublisher : public IPublisher<DataType> {
 public:
  TCPPublisher(char* remote_ip, int remote_port, int port,
               std::string stock_code, int date, std::string data_type,
               int speed);
  ~TCPPublisher();

  virtual void EstablishConnection(char* remote_ip, int remote_port, int port);

 private:
  TCPConnection* m_connection;
  // std::vector<int> m_highlight_rows;

  void GetVectorReturn(std::string& ans, uint32_t* arr, int times);

  virtual std::string GetSendData();

  virtual void SendData(char* buffer);
};

#endif  // LIRX_TRAINING_PUBLISHER_TCP_PUBLISHER_H
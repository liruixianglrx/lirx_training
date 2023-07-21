#ifndef LIRX_TRAINING_PUBLISHER_UDP_PUBLISHER_H
#define LIRX_TRAINING_PUBLISHER_UDP_PUBLISHER_H

#include <unistd.h>

#include "../connection/udp_connection.h"
#include "ipublisher.h"

template <typename DataType>
class UDPPublisher : public IPublisher<DataType> {
 public:
  UDPPublisher(char* remote_ip, int remote_port, int port,
               std::string stock_code, int date, std::string data_type,
               int speed);
  ~UDPPublisher();

  virtual void EstablishConnection(char* remote_ip, int remote_port, int port);

 private:
  UDPConnection* m_connection;

  void GetVectorReturn(std::string& ans, uint32_t* arr, int times);

  virtual std::string GetSendData();

  virtual void SendData(char* buffer);
};

#endif  // LIRX_TRAINING_PUBLISHER_UDP_PUBLISHER_H
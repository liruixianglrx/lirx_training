#ifndef LIRX_TRAINING_SERVER_ISERVER_H
#define LIRX_TRAINING_SERVER_ISERVER_H

#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <string>
#include <vector>

#include "../XyMarketData.h"
#include "../data_source/data_source.h"
#include "../publisher/ipublisher.h"
#include "../publisher/tcp_publisher.h"
#include "../publisher/udp_publisher.h"

class IServer {
 public:
  IServer(const std::string &mode, const std::string &data_type,
          const int speed, int source_size);
  virtual ~IServer();
  // void AddNewSource();

  // template <typename DataType>
  // void AddNewPublisher(IPublisher<DataType> *);
  void start();

 private:
  // template <typename DataType>
  // virtual IPublisher<DataType> *GetNewPublisher();

  int m_listen_fd;
  // speed for data broadcast
  int m_speed;
  std::string m_data_type, m_mode;
  int m_source_size;
  struct sockaddr_in m_servaddr;
  int m_server_port;
  std::string m_stock_code, m_date;  // no need

  static void worker(struct sockaddr_in client_addr,
                     const std::string &stock_code, const std::string &date,
                     const std::string &data_type, const std::string &mode);
};

#endif  // LIRX_TRAINING_SERVER_ISERVER_H
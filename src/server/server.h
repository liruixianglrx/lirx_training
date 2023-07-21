#ifndef LIRX_TRAINING_SERVER_SERVER_H
#define LIRX_TRAINING_SERVER_SERVER_H

#include <netinet/in.h>

#include "../connection/tcp_connection.h"
#include "../connection/udp_connection.h"
#include "../data_source/data_source.h"
#include "../publisher/ipublisher.h"
#include "../publisher/tcp_publisher.h"
#include "../publisher/udp_publisher.h"

class Server {
 public:
  Server(const std::string &mode, const int speed, int source_size);
  virtual ~Server();

  void Start();

  void SetDemo(bool demo) { m_demo = demo; }

 private:
  TCPConnection m_handshake;
  int m_speed;
  std::string m_mode;
  int m_source_size;

  int m_server_port;

  static bool m_demo;

  static void Worker(char *client_ip, int client_port,
                     const std::string &stock_code, const int &date,
                     const std::string &data_type, const std::string &mode,
                     int speed, TCPConnection *m_handshake, int send_index);
};

#endif  // LIRX_TRAINING_SERVER_SERVER_H
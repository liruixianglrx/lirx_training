#ifndef LIRX_TRAINING_CONNECTION_TCPCONNECTION_H
#define LIRX_TRAINING_CONNECTION_TCPCONNECTION_H

#include <arpa/inet.h>
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

#define BUFFER_SIZE 1024
class TCPConnection {
 public:
  TCPConnection() {}
  TCPConnection(char* remote_ip, int remote_port, int port) {
    setConnect(remote_ip, remote_port, port);
  }
  ~TCPConnection() { closeConnection(); }

  void setConnect(char* remote_ip, int remote_port, int port);

  int getSocket() { return m_socked; }

  int getLocalPort();

  void getRemoteIp(char* client_ip);

  int establishConnection(bool is_server);

  void sendData(const char* buf, int index);

  int receiveData(char* buf, int index);

  void closeConnection() {
    close(m_socked);
    for (auto& socket : m_real_socked) {
      close(socket);
    }
  }

 private:
  int m_socked;
  std::vector<int> m_real_socked;
  struct sockaddr_in m_local_addr, m_remote_addr;
};
#endif  // LIRX_TRAINING_CONNECTION_TCPCONNECTION_H
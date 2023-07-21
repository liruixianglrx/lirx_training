#ifndef LIRX_TRAINING_CONNECTION_UDPCONNECTION_H
#define LIRX_TRAINING_CONNECTION_UDPCONNECTION_H

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

#define BUFFER_SIZE 1024
class UDPConnection {
 public:
  UDPConnection() {}

  UDPConnection(char* remote_ip, int remote_port, int port) {
    SetConnect(remote_ip, remote_port, port);
  }

  ~UDPConnection() { closeConnection(); }

  void SetConnect(char* remote_ip, int remote_port, int port);

  int getLocalPort();

  void joinGroup(char* group);

  int sendData(char* buf);

  int receiveData(char* buf);

  void closeConnection() { close(m_socked); }

 private:
  int m_socked;
  struct sockaddr_in m_local_addr, m_remote_addr;
};
#endif  // LIRX_TRAINING_CONNECTION_UDPCONNECTION_H
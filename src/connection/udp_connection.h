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

  // RAII自动申请和释放connection
  // 如果port置0则系统自动分配port
  UDPConnection(char* remote_ip, int remote_port, int port) {
    SetConnect(remote_ip, remote_port, port);
  }

  ~UDPConnection() { CloseConnection(); }

  // 设置连接
  void SetConnect(char* remote_ip, int remote_port, int port);

  // 返回本地的使用port
  int GetLocalPort();

  // 加入广播组
  void JoinGroup(char* group);

  int SendData(char* buf);

  int ReceiveData(char* buf);

  void CloseConnection() { close(m_socked); }

 private:
  int m_socked;
  struct sockaddr_in m_local_addr, m_remote_addr;
};
#endif  // LIRX_TRAINING_CONNECTION_UDPCONNECTION_H
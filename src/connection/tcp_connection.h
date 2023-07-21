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

  // RAII自动申请和释放connection
  // 如果port置0则系统自动分配port
  TCPConnection(char* remote_ip, int remote_port, int port) {
    SetConnect(remote_ip, remote_port, port);
  }

  ~TCPConnection() { CloseConnection(); }

  // 设置连接
  void SetConnect(char* remote_ip, int remote_port, int port);

  int GetSocket() { return m_socked; }

  // 返回本地的使用port
  int GetLocalPort();

  // 得到连接的远程IP
  void GetRemoteIp(char* client_ip);

  int EstablishConnection(bool is_server);

  void SendData(const char* buf, int index);

  int ReceiveData(char* buf, int index);

  void CloseConnection() {
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
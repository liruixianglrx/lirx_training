#include "tcp_connection.h"

void TCPConnection::SetConnect(char *remote_ip, int remote_port, int port) {
  m_socked = socket(AF_INET, SOCK_STREAM, 0);
  if (m_socked < 0) {
    perror("socket failed!");
  }

  // 设置本地的IP与端口
  memset(&m_local_addr, 0, sizeof(m_local_addr));
  memset(&m_remote_addr, 0, sizeof(m_remote_addr));
  m_local_addr.sin_family = AF_INET;
  m_local_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  m_local_addr.sin_port = htons(port);

  // 设置远程的IP与端口
  m_remote_addr.sin_family = AF_INET;
  m_remote_addr.sin_addr.s_addr = inet_addr(remote_ip);
  m_remote_addr.sin_port = htons(remote_port);

  // bind
  int ret =
      bind(m_socked, (struct sockaddr *)&m_local_addr, sizeof(m_local_addr));
  if (ret < 0) {
    perror("bind failed !");
  }
}

int TCPConnection::GetLocalPort() {
  struct sockaddr_in m_local_addr;
  socklen_t local_addr_len = sizeof(m_local_addr);
  getsockname(m_socked, (struct sockaddr *)&m_local_addr, &local_addr_len);
  return ntohs(m_local_addr.sin_port);
}

void TCPConnection::GetRemoteIp(char *client_ip) {
  inet_ntop(AF_INET, &m_remote_addr.sin_addr, client_ip, sizeof(client_ip));
}

int TCPConnection::EstablishConnection(bool is_server) {
  if (is_server) {
    // 如果是server的socket，监听端口
    if (listen(m_socked, 5) == -1) {
      perror("listen");
      return -1;
    }

    int client_fd;
    if ((client_fd = accept(m_socked, NULL, NULL)) == -1) {
      perror("accept");
      return -1;
    }

    // 将accept的新连接加入连接数组中
    m_real_socked.push_back(client_fd);
    return m_real_socked.size() - 1;
  } else {
    // 如果是client端，尝试连接server端
    if (connect(m_socked, (struct sockaddr *)&m_remote_addr,
                sizeof(m_remote_addr)) == -1) {
      perror("connect");
      return -1;
    }

    // 将连接加入连接数组中
    m_real_socked.push_back(m_socked);
    return 0;
  }
}

void TCPConnection::SendData(const char *buf, int index) {
  // 按照连接数组中的index进行数据传输
  printf("sending %s\n", buf);
  if (send(m_real_socked[index], buf, BUFFER_SIZE, 0) == -1) {
    perror("send");
  }
}

int TCPConnection::ReceiveData(char *buf, int index) {
  // 按照连接数组中的index进行数据接收
  int length = recv(m_real_socked[index], buf, BUFFER_SIZE, 0);

  buf[length] = '\0';

  printf("%s\n", buf);

  return length;
}
#include "tcp_connection.h"

void TCPConnection::setConnect(char *remote_ip, int remote_port, int port) {
  m_socked = socket(AF_INET, SOCK_STREAM, 0);
  if (m_socked < 0) {
    perror("socket failed!");
  }
  // struct sockaddr_in local_addr;
  memset(&m_local_addr, 0, sizeof(m_local_addr));
  memset(&m_remote_addr, 0, sizeof(m_remote_addr));
  m_local_addr.sin_family = AF_INET;
  m_local_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  m_local_addr.sin_port = htons(port);

  m_remote_addr.sin_family = AF_INET;
  m_remote_addr.sin_addr.s_addr = inet_addr(remote_ip);
  m_remote_addr.sin_port = htons(remote_port);

  int ret =
      bind(m_socked, (struct sockaddr *)&m_local_addr, sizeof(m_local_addr));
  if (ret < 0) {
    perror("bind failed !");
  }
}

int TCPConnection::getLocalPort() {
  struct sockaddr_in m_local_addr;
  socklen_t local_addr_len = sizeof(m_local_addr);
  getsockname(m_socked, (struct sockaddr *)&m_local_addr, &local_addr_len);
  return ntohs(m_local_addr.sin_port);
}

void TCPConnection::getRemoteIp(char *client_ip) {
  inet_ntop(AF_INET, &m_remote_addr.sin_addr, client_ip, sizeof(client_ip));
}

int TCPConnection::establishConnection(bool is_server) {
  if (is_server) {
    if (listen(m_socked, 5) == -1) {
      perror("listen");
      return -1;
    }

    int client_fd;
    if ((client_fd = accept(m_socked, NULL, NULL)) == -1) {
      perror("accept");
      return -1;
    }

    m_real_socked.push_back(client_fd);
    return m_real_socked.size() - 1;
  } else {
    if (connect(m_socked, (struct sockaddr *)&m_remote_addr,
                sizeof(m_remote_addr)) == -1) {
      perror("connect");
      return -1;
    }

    m_real_socked.push_back(m_socked);
    return 0;
  }
}

void TCPConnection::sendData(const char *buf, int index) {
  printf("sending %s\n", buf);
  if (send(m_real_socked[index], buf, BUFFER_SIZE, 0) == -1) {
    perror("send");
  }
}

int TCPConnection::receiveData(char *buf, int index) {
  int length = recv(m_real_socked[index], buf, BUFFER_SIZE, 0);

  buf[length] = '\0';

  printf("%s\n", buf);

  return length;
}
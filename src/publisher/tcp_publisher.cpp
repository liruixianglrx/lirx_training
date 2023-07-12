#include "tcp_publisher.h"

#include <cstring>

#include "../XyMarketData.h"
template <typename DataType>
TCPPublisher<DataType>::TCPPublisher(const char *group_addr, int port) {
  memset(&m_remote_addr, 0, sizeof(m_remote_addr));
  m_remote_addr.sin_family = AF_INET;
  m_remote_addr.sin_addr.s_addr = inet_addr(group_addr);
  m_remote_addr.sin_port = htons(port);

  m_socket = socket(AF_INET, SOCK_STREAM, 0);
  if (m_socket < 0) {
    perror("socket failed!");
  }
}

template <typename DataType>
TCPPublisher<DataType>::~TCPPublisher() {
  close(m_socket);
}

template <typename DataType>
void TCPPublisher<DataType>::PublishOneRow() {
  char buf[1024];
  std::strcpy(buf, this->ParseOneRow().c_str());
  // int socket = IPublisher<DataType>::GetSocket();
  int length = sendto(this->m_socket, buf, strlen(buf), 0,
                      (struct sockaddr *)&(this->m_remote_addr),
                      sizeof(this->m_remote_addr));
  printf("Send Message%s\n", buf);
}

template class TCPPublisher<XyMarketData>;
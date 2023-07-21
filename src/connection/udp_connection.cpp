#include "udp_connection.h"

void UDPConnection::SetConnect(char *remote_ip, int remote_port, int port) {
  m_socked = socket(AF_INET, SOCK_DGRAM, 0);
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

int UDPConnection::GetLocalPort() {
  struct sockaddr_in m_local_addr;
  socklen_t local_addr_len = sizeof(m_local_addr);
  getsockname(m_socked, (struct sockaddr *)&m_local_addr, &local_addr_len);
  return ntohs(m_local_addr.sin_port);
}

void UDPConnection::JoinGroup(char *group) {
  // 加入UDP广播组
  struct ip_mreq mreq;
  mreq.imr_multiaddr.s_addr = inet_addr(group);
  mreq.imr_interface.s_addr = htonl(INADDR_ANY);
  int ret =
      setsockopt(m_socked, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq));
  if (ret < 0) {
    perror("setsockopt failed !");
    return;
  } else {
    printf("setsockopt success\n");
  }
}

int UDPConnection::SendData(char *buf) {
  // 进行数据传输
  int length = 0;
  length = sendto(m_socked, buf, strlen(buf), 0,
                  (struct sockaddr *)&m_remote_addr, sizeof(m_remote_addr));
  return length;
}

int UDPConnection::ReceiveData(char *buf) {
  // 进行数据接收
  int length = 0;

  socklen_t sender_len = sizeof(m_remote_addr);

  memset(buf, 0, sizeof(buf));
  length = recvfrom(m_socked, buf, BUFFER_SIZE, 0,
                    (struct sockaddr *)&m_remote_addr, &sender_len);
  buf[length] = '\0';

  printf("%s\n", buf);

  return length;
}
#include <cstring>
#include <string>

#include "../connection/tcp_connection.h"
#include "../connection/udp_connection.h"

int main(int argc, char* argv[]) {
  if (argc < 3) {
    printf("USAGE=%s [mode] [code] [date] [data_type]\n", argv[0]);
    exit(-1);
  }
  int local_connect_port;
  // int sockfd, local_connect_port;
  // struct sockaddr_in local_addr, servaddr;
  // socklen_t local_addr_len = sizeof(local_addr);

  // sockfd = socket(AF_INET, SOCK_STREAM, 0);

  if (!std::strcmp(argv[1], "easy")) {
    local_connect_port = 8080;
  } else
    local_connect_port = 8081;

  char remote_ip[16] = "127.0.0.1";
  TCPConnection handshake(remote_ip, local_connect_port, 0);

  handshake.establishConnection(false);
  // bzero(&servaddr, sizeof(servaddr));
  // servaddr.sin_family = AF_INET;
  // servaddr.sin_port = htons(local_connect_port);
  // servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");

  // if (connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) ==
  // -1) {
  //   close(sockfd);
  //   perror("connect");
  //   exit(-1);
  // }

  // char remote_ip[16] = "0";
  UDPConnection udp_connection(remote_ip, 0, 0);
  int udp_port = udp_connection.getLocalPort();
  char group[16] = "224.0.0.88";
  udp_connection.joinGroup(group);

  int tcp_port;
  std::string tmp_str;
  for (int i = 2; i < argc; i++) {
    tmp_str += argv[i];
    tmp_str.push_back(' ');
  }

  if (!std::strcmp(argv[0], "easy")) {
    tmp_str += std::to_string(tcp_port);
  } else
    tmp_str += std::to_string(udp_port);

  char buf[1024];
  strcpy(buf, tmp_str.c_str());

  handshake.sendData(buf);
  handshake.closeConnection();
  // if (send(sockfd, buf, strlen(buf), 0) == -1) {
  //   perror("send");
  //   exit(-1);
  // }

  while (true) {
    udp_connection.receiveData(buf);
  }

  exit(0);
}

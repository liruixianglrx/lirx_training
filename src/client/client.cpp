#include <cstring>
#include <string>

#include "../connection/tcp_connection.h"
#include "../connection/udp_connection.h"

int main(int argc, char* argv[]) {
  if (argc < 3) {
    printf("USAGE=%s [mode] [code] [date] [data_type]\n", argv[0]);
    exit(-1);
  }
  int remote_connect_port;

  if (!std::strcmp(argv[1], "easy")) {
    remote_connect_port = 8080;
  } else
    remote_connect_port = 8081;

  char remote_ip[16] = "127.0.0.1";
  TCPConnection handshake(remote_ip, remote_connect_port, 0);

  int index = handshake.EstablishConnection(false);
  UDPConnection udp_connection(remote_ip, 0, 0);
  int udp_port = udp_connection.GetLocalPort();
  char group[16] = "224.0.0.88";
  udp_connection.JoinGroup(group);

  int tcp_port;
  tcp_port = handshake.GetLocalPort();

  std::string tmp_str;
  for (int k = 2; k < argc; k++) {
    tmp_str += argv[k];
    tmp_str.push_back(' ');
  }

  if (!std::strcmp(argv[1], "easy")) {
    tmp_str += std::to_string(tcp_port);
  } else
    tmp_str += std::to_string(udp_port);

  char buf[1024];
  strcpy(buf, tmp_str.c_str());

  handshake.SendData(buf, index);

  while (true) {
    if (!std::strcmp(argv[1], "easy")) {
      handshake.ReceiveData(buf, index);
    } else {
      udp_connection.ReceiveData(buf);
    }

    if (!strcmp(buf, "\033[34mpublish over\033[37m")) {
      handshake.CloseConnection();
      break;
    }
  }

  exit(0);
}

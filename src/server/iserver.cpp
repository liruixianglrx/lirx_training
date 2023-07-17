#include "iserver.h"

#include <sstream>
#include <thread>
#define BUFFER_SIZE 1024

IServer::IServer(const std::string &data_type, const int speed, int source_size,
                 int server_port)
    : m_data_type(data_type), m_speed(speed), m_server_port(server_port) {}

IServer::~IServer() {}

void IServer::start() {
  char buf[BUFFER_SIZE];
  if ((m_listen_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
    perror("socket");
    exit(1);
  }

  bzero(&m_servaddr, sizeof(m_servaddr));
  m_servaddr.sin_family = AF_INET;
  m_servaddr.sin_port = htons(m_server_port);
  m_servaddr.sin_addr.s_addr = INADDR_ANY;

  int i = 1; /*允许重复使用本地址与套接字进行绑定*/
  setsockopt(m_listen_fd, SOL_SOCKET, SO_REUSEADDR, &i, sizeof(i));

  bind(m_listen_fd, (struct sockaddr *)&m_servaddr, sizeof(m_servaddr));

  int client_fd;
  std::string stock_code, date, data_type;
  struct sockaddr_in client_addr;
  socklen_t client_addr_len = sizeof(client_addr);

  while (1) {
    char sendbuf[] = "received data";  // no use
    listen(m_listen_fd, 5);
    // client_fd = accept(m_listen_fd, NULL, NULL);
    client_fd =
        accept(m_listen_fd, (struct sockaddr *)&client_addr, &client_addr_len);

    char client_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, sizeof(client_ip));

    memset(buf, 0, sizeof(buf));
    if (recv(client_fd, buf, BUFFER_SIZE, 0) == -1) {
      perror("recv");
      exit(0);
    } else {
      std::stringstream sstream(buf);
      sstream >> stock_code >> date >> data_type;
      // printf("%s\n", buf);
      send(client_fd, "lrx7777", 6, 0);
      std::thread new_thread(worker, client_fd, stock_code, date, data_type);

      new_thread.detach();
    }
  }
}

void IServer::worker(int client_fd, const std::string &stock_code,
                     const std::string &date, const std::string &data_type) {
  DataSource *new_data_source = new DataSource(data_type, 10);
  // send(client_fd, "lrx7777", 6, 0);
}
#include "server.h"

#include <sstream>
#include <thread>
#define BUFFER_SIZE 1024

Server::Server(const std::string &mode, const int speed, int source_size)
    : m_speed(speed), m_mode(mode) {
  if (mode == "easy")
    m_server_port = 8080;
  else
    m_server_port = 8081;
}

Server::~Server() {}

void Server::start() {
  char ip[16] = "0.0.0.0";
  m_handshake.setConnect(ip, 0, m_server_port);

  int i = 1; /*允许重复使用本地址与套接字进行绑定*/
  int socket = m_handshake.getSocket();  // no use
  setsockopt(m_handshake.getSocket(), SOL_SOCKET, SO_REUSEADDR, &i,
             sizeof(int));

  while (1) {
    m_handshake.establishConnection(true);

    char client_ip[INET_ADDRSTRLEN];
    m_handshake.getRemoteIp(client_ip);

    char buf[BUFFER_SIZE];
    m_handshake.receiveData(buf);

    std::stringstream sstream(buf);

    std::string stock_code, date, data_type, client_port;
    sstream >> stock_code >> date >> data_type >> client_port;

    std::thread new_thread(worker, client_ip, stoi(client_port), stock_code,
                           date, data_type, m_mode);

    new_thread.detach();
  }
  // char buf[BUFFER_SIZE];
  // if ((m_listen_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
  //   perror("socket");
  //   exit(1);
  // }

  // bzero(&m_servaddr, sizeof(m_servaddr));
  // m_servaddr.sin_family = AF_INET;
  // m_servaddr.sin_port = htons(m_server_port);
  // m_servaddr.sin_addr.s_addr = INADDR_ANY;

  // int i = 1; /*允许重复使用本地址与套接字进行绑定*/
  // setsockopt(m_listen_fd, SOL_SOCKET, SO_REUSEADDR, &i, sizeof(i));

  // bind(m_listen_fd, (struct sockaddr *)&m_servaddr, sizeof(m_servaddr));

  // int client_fd;
  // std::string stock_code, date, data_type, client_port;
  // struct sockaddr_in client_addr;
  // socklen_t client_addr_len = sizeof(client_addr);

  // while (1) {
  //   char sendbuf[] = "received data";  // no use
  //   listen(m_listen_fd, 5);
  //   // client_fd = accept(m_listen_fd, NULL, NULL);
  //   client_fd =
  //       accept(m_listen_fd, (struct sockaddr *)&client_addr,
  //       &client_addr_len);

  //   char client_ip[INET_ADDRSTRLEN];
  //   inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, sizeof(client_ip));

  //   memset(buf, 0, sizeof(buf));
  //   if (recv(client_fd, buf, BUFFER_SIZE, 0) == -1) {
  //     perror("recv");
  //     exit(0);
  //   } else {
  //     std::stringstream sstream(buf);
  //     sstream >> stock_code >> date >> data_type >> client_port;
  //     printf("%s\n", buf);
  //     client_addr.sin_port = htons(stoi(client_port));
  //     std::thread new_thread(worker, client_addr, stock_code, date,
  //     data_type,
  //                            m_mode);

  //     new_thread.detach();
  //   }
  // }
}

void Server::worker(char *client_ip, int client_port,
                    const std::string &stock_code, const std::string &date,
                    const std::string &data_type, const std::string &mode) {
  printf("port is %d\n", client_port);
  printf("data_type is %s\n", data_type.c_str());
  DataSource *new_data_source = new DataSource(data_type, 10);
  // std::shared_ptr<DataSource> new_data_source =
  //     std::make_shared<DataSource>(data_type, 10);

  if (data_type == "Market") {
    if (mode == "easy") {
      // IPublisher<XyMarketData> *publisher =
      //     new TCPPublisher<XyMarketData>(client_ip, client_port);

    } else {
      char c_ip[16] = "224.0.0.88";
      IPublisher<XyMarketData> *publisher =
          new UDPPublisher<XyMarketData>(c_ip, client_port);
      new_data_source->readNDataFromFile(5);
      unsigned char *tdata = new_data_source->getData();
      printf("%s \n", tdata);
      publisher->TranlateData(tdata);
      for (int i = 0; i < 30; i++) {
        publisher->PublishOneRow();
      }
    }
  }
  // send(client_fd, "lrx7777", 6, 0);
}
#include "server.h"

#include <cstring>
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

    // IPublisher<XyMarketData> *publisher = new UDPPublisher<XyMarketData>(
    //     client_ip, stoi(client_port), 0, stock_code, stoi(date), data_type,
    //     m_speed);
    std::thread new_thread(Worker, client_ip, std::stoi(client_port),
                           stock_code, stoi(date), data_type, m_mode, m_speed);

    // std::thread new_thread(publisher->Start);
    new_thread.detach();
  }
}

void Server::Worker(char *client_ip, int client_port,
                    const std::string &stock_code, const int &date,
                    const std::string &data_type, const std::string &mode,
                    int speed) {
  IPublisher<XyMarketData> *publisher = new UDPPublisher<XyMarketData>(
      client_ip, client_port, 0, stock_code, date, data_type, speed);

  publisher->SetDemo(m_demo);

  publisher->EstablishConnection(client_ip, client_port, 0);

  publisher->Start();
}

bool Server::m_demo = false;
// void Server::worker(char *client_ip, int client_port,
//                     const std::string &stock_code, const std::string &date,
//                     const std::string &data_type, const std::string &mode) {
//   printf("port is %d\n", client_port);
//   printf("data_type is %s\n", data_type.c_str());
//   // DataSource *new_data_source = new DataSource(data_type, 10);
//   // std::shared_ptr<DataSource> new_data_source =
//   //     std::make_shared<DataSource>(data_type, 10);

//   if (data_type == "Market") {
//     if (mode == "easy") {
//       // IPublisher<XyMarketData> *publisher =
//       //     new TCPPublisher<XyMarketData>(client_ip, client_port);

//     } else {
//       // char c_ip[16] = "224.0.0.88";
//       // IPublisher<XyMarketData> *publisher =
//       //     new UDPPublisher<XyMarketData>(c_ip, client_port);
//       // new_data_source->readNDataFromFile(5);
//       // unsigned char *tdata = new_data_source->getData();
//       // printf("%s \n", tdata);
//       // publisher->TranlateData(tdata);
//       // for (int i = 0; i < 30; i++) {
//       //   publisher->PublishOneRow();
//       // }

//       IPublisher<XyMarketData> *publisher =
//           new UDPPublisher<XyMarketData>(c_ip, client_port);
//     }
//   }
//   // send(client_fd, "lrx7777", 6, 0);
// }
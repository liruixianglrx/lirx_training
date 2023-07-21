#include "server.h"

#include <cstring>
#include <sstream>
#include <thread>

#define BUFFER_SIZE 1024

Server::Server(const std::string &mode, const int speed, int source_size)
    : m_speed(speed), m_mode(mode) {
  if (mode == "easy") {
    m_server_port = 8080;
  } else {
    m_server_port = 8081;
  }
}

Server::~Server() {}

void Server::start() {
  char ip[16] = "0.0.0.0";
  m_handshake.SetConnect(ip, 0, m_server_port);

  int i = 1; /*允许重复使用本地址与套接字进行绑定*/
  setsockopt(m_handshake.getSocket(), SOL_SOCKET, SO_REUSEADDR, &i,
             sizeof(int));

  while (true) {
    int send_index = m_handshake.establishConnection(true);

    char client_ip[INET_ADDRSTRLEN];
    m_handshake.getRemoteIp(client_ip);

    char buf[BUFFER_SIZE];
    m_handshake.receiveData(buf, send_index);

    std::stringstream sstream(buf);

    std::string stock_code, date, data_type, client_port;
    sstream >> stock_code >> date >> data_type >> client_port;

    std::thread new_thread(Worker, client_ip, std::stoi(client_port),
                           stock_code, stoi(date), data_type, m_mode, m_speed,
                           &m_handshake, send_index);

    new_thread.detach();
  }
}

void Server::Worker(char *client_ip, int client_port,
                    const std::string &stock_code, const int &date,
                    const std::string &data_type, const std::string &mode,
                    int speed, TCPConnection *m_handshake, int send_index) {
  IPublisher<XyMarketData> *publisher;
  if (mode == "easy") {
    publisher = new TCPPublisher<XyMarketData>(
        m_handshake, send_index, stock_code, date, data_type, speed);
  } else {
    publisher = new UDPPublisher<XyMarketData>(
        client_ip, client_port, 0, stock_code, date, data_type, speed);

    publisher->EstablishConnection(client_ip, client_port, 0);
  }

  publisher->SetDemo(m_demo);

  publisher->Start();
}

bool Server::m_demo = false;

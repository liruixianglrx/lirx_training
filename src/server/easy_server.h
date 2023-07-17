#ifndef LIRX_TRAINING_SERVER_EASY_SERVER_H
#define LIRX_TRAINING_SERVER_EASY_SERVER_H

#include "iserver.h"
class EasyServer : public IServer {
 public:
  EasyServer(/* args */);
  ~EasyServer();

 private:
  int m_listen_fd;
};

EasyServer::EasyServer(/* args */) {}

EasyServer::~EasyServer() {}

#endif  // LIRX_TRAINING_SERVER_EASY_SERVER_H
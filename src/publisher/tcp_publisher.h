#ifndef LIRX_TRAINING_PUBLISHER_TCPPUBLISHER_H
#define LIRX_TRAINING_PUBLISHER_TCPPUBLISHER_H

#include "ipublisher.h"
template <typename DataType>
class TCPPublisher : public IPublisher<DataType> {
 public:
  TCPPublisher(const char *group_addr, int port);
  ~TCPPublisher();

  void PublishOneRow() override;

 private:
  struct sockaddr_in m_remote_addr;
  int m_socket = 0;
};

#endif  // LIRX_TRAINING_PUBLISHER_TCPPUBLISHER_H
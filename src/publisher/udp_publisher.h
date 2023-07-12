#ifndef LIRX_TRAINING_PUBLISHER_UDPPUBLISHER_H
#define LIRX_TRAINING_PUBLISHER_UDPPUBLISHER_H

#include "ipublisher.h"
template <typename DataType>
class UDPPublisher : public IPublisher<DataType> {
 public:
  UDPPublisher(const char *group_addr, int port);
  ~UDPPublisher();

  void PublishOneRow() override;

 private:
  struct sockaddr_in m_remote_addr;
  int m_socket = 0;
};

#endif  // LIRX_TRAINING_PUBLISHER_UDPPUBLISHER_H
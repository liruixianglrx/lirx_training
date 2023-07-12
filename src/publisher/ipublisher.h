#ifndef LIRX_TRAINING_PUBLISHER_IPUBLISHER_H
#define LIRX_TRAINING_PUBLISHER_IPUBLISHER_H

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <string>
template <typename DataType>
class IPublisher {
 public:
  IPublisher();
  virtual ~IPublisher();
  void TranlateData(unsigned char* raw_data);

  // virtual void PublishOneRow() = 0;
  virtual void PublishOneRow() {}

  // this should be a private function;
  std::string ParseOneRow();

  //  protected:
  //   struct sockaddr_in m_remote_addr;
  //   int m_socket = 0;

 private:
  std::string GetVectorReturn(const char* prefix, uint32_t* arr, int times);
  // market data，The release of memory is done by DataSource
  DataType* m_data;

  // get the newest row;
  // std::string ParseOneRow();

  // m_size means how many rows of output a specific datatype has
  // m_process_index means where the current line is
  int m_size, m_process_index;
};

#endif  // LIRX_TRAINING_PUBLISHER_IPUBLISHER_H
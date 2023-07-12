#include <iostream>
#include <string>

#include "DataSource/DataSource.h"
#include "XyMarketData.h"
#include "publisher/ipublisher.h"
#include "publisher/udp_publisher.h"

int main(int argc, char* argv[]) {
  DataSource data_source("Market", 10);

  data_source.readNDataFromFile(5);

  std::string ip_addr("224.0.0.88");
  IPublisher<XyMarketData>* ipub =
      new UDPPublisher<XyMarketData>(ip_addr.c_str(), 8888);

  for (int i = 0; i < 5; i++) {
    unsigned char* tdata = data_source.getData();
    ipub->TranlateData(tdata);
    for (int i = 0; i < 30; i++) ipub->PublishOneRow();
  }

  return 0;
}

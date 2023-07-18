#include <iostream>
#include <string>

// #include "XyMarketData.h"
#include "data_source/data_source.h"
#include "publisher/ipublisher.h"
#include "publisher/udp_publisher.h"
// #include "server/iserver.h"
#include "server/server.h"

int main(int argc, char* argv[]) {
  // DataSource data_source("Market", 10);

  // data_source.readNDataFromFile(5);

  // std::string ip_addr("224.0.0.88");
  // IPublisher<XyMarketData>* ipub =
  //     new UDPPublisher<XyMarketData>(ip_addr.c_str(), 8888);

  // for (int i = 0; i < 5; i++) {
  //   unsigned char* tdata = data_source.getData();
  //   ipub->TranlateData(tdata);
  //   for (int i = 0; i < 30; i++) ipub->PublishOneRow();
  // }

  if (argc < 3) {
    printf("USAGE=%s [mode] [speed]\n", argv[0]);
    exit(-1);
  }

  std::string mode(argv[1]);
  std::string s_speed(argv[2]);

  Server server_instance(mode, stoi(s_speed), 10);
  // IServer server_instance(mode, "Market", stoi(s_speed), 10);
  server_instance.start();
  return 0;
}

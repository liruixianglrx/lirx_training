#include <iostream>
#include <string>

#include "data_source/data_source.h"
#include "publisher/ipublisher.h"
#include "publisher/udp_publisher.h"
#include "server/server.h"

int main(int argc, char* argv[]) {
  if (argc < 3) {
    printf("USAGE=%s [mode] [speed]\n", argv[0]);
    exit(-1);
  }

  std::string mode(argv[1]);
  std::string s_speed(argv[2]);

  Server server_instance(mode, stoi(s_speed), 10);

  if (!strcmp(argv[3], "y")) {
    server_instance.SetDemo(true);
  }

  server_instance.start();
  return 0;
}

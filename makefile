#g++ main.cpp data_source/data_source.cpp server/server.cpp publisher/ipublisher.cpp publisher/udp_publisher.cpp publisher/tcp_publisher.cpp -g -o main -L../lib -lXYData

g++ main.cpp data_source/data_source.cpp server/iserver.cpp server/server.cpp publisher/ipublisher.cpp publisher/udp_publisher.cpp publisher/tcp_publisher.cpp connection/tcp_connection.cpp connection/udp_connection.cpp -g -o main.out -L../lib -lXYData -lpthread

g++ client/client.cpp connection/tcp_connection.cpp connection/udp_connection.cpp -g -o client.out

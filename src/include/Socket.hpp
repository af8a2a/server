#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <iostream>
#include "helper.hpp"
auto Connect(const std::string& Address,int port) -> int {
  int fd = socket(AF_INET, SOCK_STREAM, 0);
  struct sockaddr_in addr = {};
  addr.sin_family = AF_INET;
  addr.sin_port = ntohs(port);
  auto ip = helper::SplitIPv4(Address);
  //todo
  addr.sin_addr.s_addr = ntohl(INADDR_LOOPBACK);  // 127.0.0.1
  int rv = connect(fd, reinterpret_cast<const struct sockaddr *>(&addr), sizeof(addr));
  return fd;
}

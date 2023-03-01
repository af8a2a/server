#pragma once
#include <arpa/inet.h>
#include <strings.h>

class InetAddress {
 public:
  struct sockaddr_in addr;
  socklen_t addr_len;
  InetAddress() : addr_len(sizeof(addr)) { bzero(&addr, sizeof(addr)); }
  InetAddress(const char *ip, uint16_t port) : addr_len(sizeof(addr)) {
    bzero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(ip);
    addr.sin_port = htons(port);
    addr_len = sizeof(addr);
  }
  auto getAddr() -> sockaddr_in { return addr; }
  ~InetAddress() = default;
};

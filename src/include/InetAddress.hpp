#pragma once
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <strings.h>
#include <unistd.h>
#include <cstring>
#include <string>

class InetAddress {
 public:
  struct sockaddr_in addr_ {};
  socklen_t addr_len_;
  InetAddress() : addr_len_(sizeof(addr_)) { bzero(&addr_, sizeof(addr_)); }
  InetAddress(const InetAddress &) = default;
  InetAddress(InetAddress &&) = delete;
  InetAddress &operator=(const InetAddress &) = default;
  InetAddress &operator=(InetAddress &&) = delete;
  InetAddress(const char *ipaddr, uint16_t port) : addr_len_(sizeof(addr_)) {
    bzero(&addr_, sizeof(addr_));
    addr_.sin_family = AF_INET;
    addr_.sin_addr.s_addr = inet_addr(ipaddr);
    addr_.sin_port = htons(port);
  }


  void SetAddr(sockaddr_in addr) { addr_ = addr; }

  const char *GetIp() {
    char buf[1024];
    return inet_ntop(AF_INET, &addr_.sin_addr, buf, sizeof(buf));
  }
  uint16_t GetPort() { return ntohs(addr_.sin_port); }
  auto GetAddr() -> sockaddr_in { return addr_; }
  ~InetAddress() = default;
};

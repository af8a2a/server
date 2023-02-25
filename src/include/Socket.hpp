#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cassert>
#include <cstring>
#include <iostream>
#include "helper.hpp"

static inline auto MakeHints(const int ai_flags, const int ai_family) -> addrinfo {
  addrinfo hints{};  // value initialized to all zeros
  hints.ai_flags = ai_flags;
  hints.ai_family = ai_family;
  return hints;
}
auto Connect(const std::string &Address, int port) -> int {
  int fd = socket(AF_INET, SOCK_STREAM, 0);
  struct sockaddr_in addr = {};
  addr.sin_family = AF_INET;
  addr.sin_port = ntohs(port);
  inet_aton(Address.c_str(), &addr.sin_addr);
  // todo// 127.0.0.1
  int rv = connect(fd, reinterpret_cast<const struct sockaddr *>(&addr), sizeof(addr));
  return fd;
}
auto Connect(const std::string &Address, const std::string &service) -> int {
  // client 連線到 server 的程式碼
  // 透過 stream socket 連線到 www.example.com 的 port 80 (http)
  // 不是 IPv4 就是 IPv6

  int sockfd;
  struct addrinfo hints;
  struct addrinfo *servinfo;
  struct addrinfo *p;
  int rv;

  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;  // 設定 AF_INET6 表示強迫使用 IPv6
  hints.ai_socktype = SOCK_STREAM;
  rv = getaddrinfo(Address.c_str(), service.c_str(), &hints, &servinfo);
  if (rv != 0) {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
    exit(1);
  }

  // 不斷執行迴圈，直到我們可以連線成功
  for (p = servinfo; p != nullptr; p = p->ai_next) {
    sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
    if (sockfd == -1) {
      perror("socket");
      continue;
    }

    if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
      close(sockfd);
      perror("connect");
      continue;
    }

    break;  // if we get here, we must have connected successfully
  }

  if (p == nullptr) {
    // 迴圈已經執行到 list 的最後，都無法連線
    fprintf(stderr, "failed to connect\n");
    exit(2);
  }

  freeaddrinfo(servinfo);  // 釋放 servinfo
  return sockfd;
}
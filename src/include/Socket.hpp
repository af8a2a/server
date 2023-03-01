#pragma once
#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cassert>
#include <cstring>
#include <iostream>
#include "InetAddress.hpp"
#include "util.hpp"
class Socket {
 public:
  int fd_;
  // buffer for reading
  char readbuffer_[1024];
  std::string readbuf_;
  std::string writebuffer_{};
  size_t buffer_size_ = 1024;
  size_t write_count_ = 0;
  auto Read() -> void {
    while (true) {
      int rec = recv(fd_, readbuffer_, buffer_size_, 0);

      if (rec != -1) {
        if (rec == 0) {
          break;
        }
        readbuffer_[rec] = '\0';
        readbuf_ += readbuffer_;
      } else {
        break;
      }
    }
  }
  auto Write(const std::string &content) -> void {
    int count = write(fd_, content.c_str(), content.size());
    assert(count != -1);

    write_count_ += count;
  }
  auto Connect(const std::string &Address, int port) -> void {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr_ = {};
    addr_.sin_family = AF_INET;
    addr_.sin_port = ntohs(port);
    inet_aton(Address.c_str(), &addr_.sin_addr);
    int rv = connect(fd, reinterpret_cast<const struct sockaddr *>(&addr_), sizeof(addr_));
    assert(rv == 0);
    fd_ = fd;
  }
   inline auto MakeHints(const int ai_flags, const int ai_family) -> addrinfo {
    addrinfo hints{};  // value initialized to all zeros
    hints.ai_flags = ai_flags;
    hints.ai_family = ai_family;
    return hints;
  }

  auto Connect(const std::string &Address, const std::string &service) -> void {
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
    fd_ = sockfd;
  }
};

class ServerSocket {
 public:
  ServerSocket() {
    fd_ = socket(AF_INET, SOCK_STREAM, 0);
    util::errif(fd_ == -1, "socket create error");
  }
  ~ServerSocket() {
    if (fd_ != -1) {
      close(fd_);
      fd_=-1;
    }
  }
  explicit ServerSocket(int fd) { fd_ = fd; }
  
  void SetNonBlocking() { fcntl(fd_, F_SETFL, fcntl(fd_, F_GETFL) | O_NONBLOCK); }

  auto Accept(InetAddress *addr) -> int {
    int clnt_sockfd = ::accept(fd_, reinterpret_cast<sockaddr *>(&addr->addr), &addr->addr_len);
    util::errif(clnt_sockfd == -1, "socket accept error");
    return clnt_sockfd;
  }
  auto Accept() -> int {
    int clnt_sockfd = ::accept(fd_, nullptr, nullptr);
    util::errif(clnt_sockfd == -1, "socket accept error");
    return clnt_sockfd;
  }
  void Bind(const std::string &ip, uint16_t port) {
    sockaddr_in addr_;
    socklen_t addr_len_;
    bzero(&addr_, sizeof(addr_));
    addr_.sin_family = AF_INET;
    addr_.sin_addr.s_addr = inet_addr(ip.c_str());
    addr_.sin_port = htons(port);
    addr_len_ = sizeof(addr_);
    int success = bind(fd_, reinterpret_cast<sockaddr *>(&addr_), addr_len_);
    util::errif(success == -1, "socket bind error");
  }
  void Listen(bool isNonBlocking) {
    int success = -1;
    success = listen(fd_, SOMAXCONN);
    assert(success != -1);

    if (isNonBlocking) {
      SetNonBlocking();
    }
  }
  auto GetFd() -> int { return fd_; }

 private:
  int fd_;
};
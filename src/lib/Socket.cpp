/******************************
 *   author: yuesong-feng
 *   客户端、服务器共用
 *   accept，connect都支持非阻塞式IO，但只是简单处理，如果情况太复杂可能会有意料之外的bug
 *
 ******************************/
#include "Socket.hh"
#include <fcntl.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cerrno>
#include <cstdio>
#include <cstring>
#include "InetAddress.hpp"
#include "util.hpp"

Socket::Socket() : fd_(socket(AF_INET, SOCK_STREAM, 0)) { errif(fd_ == -1, "socket create error"); }
Socket::Socket(int _fd) : fd_(_fd) { errif(fd_ == -1, "socket create error"); }

Socket::~Socket() {
  if (fd_ != -1) {
    close(fd_);
    fd_ = -1;
  }
}

void Socket::Bind(InetAddress *_addr) const {
  struct sockaddr_in addr = _addr->GetAddr();
  errif(::bind(fd_, (sockaddr *)&addr, sizeof(addr)) == -1, "socket bind error");
}
void Socket::Bind(const char *ipaddr, uint16_t port) const {
  assert(fd_ != -1);
  struct sockaddr_in addr {};
  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = inet_addr(ipaddr);
  addr.sin_port = htons(port);
  if (::bind(fd_, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
    perror("Failed to bind socket");
  }
}
void Socket::Listen() { errif(::listen(fd_, SOMAXCONN) == -1, "socket listen error"); }
void Socket::SetNonBlocking() { fcntl(fd_, F_SETFL, fcntl(fd_, F_GETFL) | O_NONBLOCK); }
bool Socket::IsNonBlocking() { return (fcntl(fd_, F_GETFL) & O_NONBLOCK) != 0; }
int Socket::Accept(InetAddress *_addr) {
  // for server socket
  int clnt_sockfd = -1;
  struct sockaddr_in addr {};
  bzero(&addr, sizeof(addr));
  socklen_t addr_len = sizeof(addr);
  if (fcntl(fd_, F_GETFL) & O_NONBLOCK) {
    while (true) {
      clnt_sockfd = ::accept(fd_, (sockaddr *)&addr, &addr_len);
      if (clnt_sockfd == -1 && ((errno == EAGAIN) || (errno == EWOULDBLOCK))) {
        // printf("no connection yet\n");
        continue;
      }
      if (clnt_sockfd == -1) {
        errif(true, "socket accept error");
      } else {
        break;
      }
    }
  } else {
    clnt_sockfd = ::accept(fd_, (sockaddr *)&addr, &addr_len);
    errif(clnt_sockfd == -1, "socket accept error");
  }
  _addr->SetAddr(addr);
  return clnt_sockfd;
}
  auto Socket::Connect(const std::string &Address, const std::string &service) -> void {
    // client 連線到 server 的程式碼
    // 透過 stream socket 連線到 www.example.com 的 port 80 (http)
    // 不是 IPv4 就是 IPv6

    int sockfd = 0;
    struct addrinfo hints{};
    struct addrinfo *servinfo = nullptr;
    struct addrinfo *ptr= nullptr;
    int result=-1;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;  // 設定 AF_INET6 表示強迫使用 IPv6
    hints.ai_socktype = SOCK_STREAM;
    result = getaddrinfo(Address.c_str(), service.c_str(), &hints, &servinfo);
    if (result != 0) {
      fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(result));
    }

    // 不斷執行迴圈，直到我們可以連線成功
    for (ptr = servinfo; ptr != nullptr; ptr = ptr->ai_next) {
      sockfd = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
      if (sockfd == -1) {
        perror("socket");
        continue;
      }

      if (connect(sockfd, ptr->ai_addr, ptr->ai_addrlen) == -1) {
        close(sockfd);
        perror("connect");
        continue;
      }

      break;  // if we get here, we must have connected successfully
    }

    if (ptr == nullptr) {
      // 迴圈已經執行到 list 的最後，都無法連線
      fprintf(stderr, "failed to connect\n");
    }

    freeaddrinfo(servinfo);  // 釋放 servinfo
    fd_ = sockfd;
  
};

void Socket::Connect(InetAddress *_addr) {
  // for client socket
  struct sockaddr_in addr = _addr->GetAddr();
  if (fcntl(fd_, F_GETFL) & O_NONBLOCK) {
    while (true) {
      int ret = ::connect(fd_, (sockaddr *)&addr, sizeof(addr));
      if (ret == 0) {
        break;
      }
      if (ret == -1 && (errno == EINPROGRESS)) {
        continue;
        /* 连接非阻塞式sockfd建议的做法：
            The socket is nonblocking and the connection cannot be
          completed immediately.  (UNIX domain sockets failed with
          EAGAIN instead.)  It is possible to select(2) or poll(2)
          for completion by selecting the socket for writing.  After
          select(2) indicates writability, use getsockopt(2) to read
          the SO_ERROR option at level SOL_SOCKET to determine
          whether connect() completed successfully (SO_ERROR is
          zero) or unsuccessfully (SO_ERROR is one of the usual
          error codes listed here, explaining the reason for the
          failure).
          这里为了简单、不断连接直到连接完成，相当于阻塞式
        */
      }
      if (ret == -1) {
        errif(true, "socket connect error");
      }
    }
  } else {
    errif(::connect(fd_, (sockaddr *)&addr, sizeof(addr)) == -1, "socket connect error");
  }
}
void Socket::Connect(const char *ipaddr, uint16_t port) {
  InetAddress *addr = new InetAddress(ipaddr, port);
  Connect(addr);
  delete addr;
}

int Socket::GetFd() { return fd_; }

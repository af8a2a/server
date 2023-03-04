/******************************
 *   author: yuesong-feng
 *
 *
 *
 ******************************/

#pragma once
#include <arpa/inet.h>
#include "InetAddress.hpp"
#include "Macros.h"

class Socket {
 private:
  int fd_;

 public:
  Socket();
  explicit Socket(int _fd);
  ~Socket();

  DISALLOW_COPY_AND_MOVE(Socket);

  void Bind(InetAddress *) const;
  void Listen();
  auto Accept(InetAddress *) -> int;
  void Bind(const char *ipaddr, uint16_t port) const;
  void Connect(InetAddress *);
  void Connect(const char *ipaddr, uint16_t port);
  void SetNonBlocking();
  bool IsNonBlocking();

  auto GetFd() -> int;
};

#pragma once
#include <sys/epoll.h>
#include <memory>
#include <vector>
#include "Macros.h"
class HeapTimer;
class Channel;
class Epoll {
 public:
  Epoll();

  ~Epoll();
  DISALLOW_COPY_AND_MOVE(Epoll);
  void UpdateChannel(Channel *channel);
  void DeleteChannel(Channel *channel);

  std::vector<Channel *> Poll(int timeout = -1);

 private:
  int epfd_{1};
  std::unique_ptr<HeapTimer> timer_;
  struct epoll_event *events_{nullptr};
};

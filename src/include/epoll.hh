#pragma once
#include <sys/epoll.h>
#include <vector>
#include "Macros.h"

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
  
  struct epoll_event *events_{nullptr};
};

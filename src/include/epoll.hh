#pragma once
#include <sys/epoll.h>
#include <vector>

class Channel;
class Epoll {
 public:
  Epoll();
  Epoll(const Epoll &) = default;
  Epoll(Epoll &&) = delete;
  Epoll &operator=(const Epoll &) = default;
  Epoll &operator=(Epoll &&) = delete;
  ~Epoll();
  void UpdateChannel(Channel *channel);
  void DeleteChannel(Channel *channel);

  std::vector<Channel *> Poll(int timeout = -1);

 private:
  int epfd_{1};
  struct epoll_event *events_{nullptr};
};

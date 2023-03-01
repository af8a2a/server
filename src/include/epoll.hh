#pragma once
#include <sys/epoll.h>
#include <vector>
class Channel;
class Epoll {
 public:
  Epoll();
  ~Epoll();

  void deleteChannel(Channel *);

  void UpdateChannel(Channel *channel);
  auto poll(int timeout = -1) -> std::vector<Channel *>;

 private:
  epoll_event *event_;
  int epoll_fd_;
  int max_event_{1000};
  int timeout_;
};
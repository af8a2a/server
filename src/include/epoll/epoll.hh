#pragma once
#include <sys/epoll.h>
#include <cstdint>
#include <vector>
class Channel;
class Epoll {
 public:
  Epoll();
  Epoll(int timeout, int max_event);
  ~Epoll();

  /*将文件描述符设置成非阻塞的*/
  auto Setnonblocking(int fd) -> int;

  /*将文件描述符fd上的EPOLLIN注册到epollfd指示的epoll内核事件表中，参数
enable_et指定是否对fd启用ET模式*/
  void Addfd(int fd, bool enable_et);
  void Addfd(int fd, uint32_t op);

  auto Getevent() -> epoll_event * { return event_; }
  auto GetEpollfd() -> int { return epoll_fd_; }

  auto GetTimeout() -> int { return timeout_; }

  auto Poll(int timeout) -> std::vector<Channel*>;

  void UpdateChannel(Channel *channel);
 private:
  epoll_event *event_;
  int epoll_fd_;
  int max_event_;
  int timeout_;
};
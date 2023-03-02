#pragma once

#include <functional>
class Epoll;
class Channel;
class ThreadPool;
class EventLoop {
 private:
  Epoll *ep_;
  bool quit_;

 public:
  EventLoop();
  ~EventLoop();

  void loop();
  void UpdateChannel(Channel *ch);
};

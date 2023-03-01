#pragma once

#include <functional>
class Epoll;
class Channel;
class ThreadPool;
class EventLoop {
 private:
  Epoll *ep_;
  ThreadPool *threadPool;
  bool quit_;

 public:
  EventLoop();
  ~EventLoop();

  void loop();
  void UpdateChannel(Channel *ch);
  void addThread(const std::function<void()>& func);
};

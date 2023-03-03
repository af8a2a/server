#pragma once

#include <functional>
class Epoll;
class Channel;
class ThreadPool;

class EventLoop {
 public:
  EventLoop();
  EventLoop(const EventLoop &) = default;
  EventLoop(EventLoop &&) = delete;
  EventLoop &operator=(const EventLoop &) = default;
  EventLoop &operator=(EventLoop &&) = delete;
  ~EventLoop();

  void Loop();
  void UpdateChannel(Channel *channel);

 private:
  Epoll *epoll_{nullptr};
  bool quit_{false};
};
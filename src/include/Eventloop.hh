#pragma once

#include <functional>
#include <memory>
#include "Macros.h"
#include "timer.hh"
class Epoll;
class Channel;
class ThreadPool;

class EventLoop {
 public:
  EventLoop();
  ~EventLoop();
  DISALLOW_COPY_AND_MOVE(EventLoop);
  void AddTimer();
  void Loop();
  void UpdateChannel(Channel *channel);
  void DeleteChannel(Channel *channel);

 private:
  std::unique_ptr<HeapTimer> timer_;
  time_t timeout_{-1};
  std::unique_ptr<Epoll> epoll_;
  // Epoll *epoll_{nullptr};
  bool quit_{false};
};
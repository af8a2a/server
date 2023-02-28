#pragma once
#include "channel/Channel.hh"
#include "epoll/epoll.hh"
class EventLoop {
 private:
  Epoll *ep_{nullptr};
  bool quit_{false};

 public:
  EventLoop() { ep_ = new Epoll(); }
  ~EventLoop() { delete ep_; }

  void loop();
  void UpdateChannel(Channel *ch);
};

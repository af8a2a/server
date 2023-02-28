#pragma once
#include "epoll/Channel.h"
#include "epoll/epoll.hpp"
class EventLoop {
 private:
  Epoll *ep_{nullptr};
  bool quit_{false};

 public:
  EventLoop() { ep_ = new Epoll(); }
  ~EventLoop() { delete ep_; }

  void loop() {
    while (!quit_) {
      std::vector<Channel *> chs;
      chs = ep_->Poll(-1);
      for (auto it : chs) {
        it->handleEvent();
      }
    }
  }
  void updateChannel(Channel *ch) { ep_->UpdateChannel(ch); }
};

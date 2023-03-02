#include "Eventloop.hh"
#include <vector>
#include "Channel.hh"
#include "epoll.hh"
#include "threadpool.hh"
EventLoop::EventLoop() : ep_(nullptr), quit_(false) {
  ep_ = new Epoll();
  
}
EventLoop::~EventLoop() {
  delete ep_;
}
void EventLoop::loop() {
  while (!quit_) {
    std::vector<Channel *> chs;
    chs = ep_->poll();
    for (auto it : chs) {
      it->handleEvent();
    }
  }
}
void EventLoop::UpdateChannel(Channel *ch) { ep_->UpdateChannel(ch); }


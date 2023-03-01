#include "Eventloop.hh"
#include "epoll.hh"
#include "Channel.hh"
#include <vector>
EventLoop::EventLoop() { ep_ = new Epoll(); }
EventLoop::~EventLoop() { delete ep_; }
void EventLoop::loop() {
  while (!quit_) {
    std::vector<Channel *> chs;
    chs = ep_->Poll(-1);
    for (auto it : chs) {
      it->handleEvent();
    }
  }
}
void EventLoop::UpdateChannel(Channel *ch) { ep_->UpdateChannel(ch); }
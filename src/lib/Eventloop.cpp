
#include <memory>
#include <vector>

#include "Channel.hh"
#include "Eventloop.hh"
#include "epoll.hh"
#include "timer.hh"

EventLoop::EventLoop() : epoll_(new Epoll()) {}

EventLoop::~EventLoop() { delete epoll_; }

void EventLoop::Loop() {
  while (!quit_) {
    if (timeout_ > 0) {
    }
    for (auto &channel : epoll_->Poll()) {
      channel->HandleEvent();
    }
  }
}

void EventLoop::UpdateChannel(Channel *channel) { epoll_->UpdateChannel(channel); }
void EventLoop::DeleteChannel(Channel *channel) { epoll_->DeleteChannel(channel); }


#include <vector>

#include "Channel.hh"
#include "Eventloop.hh"
#include "epoll.hh"

EventLoop::EventLoop() : epoll_(new Epoll()) {  }

EventLoop::~EventLoop() { delete epoll_; }

void EventLoop::Loop() {
  while (!quit_) {
    std::vector<Channel *> chs;
    chs = epoll_->Poll();
    for (auto &channel : chs) {
      channel->HandleEvent();
    }
  }
}

void EventLoop::UpdateChannel(Channel *channel) { epoll_->UpdateChannel(channel); }
void EventLoop::DeleteChannel(Channel *channel) { epoll_->DeleteChannel(channel); }

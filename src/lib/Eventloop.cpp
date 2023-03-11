
#include <memory>
#include <vector>

#include "Channel.hh"
#include "Eventloop.hh"
#include "epoll.hh"
#include "timer.hh"

EventLoop::EventLoop() {
  epoll_ = std::make_unique<Epoll>();
  timer_=std::make_unique<HeapTimer>();
}

EventLoop::~EventLoop() = default;

void EventLoop::Loop() {
  while (!quit_) {
    for (auto &channel : epoll_->Poll()) {
      channel->HandleEvent();
    }
  }
}
    
void EventLoop::UpdateChannel(Channel *channel) { epoll_->UpdateChannel(channel); }


void EventLoop::DeleteChannel(Channel *channel) { epoll_->DeleteChannel(channel); }

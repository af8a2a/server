
#include <iostream>
#include <memory>
#include <vector>

#include "Channel.hh"
#include "Eventloop.hh"
#include "Socket.hh"
#include "TcpServer.hh"
#include "epoll.hh"
#include "timer.hh"

EventLoop::EventLoop() {
  epoll_ = std::make_unique<Epoll>();
  timer_=std::make_unique<HeapTimer>();
}

EventLoop::~EventLoop() = default;

void EventLoop::Loop() {
  while (!quit_) {
    int time_ms=0;
    if (timeout_ > 0) {

      time_ms = timer_->GetNextTick();
      std::cout<<time_ms<<std::endl;
    }
    for (auto &channel : epoll_->Poll(time_ms)) {
      channel->HandleEvent();
    }
  }
}
    
void EventLoop::UpdateChannel(Channel *channel) { epoll_->UpdateChannel(channel); }

void EventLoop::AddTimer(int _fd, const std::function<void()> &func) {
  timer_->Add(_fd, timeout_,func );
}
void EventLoop::SetTimeout(int timeout) {
  timeout_=timeout;
}
void EventLoop::DeleteChannel(Channel *channel) {
  epoll_->DeleteChannel(channel);
  }

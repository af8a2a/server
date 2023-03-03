
#include "Channel.hh"
#include <unistd.h>
#include <utility>
#include "Eventloop.hh"
#include "epoll.hh"

Channel::Channel(EventLoop *loop, int _fd)
    : loop_(loop), fd_(_fd), listen_events_(0), ready_events_(0), in_epoll_(false) {}

Channel::~Channel() {
  if (fd_ != -1) {
    close(fd_);
    fd_ = -1;
  }
}

void Channel::HandleEvent() {
  if (ready_events_ & (EPOLLIN | EPOLLPRI)) {
    read_callback_();
  }
  if (ready_events_ & (EPOLLOUT)) {
    write_callback_();
  }
}

void Channel::EnableRead() {
  listen_events_ |= EPOLLIN | EPOLLPRI;
  loop_->UpdateChannel(this);
}

void Channel::UseET() {
  listen_events_ |= EPOLLET;
  loop_->UpdateChannel(this);
}
int Channel::GetFd() { return fd_; }

uint32_t Channel::GetListenEvents() { return listen_events_; }
uint32_t Channel::GetReadyEvents() { return ready_events_; }

bool Channel::GetInEpoll() { return in_epoll_; }

void Channel::SetInEpoll(bool setting) { in_epoll_ = setting; }

void Channel::SetReadyEvents(uint32_t event) { ready_events_ = event; }

void Channel::SetReadCallback(std::function<void()> const &callback) { read_callback_ = callback; }

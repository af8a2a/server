
#include "Channel.hh"
#include <unistd.h>
#include <utility>
#include "Eventloop.hh"
#include "epoll.hh"
#include "timer.hh"
const int Channel::READ_EVENT = 1;
const int Channel::WRITE_EVENT = 2;
const int Channel::ET = 4;  // NOLINT
const int Channel::TIMEOUT_EVENT=8;
Channel::Channel(EventLoop *loop, Socket *socket) : loop_(loop), socket_(socket) {}

Channel::~Channel() { loop_->DeleteChannel(this); }

void Channel::HandleEvent() {
  if (ready_events_ & READ_EVENT) {
    read_callback_();
  }
  if (ready_events_ & WRITE_EVENT) {
    write_callback_();
  }
}

void Channel::EnableRead() {
  listen_events_ |= READ_EVENT;
  loop_->UpdateChannel(this);
}

void Channel::EnableWrite() {
  listen_events_ |= WRITE_EVENT;
  loop_->UpdateChannel(this);
}

void Channel::EnableTimeout() {
  listen_events_ |= TIMEOUT_EVENT;
  loop_->UpdateChannel(this);
}

void Channel::UseET() {
  listen_events_ |= ET;
  loop_->UpdateChannel(this);
}

Socket *Channel::GetSocket() { return socket_; }

uint32_t Channel::GetListenEvents() { return listen_events_; }
uint32_t Channel::GetReadyEvents() { return ready_events_; }

bool Channel::GetInEpoll() { return in_epoll_; }

void Channel::SetInEpoll(bool setting) { in_epoll_ = setting; }

void Channel::SetReadyEvents(uint32_t event) {
  if (event & READ_EVENT) {
    ready_events_ |= READ_EVENT;
  }
  if (event & WRITE_EVENT) {
    ready_events_ |= WRITE_EVENT;
  }
  if (event & ET) {
    ready_events_ |= ET;
  }
}

void Channel::SetReadCallback(std::function<void()> const &callback) { read_callback_ = callback; }
void Channel::SetWriteCallback(std::function<void()> const &callback) { write_callback_ = callback; }
void Channel::SetTimeoutCallback(std::function<void()> const &callback) { timeout_callback_ = callback; }

std::function<void()> &Channel::GetTimeoutCallback() { return timeout_callback_; }

int Channel::GetTimeout() { return timeout_; }
void Channel::SetTimeout(int timeout) { timeout_ = timeout; }

bool Channel::ShouldDelete() {
  return should_delete_;
}
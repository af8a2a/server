#include "epoll.hh"
#include <strings.h>
#include <unistd.h>
#include <cstring>
#include <memory>
#include "Channel.hh"
#include "Socket.hh"
#include "timer.hh"
#include "util.hpp"
#define MAX_EVENTS 1000
Epoll::Epoll() : epfd_(epoll_create1(0)), events_(new epoll_event[MAX_EVENTS]) {
  errif(epfd_ == -1, "epoll create error");
  //timer_ = std::make_unique<HeapTimer>();
  memset(events_, 0, sizeof(*events_) * MAX_EVENTS);
}

Epoll::~Epoll() {
  if (epfd_ != -1) {
    close(epfd_);
  }
  delete[] events_;
}

std::vector<Channel *> Epoll::Poll(int timeout) {
  std::vector<Channel *> active_channels;
  int nfds = epoll_wait(epfd_, events_, MAX_EVENTS, timeout);
  errif(nfds == -1 && errno != EINTR, "epoll wait error");
  for (int i = 0; i < nfds; ++i) {
    Channel *channel = (Channel *)events_[i].data.ptr;
    int events = static_cast<int>(events_[i].events);
    if (events & EPOLLIN) {
      channel->SetReadyEvents(Channel::READ_EVENT);
    }
    if (events & EPOLLOUT) {
      channel->SetReadyEvents(Channel::WRITE_EVENT);
    }
    if (events & EPOLLET) {
      channel->SetReadyEvents(Channel::ET);
    }
    active_channels.push_back(channel);
  }
  return active_channels;
}

void Epoll::UpdateChannel(Channel *channel) {
  int sockfd = channel->GetSocket()->GetFd();
  //timer_->Add(channel->GetTimeout(),channel.)
  struct epoll_event event {};
  event.data.ptr = channel;
  if (channel->GetListenEvents() & Channel::READ_EVENT) {
    event.events |= EPOLLIN | EPOLLPRI;
  }
  if (channel->GetListenEvents() & Channel::WRITE_EVENT) {
    event.events |= EPOLLOUT;
  }
  if (channel->GetListenEvents() & Channel::ET) {
    event.events |= EPOLLET;
  }
  if (channel->GetListenEvents()& Channel::TIMEOUT_EVENT) {
    DeleteChannel(channel);
    return;
  }
  if (!channel->GetInEpoll()) {
    errif(epoll_ctl(epfd_, EPOLL_CTL_ADD, sockfd, &event) == -1, "epoll add error");
    //timer_->Add(channel->GetSocket()->GetFd(), channel->GetTimeout(), channel->GetTimeoutCallback());
    channel->SetInEpoll();
  } else {
    errif(epoll_ctl(epfd_, EPOLL_CTL_MOD, sockfd, &event) == -1, "epoll modify error");
  }
}

void Epoll::DeleteChannel(Channel *channel) {
  int sockfd = channel->GetSocket()->GetFd();
  errif(epoll_ctl(epfd_, EPOLL_CTL_DEL, sockfd, nullptr) == -1, "epoll delete error");
  channel->SetInEpoll(false);
}

#include "epoll.hh"
#include <strings.h>
#include <unistd.h>
#include "Channel.hh"
#include "util.hpp"
#define MAX_EVENTS 1000
Epoll::Epoll() : epoll_fd_(-1), event_(nullptr) {
  epoll_fd_ = epoll_create1(0);
  util::errif(epoll_fd_ == -1, "epoll create error");
  event_ = new epoll_event[MAX_EVENTS];
  bzero(event_, sizeof(*event_) * MAX_EVENTS);
}

Epoll::~Epoll() {
  if (epoll_fd_ != -1) {
    close(epoll_fd_);
    epoll_fd_ = -1;
  }
  delete[] event_;
}

auto Epoll::poll(int timeout) -> std::vector<Channel *> {
  std::vector<Channel *> activeChannels;
  int nfds = epoll_wait(epoll_fd_, event_, MAX_EVENTS, timeout);
  util::errif(nfds == -1, "epoll wait error");
  for (int i = 0; i < nfds; ++i) {
    auto *ch = static_cast<Channel *>(event_[i].data.ptr);
    ch->setReady(event_[i].events);
    activeChannels.push_back(ch);
  }
  return activeChannels;
}

void Epoll::UpdateChannel(Channel *channel) {
  int fd = channel->getFd();
  struct epoll_event ev;
  bzero(&ev, sizeof(ev));
  ev.data.ptr = channel;
  ev.events = channel->getEvents();
  if (!channel->getInEpoll()) {
    util::errif(epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, fd, &ev) == -1, "epoll add error");
    channel->setInEpoll();
  } else {
    util::errif(epoll_ctl(epoll_fd_, EPOLL_CTL_MOD, fd, &ev) == -1, "epoll modify error");
  }
}

void Epoll::deleteChannel(Channel *channel) {
  int fd = channel->getFd();
  util::errif(epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, fd, nullptr) == -1, "epoll delete error");
  channel->setInEpoll(false);
}
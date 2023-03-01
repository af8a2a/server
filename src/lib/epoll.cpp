#include "epoll.hh"
#include <strings.h>
#include <unistd.h>
#include "Channel.hh"
#include "util.hpp"
Epoll::Epoll() : epoll_fd_(-1), event_(nullptr) {
  epoll_fd_ = epoll_create1(0);
  util::errif(epoll_fd_ == -1, "epoll create error");
  event_ = new epoll_event[1000];
  bzero(event_, sizeof(*event_) * 1000);
}
Epoll::Epoll(int timeout, int max_event) : timeout_(timeout), max_event_(max_event) {
  epoll_fd_ = epoll_create(max_event);
  event_ = new epoll_event[max_event];
  bzero(event_, sizeof(*event_) * max_event);
}
Epoll::~Epoll() {
  if (epoll_fd_ != -1) {
    close(epoll_fd_);
    epoll_fd_ = -1;
  }
  delete[] event_;
}
void Epoll::Addfd(int fd, bool enable_et) {
  epoll_event event;
  bzero(&event, sizeof(event));
  event.data.fd = fd;
  event.events = EPOLLIN;
  if (enable_et) {
    event.events |= EPOLLET;
  }
  util::errif(epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, fd, &event) == -1, "epoll add event error");
}
void Epoll::Addfd(int fd, uint32_t op) {
  struct epoll_event ev;
  bzero(&ev, sizeof(ev));
  ev.data.fd = fd;
  ev.events = op;
  util::errif(epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, fd, &ev) == -1, "epoll add event error");
}

auto Epoll::Poll(int timeout) -> std::vector<Channel *> {
  std::vector<Channel *> activeEvents;
  int nfds = epoll_wait(epoll_fd_, event_, max_event_, timeout);
  util::errif(nfds == -1, "epoll wait error");
  activeEvents.reserve(nfds);
  for (int i = 0; i < nfds; ++i) {
    auto *ch = static_cast<Channel *>(event_[i].data.ptr);
    ch->setRevents(event_[i].events);
    activeEvents.push_back(ch);
  }
  return activeEvents;
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
    // debug("Epoll: add Channel to epoll tree success, the Channel's fd is: ", fd);
  } else {
    util::errif(epoll_ctl(epoll_fd_, EPOLL_CTL_MOD, fd, &ev) == -1, "epoll modify error");
    // debug("Epoll: modify Channel in epoll tree success, the Channel's fd is: ", fd);
  }
}
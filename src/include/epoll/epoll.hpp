#include <fcntl.h>
#include <strings.h>
#include <sys/epoll.h>
#include <memory>
#include <vector>
#include "epoll/Channel.h"
#include "util/util.hpp"
class Epoll {
 public:
  Epoll() : epoll_fd_(-1), event_(nullptr){
    epoll_fd_ = epoll_create1(0);
    util::errif(epoll_fd_ == -1, "epoll create error");
    event_ = new epoll_event[max_event_];
    bzero(event_, sizeof(*event_) * max_event_);
}
  Epoll(int timeout, int max_event) : timeout_(timeout), max_event_(max_event) {
    epoll_fd_ = epoll_create(max_event);
    event_ = new epoll_event[max_event];
    bzero(event_, sizeof(*event_) * max_event);
  }
  ~Epoll() {
    close(epoll_fd_);
    delete[] event_;
  }

  /*将文件描述符设置成非阻塞的*/
  auto Setnonblocking(int fd) -> int {
    int old_option = fcntl(fd, F_GETFL);
    int new_option = old_option | O_NONBLOCK;
    fcntl(fd, F_SETFL, new_option);
    return old_option;
  }

  /*将文件描述符fd上的EPOLLIN注册到epollfd指示的epoll内核事件表中，参数
enable_et指定是否对fd启用ET模式*/
  void Addfd(int fd, bool enable_et) {
    epoll_event event;
    bzero(&event, sizeof(event));
    event.data.fd = fd;
    event.events = EPOLLIN;
    if (enable_et) {
      event.events |= EPOLLET;
    }
    util::errif(epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, fd, &event) == -1, "epoll add event error");
  }
  void Addfd(int fd, uint32_t op){
    struct epoll_event ev;
    bzero(&ev, sizeof(ev));
    ev.data.fd = fd;
    ev.events = op;
    util::errif(epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, fd, &ev) == -1, "epoll add event error");
}

  auto Getevent() -> epoll_event * { return event_; }
  auto GetEpollfd() -> int { return epoll_fd_; }

  auto GetTimeout() -> int { return timeout_; }

  auto Poll(int timeout) -> std::vector<Channel*> {
    std::vector<Channel*> activeEvents;
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

  void UpdateChannel(Channel *channel) {
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

 private:
  epoll_event *event_;
  int epoll_fd_;
  int max_event_;
  int timeout_;
};
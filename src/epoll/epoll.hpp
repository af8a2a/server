#include <fcntl.h>
#include <sys/epoll.h>
#include <memory>
#include <vector>

class Epoll {
 public:
  Epoll(int timeout, int max_event) : timeout_(timeout), max_event_(max_event) {
    epoll_fd_ = epoll_create(max_event);
    event_ = new epoll_event[max_event];
  }
  ~Epoll() {
    close(epoll_fd_);
    delete[] event_;
  }

  auto Wait() -> int { return epoll_wait(epoll_fd_, event_, max_event_, timeout_); }

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
    event.data.fd = fd;
    event.events = EPOLLIN;
    if (enable_et) {
      event.events |= EPOLLET;
    }
    epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, fd, &event);
    Setnonblocking(fd);
  }
  auto Getevent() -> epoll_event * {
    return event_;
  }
  auto GetEpollfd() -> int { return epoll_fd_; }

  auto GetTimeout() -> int {
    return timeout_;
  }
 private:
  epoll_event *event_;  
  int epoll_fd_;
  int max_event_;
  int timeout_;
};
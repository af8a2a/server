#include "Eventloop.hh"
#include <vector>
#include "Channel.hh"
#include "epoll.hh"
#include "threadpool.hh"
EventLoop::EventLoop() : threadPool(nullptr), ep_(nullptr), quit_(false) {
  ep_ = new Epoll();
  threadPool = new ThreadPool();
}
EventLoop::~EventLoop() {
  delete ep_;
  delete threadPool;
}
void EventLoop::loop() {
  while (!quit_) {
    std::vector<Channel *> chs;
    chs = ep_->poll();
    for (auto it : chs) {
      it->handleEvent();
    }
  }
}
void EventLoop::UpdateChannel(Channel *ch) { ep_->UpdateChannel(ch); }

void EventLoop::addThread(const std::function<void()>& func) {
  threadPool->add(func);
}
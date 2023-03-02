#pragma once
#include <sys/epoll.h>
#include <functional>
class EventLoop;
class Channel {
 private:
  EventLoop *loop;
  int fd;
  uint32_t events;
  uint32_t ready;
  ;
  bool inEpoll;
  std::function<void()> readCallback;
  std::function<void()> writeCallback;

 public:
  Channel(EventLoop *_loop, int _fd);
  ~Channel();
  void handleEvent();
  void enableReading();

  auto getFd() -> int;
  auto getEvents() -> uint32_t;
  auto getReady() -> uint32_t;

  auto getInEpoll() -> bool;
  void setInEpoll(bool _in = true);
  void useET();

  void setReady(uint32_t);
  void setReadCallback(std::function<void()>);
};

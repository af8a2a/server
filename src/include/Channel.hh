#pragma once
#include <sys/epoll.h>
#include <functional>
class EventLoop;

class Socket;
class EventLoop;
class Channel {
 public:
  Channel(const Channel &) = default;
  Channel(Channel &&) = delete;
  Channel &operator=(const Channel &) = default;
  Channel &operator=(Channel &&) = delete;
  Channel(EventLoop *loop, int _fd);
  ~Channel();


  void HandleEvent();
  void EnableRead();

  int GetFd();
  uint32_t GetListenEvents();
  uint32_t GetReadyEvents();
  bool GetInEpoll();
  void SetInEpoll(bool setting = true);
  void UseET();

  void SetReadyEvents(uint32_t event);
  void SetReadCallback(std::function<void()> const &callback);

 private:
  EventLoop *loop_;
  int fd_;
  uint32_t listen_events_;
  uint32_t ready_events_;
  bool in_epoll_;
  std::function<void()> read_callback_;
  std::function<void()> write_callback_;
};
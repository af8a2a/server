#pragma once
#include <sys/epoll.h>
#include <functional>
#include "Macros.h"
class EventLoop;

class Socket;
class EventLoop;
class Channel {
 public:
  Channel(EventLoop *loop, Socket *socket);
  ~Channel();
  DISALLOW_COPY_AND_MOVE(Channel);

  void HandleEvent();
  void EnableRead();
  void EnableWrite();

  uint32_t GetListenEvents();
  uint32_t GetReadyEvents();
  bool GetInEpoll();
  void SetInEpoll(bool setting = true);
  void UseET();
  Socket *GetSocket();
  void SetReadyEvents(uint32_t event);
  void SetReadCallback(std::function<void()> const &callback);
  void SetWriteCallback(std::function<void()> const &callback);

  
  static const int READ_EVENT;   // NOLINT
  static const int WRITE_EVENT;  // NOLINT
  static const int ET;           // NOLINT
 private:
  EventLoop *loop_;
  Socket *socket_;
  int listen_events_{0};
  int ready_events_{0};
  bool in_epoll_{false};
  std::function<void()> read_callback_;
  std::function<void()> write_callback_;
};
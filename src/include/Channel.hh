#pragma once
#include <sys/epoll.h>
#include <functional>
#include "Macros.h"

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
  bool ShouldDelete();

  void SetReadyEvents(uint32_t event);
  void SetReadCallback(std::function<void()> const &callback);
  void SetWriteCallback(std::function<void()> const &callback);
  void SetTimeoutCallback(std::function<void()> const &callback);
  std::function<void()> &GetTimeoutCallback();
  int GetTimeout();
  void SetTimeout(int timeout);
  void EnableTimeout();

  static const int READ_EVENT;
  static const int WRITE_EVENT;
  static const int TIMEOUT_EVENT;
  static const int ET;  // NOLINT

 private:
  EventLoop *loop_;
  Socket *socket_;
  int listen_events_{0};
  int ready_events_{0};
  bool in_epoll_{false};
  bool should_delete_{false};
  int timeout_{-1};
  std::function<void()> read_callback_;
  std::function<void()> write_callback_;
  std::function<void()> timeout_callback_;
};
#pragma once

class Epoll;
class Channel;
class EventLoop {
 private:
  Epoll *ep_{nullptr};
  bool quit_{false};

 public:
  EventLoop();
  ~EventLoop();

  void loop();
  void UpdateChannel(Channel *ch);
};

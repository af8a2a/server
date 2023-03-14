#pragma once
#include <functional>
#include <memory>
#include "Macros.h"
class HeapTimer;
class EventLoop;
class Socket;
class InetAddress;
class Channel;
class Acceptor
{
private:
    std::unique_ptr<Socket> sock_;
    std::unique_ptr<Channel> accept_channel_;
    std::function<void(Socket *)> new_connection_callback_;
      std::unique_ptr<HeapTimer> timer_;

public:

 explicit Acceptor(EventLoop *_loop);
 ~Acceptor();
 DISALLOW_COPY_AND_MOVE(Acceptor);
 void AcceptConnection();
 void SetNewConnectionCallback(const std::function<void(Socket *)> &);
};


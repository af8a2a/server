#pragma once
#include <functional>
#include "Macros.h"

class EventLoop;
class Socket;
class InetAddress;
class Channel;
class Acceptor
{
private:
    EventLoop *loop_;
    Socket *sock_;
    Channel *accept_channel_;
    std::function<void(Socket*)> new_connection_callback_;
public:

 explicit Acceptor(EventLoop *_loop);
 ~Acceptor();
 DISALLOW_COPY_AND_MOVE(Acceptor);
 void AcceptConnection();
 void SetNewConnectionCallback(const std::function<void(Socket *)> &);
};


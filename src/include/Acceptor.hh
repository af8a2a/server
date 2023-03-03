#pragma once
#include <functional>

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
 Acceptor(const Acceptor &) = default;
 Acceptor(Acceptor &&) = delete;
 Acceptor &operator=(const Acceptor &) = default;
 Acceptor &operator=(Acceptor &&) = delete;
 explicit Acceptor(EventLoop *_loop);
 ~Acceptor();
 void AcceptConnection();
 void SetNewConnectionCallback(const std::function<void(Socket *)> &);
};


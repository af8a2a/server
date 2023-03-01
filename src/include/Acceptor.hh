#pragma once
#include <functional>

class EventLoop;
class ServerSocket;
class InetAddress;
class Channel;
class Acceptor
{
private:
    EventLoop *loop;
    ServerSocket *sock;
    InetAddress *addr;
    Channel *acceptChannel;
public:
    explicit Acceptor(EventLoop *_loop);
    ~Acceptor();
    void acceptConnection();
    std::function<void(ServerSocket*)> newConnectionCallback;
    void setNewConnectionCallback(const std::function<void(ServerSocket*)>&);
};


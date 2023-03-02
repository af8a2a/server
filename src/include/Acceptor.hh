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
    Channel *acceptChannel;
    std::function<void(ServerSocket*)> newConnectionCallback;
public:
    explicit Acceptor(EventLoop *_loop);
    ~Acceptor();
    void acceptConnection();
    void setNewConnectionCallback(const std::function<void(ServerSocket*)>&);
};


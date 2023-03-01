#pragma once
#include <functional>

class EventLoop;
class ServerSocket;
class Channel;
class Connection
{
private:
    EventLoop *loop;
    ServerSocket *sock;
    Channel *channel;
    std::function<void(ServerSocket*)> deleteConnectionCallback;
public:
    Connection(EventLoop *_loop, ServerSocket *_sock);
    ~Connection();
    
    void echo(int sockfd);
    void setDeleteConnectionCallback(std::function<void(ServerSocket*)>);
};


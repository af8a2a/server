#pragma once
#include <functional>
#include <string>
class Buffer;
class EventLoop;
class ServerSocket;
class Channel;
class Connection
{
private:
    EventLoop *loop;
    ServerSocket *sock;
    Channel *channel;
    std::function<void(int)> deleteConnectionCallback;
    Buffer *readBuffer;
public:
    Connection(EventLoop *_loop, ServerSocket *_sock);
    ~Connection();
    
    void echo(int sockfd);
    void setDeleteConnectionCallback(std::function<void(int)>);
    void send(int sockfd);
};


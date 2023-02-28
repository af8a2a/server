#pragma once

class EventLoop;
class ServerSocket;
class Server
{
private:
    EventLoop *loop;
public:
    explicit Server(EventLoop*);
    ~Server()=default;

    void handleReadEvent(int);
    void newConnection(ServerSocket *serv_sock);
};

#pragma once

class EventLoop;
class ServerSocket;
class Acceptor;
class Server
{
private:
 EventLoop *loop;
Acceptor* acceptor;
 //Acceptor *acceptor;
public:
    explicit Server(EventLoop* _loop);
    void handleReadEvent(int);
    void newConnection(ServerSocket *serv_sock);
};

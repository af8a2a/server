#include "server/Acceptor.hh"
#include "Socket.hpp"
#include "channel/Channel.hh"


Acceptor::Acceptor(EventLoop *_loop) : loop(_loop)
{
    sock = new ServerSocket();
    sock->Bind("127.0.0.1", 8080);
    sock->Listen(true); 
    acceptChannel = new Channel(loop, sock->GetFd());
    std::function<void()> cb = [this] { acceptConnection(); };
    acceptChannel->setCallback(cb);
    acceptChannel->enableReading();
}



void Acceptor::acceptConnection(){
    newConnectionCallback(sock);
}

void Acceptor::setNewConnectionCallback(const std::function<void(ServerSocket*)>& _cb){
    newConnectionCallback = _cb;
}
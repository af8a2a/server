#include "Acceptor.hh"
#include "Channel.hh"
#include "Socket.hpp"

Acceptor::Acceptor(EventLoop *_loop) : loop(_loop) {
  sock = new ServerSocket();
  sock->Bind("127.0.0.1", 8000);
  sock->Listen(true);
  acceptChannel = new Channel(loop, sock->GetFd());
  std::function<void()> cb = [this] { acceptConnection(); };
  acceptChannel->setReadCallback(cb);
  acceptChannel->enableReading();
  acceptChannel->setUseThreadPool(false);
}
Acceptor::~Acceptor() {
  delete sock;
  delete addr;
  delete acceptChannel;
}

void Acceptor::acceptConnection() {
  auto *clnt_addr = new InetAddress();
  auto *clnt_sock = new ServerSocket(sock->Accept(clnt_addr));
  printf("new client fd %d! IP: %s Port: %d\n", clnt_sock->GetFd(), inet_ntoa(clnt_addr->getAddr().sin_addr),
         ntohs(clnt_addr->getAddr().sin_port));
  clnt_sock->SetNonBlocking();
  newConnectionCallback(clnt_sock);
  delete clnt_addr;
}

void Acceptor::setNewConnectionCallback(const std::function<void(ServerSocket *)> &_cb) { newConnectionCallback = _cb; }
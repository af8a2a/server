#include "Acceptor.hh"
#include <fcntl.h>
#include <cstdio>
#include <memory>
#include "Channel.hh"
#include "Socket.hh"
#include "timer.hh"

Acceptor::Acceptor(EventLoop *_loop) {
  sock_ = std::make_unique<Socket>();
  sock_->Bind("127.0.0.1", 1234);
  //sock_->SetNonBlocking(); //acceptor使用阻塞式IO比较好
  sock_->Listen();

  accept_channel_ = std::make_unique<Channel>(_loop, sock_.get());
  std::function<void()> callback = std::bind(&Acceptor::AcceptConnection, this);
  accept_channel_->SetReadCallback(callback);
  timer_=std::make_unique<HeapTimer>();
  accept_channel_->EnableRead();
}
Acceptor::~Acceptor() = default;

//接受新连接的起始
void Acceptor::AcceptConnection() {
  std::unique_ptr<InetAddress> clnt_addr = std::make_unique<InetAddress>();
  int clnt_fd = sock_->Accept(clnt_addr.get());
  printf("new client fd %d! IP: %s Port: %d\n", clnt_fd, clnt_addr->GetIp(), clnt_addr->GetPort());
  fcntl(clnt_fd, F_SETFL, fcntl(clnt_fd, F_GETFL) | O_NONBLOCK);  // 新接受到的连接设置为非阻塞式
  // 生成新的socket
  if (new_connection_callback_) {
    new_connection_callback_(new Socket(clnt_fd));
  }
}

void Acceptor::SetNewConnectionCallback(const std::function<void(Socket *)> &_cb) { new_connection_callback_ = _cb; }
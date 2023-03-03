#include "Acceptor.hh"
#include <cstdio>
#include "Channel.hh"
#include "Socket.hh"

Acceptor::Acceptor(EventLoop *_loop) : loop_(_loop), sock_(new Socket()), accept_channel_(new Channel(loop_, sock_->GetFd())) {
  InetAddress *addr = new InetAddress("127.0.0.1", 1234);

  sock_->Bind(addr);
  sock_->Listen();
  
  std::function<void()> callback = [this] { AcceptConnection(); };
  accept_channel_->SetReadCallback(callback);
  accept_channel_->EnableRead();
}
Acceptor::~Acceptor() {
  delete sock_;
  delete accept_channel_;
}

void Acceptor::AcceptConnection() {
  InetAddress *clnt_addr = new InetAddress();
  Socket *clnt_sock = new Socket(sock_->Accept(clnt_addr));
  printf("new client fd %d! IP: %s Port: %d\n", clnt_sock->GetFd(), clnt_addr->GetIp(), clnt_addr->GetPort());
  clnt_sock->SetNonBlocking();  // 新接受到的连接设置为非阻塞式
  new_connection_callback_(clnt_sock);
  delete clnt_addr;

}

void Acceptor::SetNewConnectionCallback(const std::function<void(Socket *)> &_cb) { new_connection_callback_ = _cb; }
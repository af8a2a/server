#include "TcpServer.hh"
#include <arpa/inet.h>
#include <iostream>
#include <memory>
#include "Acceptor.hh"
#include "Channel.hh"
#include "Connection.hh"
#include "Eventloop.hh"
#include "InetAddress.hpp"
#include "Socket.hh"
#include "threadpool.hh"
#include "util.hpp"
#define READ_BUFFER 1024

TcpServer::TcpServer() {
  main_reactor_ = std::make_unique<EventLoop>();
  acceptor_ = std::make_unique<Acceptor>(main_reactor_.get());
  thread_pool_ = std::make_unique<ThreadPool>();
  std::function<void(Socket *)> callback = std::bind(&TcpServer::NewConnection, this, std::placeholders::_1);
  acceptor_->SetNewConnectionCallback(callback);

  int size = static_cast<int>(std::thread::hardware_concurrency());
  thread_pool_ = std::make_unique<ThreadPool>(size);
  for (int i = 0; i < size; ++i) {
    sub_reactors_.emplace_back(std::make_unique<EventLoop>());
  }
  main_reactor_->SetTimeout(20000);
}
void TcpServer::Start() {
  for (auto &sub_reactor : sub_reactors_) {
    std::function<void()> sub_loop = std::bind(&EventLoop::Loop, sub_reactor.get());
    thread_pool_->Add(std::move(sub_loop));
  }
  printf("success\n");
  main_reactor_->Loop();
}
TcpServer::~TcpServer() = default;

void TcpServer::NewConnection(Socket *sock) {
  //新连接到达，随机分配给一个线程
  errif(sock->GetFd() == -1, "new connection error");
  uint64_t random = sock->GetFd() % sub_reactors_.size();
  auto conn = std::make_unique<Connection>(sub_reactors_[random].get(), sock);
  printf("connect random\n");
  std::function<void(Socket *)> callback = std::bind(&TcpServer::DeleteConnection, this, std::placeholders::_1);
  conn->SetDeleteConnectionCallback(callback);

  //conn->AddTimer(sock->GetFd(), std::bind(&TcpServer::DeleteConnection, this, sock));
  // conn->SetOnConnectCallback(on_connect_callback_);
  conn->SetOnMessageCallback(on_message_callback_);

   main_reactor_->AddTimer(sock->GetFd(), std::bind(&TcpServer::DeleteConnection, this, sock));

  connections_[sock->GetFd()] = std::move(conn);
  if (new_connect_callback_) {
    new_connect_callback_(connections_[sock->GetFd()].get());
  }
}

void TcpServer::DeleteConnection(Socket *sock) {
  int sockfd = sock->GetFd();
  auto iter = connections_.find(sockfd);
  if (iter != connections_.end()) {
    iter->second->GetChannel()->SetDelete();
    connections_.erase(sockfd);
  }
}


void TcpServer::OnMessage(std::function<void(Connection *)> func) { on_message_callback_ = std::move(func); }

void TcpServer::NewConnect(std::function<void(Connection *)> func) { new_connect_callback_ = std::move(func); }

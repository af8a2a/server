#include "Server.hh"
#include <arpa/inet.h>
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

Server::Server(){
  main_reactor_ = std::make_unique<EventLoop>();
  acceptor_ = std::make_unique<Acceptor>(main_reactor_.get());
  thread_pool_=std::make_unique<ThreadPool>();
  std::function<void(Socket *)> callback = std::bind(&Server::NewConnection, this, std::placeholders::_1);
  acceptor_->SetNewConnectionCallback(callback);

  int size = static_cast<int>(std::thread::hardware_concurrency());
  thread_pool_=std::make_unique<ThreadPool>(size);
  for (int i = 0; i < size; ++i) {
    sub_reactors_.emplace_back(std::make_unique<EventLoop>());

  }

  
}
void Server::Start() {
    for (auto & sub_reactor : sub_reactors_) {
    std::function<void()> sub_loop = std::bind(&EventLoop::Loop, sub_reactor.get());
    thread_pool_->Add(std::move(sub_loop));
  }
  main_reactor_->Loop();
}
Server::~Server() = default;

void Server::NewConnection(Socket *sock) {
  errif(sock->GetFd() == -1, "new connection error");
  uint64_t random = sock->GetFd() % sub_reactors_.size();
  auto conn=std::make_unique<Connection>(sub_reactors_[random].get(), sock);
  std::function<void(Socket *)> callback = std::bind(&Server::DeleteConnection, this, std::placeholders::_1);
  conn->SetDeleteConnectionCallback(callback);
  conn->SetOnConnectCallback(on_connect_callback_);
  conn->SetOnMessageCallback(on_message_callback_);

  connections_[sock->GetFd()] = std::move(conn);
  if (new_connect_callback_) {
    new_connect_callback_(connections_[sock->GetFd()].get());
  }
}

void Server::DeleteConnection(Socket *sock) {
  int sockfd = sock->GetFd();
  auto iter = connections_.find(sockfd);
  if (iter != connections_.end()) {
    
    connections_.erase(sockfd);
  }
}

void Server::OnConnect(std::function<void(Connection *)> func) { on_connect_callback_ = std::move(func); }

void Server::OnMessage(std::function<void(Connection *)> func) { on_message_callback_ = std::move(func); }

void Server::NewConnect(std::function<void(Connection *)> func) { new_connect_callback_ = std::move(func); }

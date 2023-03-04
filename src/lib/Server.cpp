#include "Server.hh"
#include <arpa/inet.h>
#include "Acceptor.hh"
#include "Channel.hh"
#include "Connection.hh"
#include "Eventloop.hh"
#include "InetAddress.hpp"
#include "Socket.hh"
#include "threadpool.hh"
#include "util.hpp"
#define READ_BUFFER 1024

Server::Server(EventLoop *loop) : main_reactor_(loop), acceptor_(new Acceptor(main_reactor_)), thread_pool_(nullptr) {

  
  std::function<void(Socket *)> callback = std::bind(&Server::NewConnection, this, std::placeholders::_1);
  acceptor_->SetNewConnectionCallback(callback);

  int size = static_cast<int>(std::thread::hardware_concurrency());
  thread_pool_ = new ThreadPool(size);
  for (int i = 0; i < size; ++i) {
    sub_reactors_.push_back(new EventLoop());
  }

  for (int i = 0; i < size; ++i) {
    std::function<void()> sub_loop = std::bind(&EventLoop::Loop, sub_reactors_[i]);
    thread_pool_->Add(std::move(sub_loop));
  }
}

Server::~Server() {
  for(EventLoop *each : sub_reactors_){
    delete each;
  }
  delete acceptor_;
  delete thread_pool_;
}

void Server::NewConnection(Socket *sock) {
  errif(sock->GetFd() == -1, "new connection error");
  uint64_t random = sock->GetFd() % sub_reactors_.size();
  Connection *conn = new Connection(sub_reactors_[random], sock);
  std::function<void(Socket *)> callback = std::bind(&Server::DeleteConnection, this, std::placeholders::_1);
  conn->SetDeleteConnectionCallback(callback);
  conn->SetOnConnectCallback(on_connect_callback_);
  conn->SetOnMessageCallback(on_message_callback_);

  connections_[sock->GetFd()] = conn;
  if (new_connect_callback_) {
    new_connect_callback_(conn);
  }
}

void Server::DeleteConnection(Socket *sock) {
  int sockfd = sock->GetFd();
  auto iter = connections_.find(sockfd);
  if (iter != connections_.end()) {
    Connection *conn = connections_[sockfd];
    connections_.erase(sockfd);
    delete conn;
    conn = nullptr;
  }
}

void Server::OnConnect(std::function<void(Connection *)> func) { on_connect_callback_ = std::move(func); }

void Server::OnMessage(std::function<void(Connection *)> func) { on_message_callback_ = std::move(func); }

void Server::NewConnect(std::function<void(Connection *)> func) { new_connect_callback_ = std::move(func); }

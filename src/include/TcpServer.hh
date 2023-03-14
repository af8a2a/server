#pragma once

#include "Macros.h"

#include <functional>
#include <map>
#include <memory>
#include <vector>
class EventLoop;
class Socket;
class Acceptor;
class Connection;
class ThreadPool;
class TcpServer {
 private:
  std::unique_ptr<EventLoop> main_reactor_;
  //Accepter负责接受连接，负责分发到epoll
  std::unique_ptr<Acceptor> acceptor_;
  std::unordered_map<int, std::unique_ptr<Connection>> connections_;
  std::vector<std::unique_ptr<EventLoop>> sub_reactors_;
  std::unique_ptr<ThreadPool> thread_pool_;
  std::function<void(Connection *)> on_message_callback_;
  std::function<void(Connection *)> new_connect_callback_;
 public:
  explicit TcpServer();
  ~TcpServer();

  DISALLOW_COPY_AND_MOVE(TcpServer);
  void Start();
  void NewConnection(Socket *sock);
  void DeleteConnection(Socket *sock);
  void OnMessage(std::function<void(Connection *)> func);
  void NewConnect(std::function<void(Connection *)> func);
};

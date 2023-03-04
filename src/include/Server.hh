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
class Server {
 private:
  std::unique_ptr<EventLoop> main_reactor_;
  std::unique_ptr<Acceptor> acceptor_;
  std::unordered_map<int, std::unique_ptr<Connection>> connections_;
  std::vector<std::unique_ptr<EventLoop>> sub_reactors_;
  std::unique_ptr<ThreadPool> thread_pool_;
  std::function<void(Connection *)> on_connect_callback_;
  std::function<void(Connection *)> on_message_callback_;
  std::function<void(Connection *)> new_connect_callback_;
 public:
  explicit Server();
  ~Server();

  DISALLOW_COPY_AND_MOVE(Server);
  void Start();
  void NewConnection(Socket *sock);
  void DeleteConnection(Socket *sock);
  void OnConnect(std::function<void(Connection *)> func);
  void OnMessage(std::function<void(Connection *)> func);
  void NewConnect(std::function<void(Connection *)> func);
};

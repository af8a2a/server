#pragma once

#include <map>
class EventLoop;
class ServerSocket;
class Acceptor;
class Connection;
class Server {
 private:
  EventLoop *loop;
  Acceptor *acceptor;
  std::map<int, Connection *> connections;
  // Acceptor *acceptor;
 public:
  explicit Server(EventLoop *_loop);
  ~Server();
  void handleReadEvent(int);
  void newConnection(ServerSocket *serv_sock);
  void deleteConnection(ServerSocket * sock);
};

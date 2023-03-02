#pragma once

#include <map>
#include <vector>
class EventLoop;
class ServerSocket;
class Acceptor;
class Connection;
class ThreadPool;
class Server {
 private:
  EventLoop *mainReactor;
  Acceptor *acceptor;
  std::vector<EventLoop*> subReactors;
  std::map<int, Connection *> connections;
  ThreadPool *thpool;
 public:
  explicit Server(EventLoop *_loop);
  ~Server();
  void handleReadEvent(int);
  void newConnection(ServerSocket *serv_sock);
  void deleteConnection(int sockfd);
};

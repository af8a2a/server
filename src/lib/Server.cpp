#include "Server.hh"
#include <arpa/inet.h>
#include "Acceptor.hh"
#include "Channel.hh"
#include "Connection.hh"
#include "Eventloop.hh"
#include "InetAddress.hpp"
#include "Socket.hpp"
#include "threadpool.hh"
#define READ_BUFFER 1024

Server::Server(EventLoop *_mainReactor) : mainReactor(_mainReactor), acceptor(nullptr) {
  acceptor = new Acceptor(mainReactor);
  std::function<void(ServerSocket *)> cb = [this](auto &&PH1) { newConnection(std::forward<decltype(PH1)>(PH1)); };
  acceptor->setNewConnectionCallback(cb);
  
  int size = std::thread::hardware_concurrency();
  thpool = new ThreadPool(size);
  for (int i = 0; i < size; ++i) {
    subReactors.push_back(new EventLoop());
  }
  for (int i = 0; i < size; ++i) {
    std::function<void()> sub_loop = [capture0 = subReactors[i]] { capture0->loop(); };
    thpool->add(sub_loop);
  }
}
Server::~Server() {
  delete acceptor;
  delete thpool;
}
void Server::handleReadEvent(int sockfd) {
  char buf[READ_BUFFER];
  while (true) {  //由于使用非阻塞IO，读取客户端buffer，一次读取buf大小数据，直到全部读取完毕
    bzero(&buf, sizeof(buf));
    ssize_t bytes_read = read(sockfd, buf, sizeof(buf));
    if (bytes_read > 0) {
      printf("message from client fd %d: %s\n", sockfd, buf);
      write(sockfd, buf, sizeof(buf));
    } else if (bytes_read == -1 && errno == EINTR) {  //客户端正常中断、继续读取
      printf("continue reading");
      continue;
    } else if (bytes_read == -1 &&
               ((errno == EAGAIN) || (errno == EWOULDBLOCK))) {  //非阻塞IO，这个条件表示数据全部读取完毕
      printf("finish reading once, errno: %d\n", errno);
      break;
    } else if (bytes_read == 0) {  // EOF，客户端断开连接
      printf("EOF, client fd %d disconnected\n", sockfd);
      close(sockfd);  //关闭socket会自动将文件描述符从epoll树上移除
      break;
    }
  }
}
void Server::newConnection(ServerSocket *serv_sock) {
  if (serv_sock->GetFd() != -1) {
    int random = serv_sock->GetFd() % subReactors.size();

    auto *conn = new Connection(subReactors[random], serv_sock);
    std::function<void(int)> cb = [this](auto &&PH1) { deleteConnection(std::forward<decltype(PH1)>(PH1)); };
    conn->setDeleteConnectionCallback(cb);
    connections[serv_sock->GetFd()] = conn;
  }
}

void Server::deleteConnection(int sockfd) {
  if (sockfd != -1) {
    auto it = connections.find(sockfd);
    if (it != connections.end()) {
      Connection *conn = connections[sockfd];
      connections.erase(sockfd);
      // close(sockfd);       //正常
      delete conn;  //会Segmant fault
    }
  }
}
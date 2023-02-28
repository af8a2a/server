#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <pthread.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <algorithm>
#include <cassert>
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <functional>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include "Socket.hpp"
#include "epoll/epoll.hpp"
#include "util/InetAddress.hpp"
#include "util/threadpool.hpp"
#define MAX_EVENT_NUMBER 1024
#define BUFFER_SIZE 1024
int connect_count = 0;
ThreadPool t(8);

/*ET模式的工作流程*/
void Et(int sockfd) {
  char buf[BUFFER_SIZE];
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
void handleReadEvent(int sockfd) {
  char buf[BUFFER_SIZE];
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
auto main(int argc, char *argv[]) -> int {
  // Socket初始化

  ServerSocket server;
  server.Bind("127.0.0.1", 8080);
  server.Listen(true);

  Epoll ep(1000, 5);
  server.SetNonBlocking();
  // 设置处理的
  //  文件描述符,设置为非阻塞
  ep.Addfd(server.GetFd(), true);
  ep.Setnonblocking(server.GetFd());
  while (true) {
    // 等待epoll事件的发生
    std::vector<Channel *> activeChannels = ep.Poll(-1);

    int nfds = activeChannels.size();

    // 处理所有的事件
    for (int i = 0; i < nfds; ++i) {
      int chfd = activeChannels[i]->getFd();

      if (chfd == server.GetFd()) {  // 新客户端连接
                                     //获取新客户端连接
        InetAddress clnt_addr;
        ServerSocket client_sock(server.Accept(&clnt_addr));
        printf("new client fd %d! IP: %s Port: %d\n", client_sock.GetFd(), inet_ntoa(clnt_addr.addr.sin_addr),
               ntohs(clnt_addr.addr.sin_port));

        client_sock.SetNonBlocking();
        //加入epoll
        ep.Addfd(client_sock.GetFd(), EPOLLIN | EPOLLET);
      } else if (activeChannels[i]->getRevents() & EPOLLIN) {  //可读事件
        Et(activeChannels[i]->getFd());
      } else {  //其他事件，之后的版本实现
        printf("something else happened\n");
      }
    }
  }

  return 0;
}

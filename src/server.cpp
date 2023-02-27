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
#include "util/threadpool.hpp"
#include "util/InetAddress.hpp"
#define MAX_EVENT_NUMBER 1024
#define BUFFER_SIZE 1024
int connect_count = 0;
ThreadPool t(8);

/*将文件描述符设置成非阻塞的*/
auto Setnonblocking(int fd) -> int {
  int old_option = fcntl(fd, F_GETFL);
  int new_option = old_option | O_NONBLOCK;
  fcntl(fd, F_SETFL, new_option);
  return old_option;
}
/*将文件描述符fd上的EPOLLIN注册到epollfd指示的epoll内核事件表中，参数
enable_et指定是否对fd启用ET模式*/
void Addfd(int epollfd, int fd, bool enable_et) {
  epoll_event event;
  event.data.fd = fd;
  event.events = EPOLLIN;
  if (enable_et) {
    event.events |= EPOLLET;
  }
  Setnonblocking(fd);
  epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &event);
}
/*LT模式的工作流程*/
void Lt(epoll_event *events, int number, int epollfd, int listenfd) {
  char buf[BUFFER_SIZE];
  for (int i = 0; i < number; i++) {
    int sockfd = events[i].data.fd;
    if (sockfd == listenfd) {
      struct sockaddr_in client_address;
      socklen_t client_addrlength = sizeof(client_address);
      int connfd = accept(listenfd, reinterpret_cast<struct sockaddr *>(&client_address), &client_addrlength);

      Addfd(epollfd, connfd, false); /*对connfd禁用ET模式*/
    } else if ((events[i].events & EPOLLIN) != 0U) {
      //读缓存中 有未读出的数据 这段代 就被触发
      /*只要socket读缓存中还有未读出的数据，这段代码就被触发*/

      memset(buf, '\0', BUFFER_SIZE);
      int ret = recv(sockfd, buf, BUFFER_SIZE - 1, 0);
      if (ret <= 0) {
        close(sockfd);
        continue;
      }

      // do something
    }
  }
}

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
void handleReadEvent(int sockfd){
    char buf[BUFFER_SIZE];
    while(true){    //由于使用非阻塞IO，读取客户端buffer，一次读取buf大小数据，直到全部读取完毕
        bzero(&buf, sizeof(buf));
        ssize_t bytes_read = read(sockfd, buf, sizeof(buf));
        if(bytes_read > 0){
            printf("message from client fd %d: %s\n", sockfd, buf);
            write(sockfd, buf, sizeof(buf));
        } else if(bytes_read == -1 && errno == EINTR){  //客户端正常中断、继续读取
            printf("continue reading");
            continue;
        } else if(bytes_read == -1 && ((errno == EAGAIN) || (errno == EWOULDBLOCK))){//非阻塞IO，这个条件表示数据全部读取完毕
            printf("finish reading once, errno: %d\n", errno);
            break;
        } else if(bytes_read == 0){  //EOF，客户端断开连接
            printf("EOF, client fd %d disconnected\n", sockfd);
            close(sockfd);   //关闭socket会自动将文件描述符从epoll树上移除
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
    std::vector<epoll_event> events = ep.Poll(-1);
    int nfds = events.size();

    // 处理所有的事件
    for (int i = 0; i < nfds; ++i) {
      if (events[i].data.fd == server.GetFd()) {  // 新客户端连接
        InetAddress clnt_addr;
        ServerSocket client_sock(server.Accept(&clnt_addr));
        printf("new client fd %d! IP: %s Port: %d\n", client_sock.GetFd(), inet_ntoa(clnt_addr.addr.sin_addr),
               ntohs(clnt_addr.addr.sin_port));

        client_sock.SetNonBlocking();

        ep.Addfd(client_sock.GetFd(), EPOLLIN | EPOLLET);
      } else if (events[i].events & EPOLLIN) {  //可读事件
        handleReadEvent(events[i].data.fd);
      } else {  //其他事件，之后的版本实现
        printf("something else happened\n");
      }
    }
  }
  // Et(ep.Getevent(), ret, ep.GetEpollfd(), fd); /*使用ET模式*/

  return 0;
}

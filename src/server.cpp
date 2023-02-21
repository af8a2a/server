#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <pthread.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <cassert>
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#define MAX_EVENT_NUMBER 1024
#define BUFFER_SIZE 10
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
  epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &event);
  Setnonblocking(fd);
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
      printf("event trigger once\n");
      memset(buf, '\0', BUFFER_SIZE);
      int ret = recv(sockfd, buf, BUFFER_SIZE - 1, 0);
      if (ret <= 0) {
        close(sockfd);
        continue;
      }
      printf("get%d bytes of content:%s\n", ret, buf);
    } else {
      printf("something else happened\n");
    }
  }
}
/*ET模式的工作流程*/
void Et(epoll_event *events, int number, int epollfd, int listenfd) {
  char buf[BUFFER_SIZE];
  for (int i = 0; i < number; i++) {
    int sockfd = events[i].data.fd;
    if (sockfd == listenfd) {
      struct sockaddr_in client_address;
      socklen_t client_addrlength = sizeof(client_address);
      int connfd = accept(listenfd, reinterpret_cast<struct sockaddr *>(&client_address), &client_addrlength);
      Addfd(epollfd, connfd, true); /*对connfd开启ET模式*/
    } else if ((events[i].events & EPOLLIN) != 0U) {
      /*这段代码不会被重复触发，所以我们循环读取数据，以确保把socket读缓存中的所
      有数据读出*/
      printf("event trigger once\n");
      while (true) {
        memset(buf, '\0', BUFFER_SIZE);
        int ret = recv(sockfd, buf, BUFFER_SIZE - 1, 0);
        if (ret < 0) {
          /*对于非阻塞IO，下面的条件成立表示数据已经全部读取完毕。此后，epoll就能再次
          触发sockfd上的EPOLLIN事件，以驱动下一次读操作*/
          if ((errno == EAGAIN) || (errno == EWOULDBLOCK)) {
            printf("read later\n");
            break;
          }
          close(sockfd);
          break;
        }
        if (ret == 0) {
          close(sockfd);
        } else {
          printf("get%d bytes of content:%s\n", ret, buf);
        }
      }
    } else {
      printf("something else happened\n");
    }
  }
}
auto main(int argc, char *argv[]) -> int {
  if (argc <= 2) {
    printf("usage:%s ip_address port_number\n", basename(argv[0]));
    return 1;
  }
  const char *ip = argv[1];
  int port = atoi(argv[2]);
  int ret = 0;
  struct sockaddr_in address;
  bzero(&address, sizeof(address));
  address.sin_family = AF_INET;
  inet_pton(AF_INET, ip, &address.sin_addr);
  address.sin_port = htons(port);
  int listenfd = socket(PF_INET, SOCK_STREAM, 0);
  assert(listenfd >= 0);
  ret = bind(listenfd, reinterpret_cast<struct sockaddr *>(&address), sizeof(address));
  assert(ret != -1);
  ret = listen(listenfd, 5);
  assert(ret != -1);
  epoll_event events[MAX_EVENT_NUMBER];
  int epollfd = epoll_create(5);
  assert(epollfd != -1);
  Addfd(epollfd, listenfd, true);
  while (true) {
    int ret = epoll_wait(epollfd, events, MAX_EVENT_NUMBER, -1);
    if (ret < 0) {
      printf("epoll failure\n");
      break;
    }
    Lt(events, ret, epollfd, listenfd); /*使用LT模式*/
    // et(events,ret,epollfd,listenfd);/*使用ET模式*/
  }
  close(listenfd);
  return 0;
}

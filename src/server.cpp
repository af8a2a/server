#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <pthread.h>
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
#include "Channel.hh"
#include "epoll.hh"
#include "Eventloop.hh"
#include "Server.hh"
#include "InetAddress.hpp"
#include "threadpool.hpp"
#define MAX_EVENT_NUMBER 1024
#define BUFFER_SIZE 1024
int connect_count = 0;

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
    auto loop=new EventLoop();
    auto server = new Server(loop);
    loop->loop();
    return 0;
}

/*
/*
** client.c -- 一个 stream socket client 的 demo
*/
#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <thread>
#define PORT "3490"      // Client 所要连接的 port
#define MAXDATASIZE 100  // 我们一次可以收到的最大字节数量（number of bytes）

// 取得 IPv4 或 IPv6 的 sockaddr：
auto GetInAddr(struct sockaddr *sa) -> void * {
  if (sa->sa_family == AF_INET) {
    return &((reinterpret_cast<struct sockaddr_in *>(sa))->sin_addr);
  }

  return &((reinterpret_cast<struct sockaddr_in6 *>(sa))->sin6_addr);
}
void ThreadClient() {
  int sockfd;
  int numbytes;
  char buf[MAXDATASIZE];
  struct addrinfo hints;
  struct addrinfo *servinfo;
  struct addrinfo *p;
  int rv;
  char s[INET6_ADDRSTRLEN];
  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;

  if ((rv = getaddrinfo("127.0.0.1", PORT, &hints, &servinfo)) != 0) {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
    return;
  }

  // 用循环取得全部的结果，并先连接到能成功连接的
  for (p = servinfo; p != nullptr; p = p->ai_next) {
    sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
    if (sockfd == -1) {
      perror("client: socket");
      continue;
    }

    if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
      close(sockfd);
      perror("client: connect");
      continue;
    }

    break;
  }

  if (p == nullptr) {
    fprintf(stderr, "client: failed to connect\n");
    return;
  }

  inet_ntop(p->ai_family, GetInAddr(p->ai_addr), s, sizeof s);

  printf("client: connecting to %s\n", s);

  freeaddrinfo(servinfo);  // 全部皆以这个 structure 完成
  numbytes = recv(sockfd, buf, MAXDATASIZE - 1, 0);
  if (numbytes == -1) {
    perror("recv");
    exit(1);
  }

  buf[numbytes] = '\0';
  printf("client: received '%s'\n", buf);

  close(sockfd);
}
auto main(int argc, char *argv[]) -> int {
  std::thread a(ThreadClient);
  std::thread b(ThreadClient);
  a.join();
}
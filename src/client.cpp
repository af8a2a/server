
/*
** client.c -- 一个 stream socket client 的 demo
*/
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <thread>
#include <vector>
#define PORT "3490"      // Client 所要连接的 port
#define MAXDATASIZE 100  // 我们一次可以收到的最大字节数量（number of bytes）

// 取得 IPv4 或 IPv6 的 sockaddr：
auto GetInAddr(struct sockaddr *sa) -> void * {
  if (sa->sa_family == AF_INET) {
    return &((reinterpret_cast<struct sockaddr_in *>(sa))->sin_addr);
  }

  return &((reinterpret_cast<struct sockaddr_in6 *>(sa))->sin6_addr);
}

void ThreadClient(const std::vector<std::string> &cmd) {
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
  rv = getaddrinfo("127.0.0.1", PORT, &hints, &servinfo);
  if (rv != 0) {
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
  // int nums = send(sockfd,"Get",4,0);
  freeaddrinfo(servinfo);  // 全部皆以这个 structure 完成
  std::string msg;
  for (const auto &i : cmd) {
    msg += i;
    msg += " ";
  }
  send(sockfd, msg.data(), msg.size(), 0);

  numbytes = recv(sockfd, buf, MAXDATASIZE - 1, 0);
  if (numbytes == -1) {
    perror("recv");
    exit(1);
  }

  buf[numbytes] = '\0';
  printf("client: received '%s'\n", buf);

  close(sockfd);
}

//与服务器取得连接，返回描述符
auto Connect() -> int {
  int fd = socket(AF_INET, SOCK_STREAM, 0);
  struct sockaddr_in addr = {};
  addr.sin_family = AF_INET;
  addr.sin_port = ntohs(3490);
  addr.sin_addr.s_addr = ntohl(INADDR_LOOPBACK);  // 127.0.0.1
  int rv = connect(fd, reinterpret_cast<const struct sockaddr *>(&addr), sizeof(addr));
  return fd;
}

void Hello() { std::cout << "hello"; }
auto main(int argc, char *argv[]) -> int {
  std::vector<std::string> cmd;
  for (int i = 1; i < argc; i++) {
    cmd.emplace_back(argv[i]);
  }
    //put
  int fd = Connect();
  write(fd, "put 1 1", strlen("put 1 1"));
  char buf[1024];
  int rec = recv(fd, buf, sizeof(buf), 0);
  buf[rec] = '\0';
  std::cout << buf << std::endl;

//get
  memset(buf, '\0', strlen(buf));
  write(fd, "get 1", strlen("get 1"));
  rec = recv(fd, buf, sizeof(buf), 0);
  buf[rec]='\0';
  std::cout << buf << std::endl;
  // del
  memset(buf, '\0', strlen(buf));
  write(fd, "del 1", strlen("del 1"));
  rec = recv(fd, buf, sizeof(buf), 0);
  buf[rec] = '\0';
  std::cout << buf << std::endl;


  // get
  memset(buf, '\0', strlen(buf));
  write(fd, "get 1", strlen("get 1"));
  rec = recv(fd, buf, sizeof(buf), 0);
  buf[rec] = '\0';
  std::cout << buf << std::endl;
}
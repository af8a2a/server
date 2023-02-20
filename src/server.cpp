/*
** server.c － 展示一个stream socket server
*/
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <cerrno>
#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>
#include <unordered_map>
#include "helper.hpp"
#include <format>
#define PORT "3490"  // 提供给用戶连接的 port
#define BACKLOG 10   // 有多少个特定的连接队列（pending connections queue）

void SigchldHandler(int s) {
  while (waitpid(-1, nullptr, WNOHANG) > 0) {
    ;
  }
}

// 取得 sockaddr，IPv4 或 IPv6：
auto GetInAddr(struct sockaddr *sa) -> void * {
  if (sa->sa_family == AF_INET) {
    return &((reinterpret_cast<struct sockaddr_in *>(sa))->sin_addr);
  }
  return &((reinterpret_cast<struct sockaddr_in6 *>(sa))->sin6_addr);
}
std::unordered_map<int, int> hash;
enum class Command { Get, Set, Del };

void Put(int key, int val) { hash[key] = val; }

auto main() -> int {
  int sockfd;
  int new_fd;  // 在 sock_fd 进行 listen，new_fd 是新的连接
  struct addrinfo hints;
  struct addrinfo *servinfo;
  struct addrinfo *p;
  struct sockaddr_storage their_addr;  // 连接者的地址资料
  socklen_t sin_size;
  struct sigaction sa;
  int yes = 1;
  char s[INET6_ADDRSTRLEN];
  int rv;

  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;  // 使用我的 IP
  rv = getaddrinfo(nullptr, PORT, &hints, &servinfo);
  if (rv != 0) {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
    return 1;
  }

  // 以循环找出全部的结果，并绑定（bind）到第一个能用的结果
  for (p = servinfo; p != nullptr; p = p->ai_next) {
    sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
    if (sockfd == -1) {
      perror("server: socket");
      continue;
    }

    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
      perror("setsockopt");
      exit(1);
    }

    if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
      close(sockfd);
      perror("server: bind");
      continue;
    }

    break;
  }

  if (p == nullptr) {
    fprintf(stderr, "server: failed to bind\n");
    return 2;
  }

  freeaddrinfo(servinfo);  // 全部都用这个 structure

  if (listen(sockfd, BACKLOG) == -1) {
    perror("listen");
    exit(1);
  }

  sa.sa_handler = SigchldHandler;  // 收拾全部死掉的 processes
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = SA_RESTART;

  if (sigaction(SIGCHLD, &sa, nullptr) == -1) {
    perror("sigaction");
    exit(1);
  }

  printf("server: waiting for connections...\n");

  while (true) {  // 主要的 accept() 循环

    sin_size = sizeof their_addr;
    new_fd = accept(sockfd, reinterpret_cast<struct sockaddr *>(&their_addr), &sin_size);

    if (new_fd == -1) {
      perror("accept");
      continue;
    }

    inet_ntop(their_addr.ss_family, GetInAddr(reinterpret_cast<struct sockaddr *>(&their_addr)), s, sizeof s);
    printf("server: got connection from %s\n", s);

    // int rec = recv(sockfd, buf, 1023, 0);
    // buf[rec] = '\0';
    // std::cout << buf;
    if (fork() == 0) {  // 这个是 child process
      close(sockfd);    // child 不需要 listener
      char buf[1024];
      int rec = recv(new_fd, buf, 1024, 0);
      if (rec == -1) {
        perror("send");
      }
      buf[rec] = '\0';
      auto cmd = helper::Split(std::string(buf));
      if (!cmd.empty()) {
        if (cmd.size() == 2 && cmd[0] == "get" && !cmd[1].empty()) {
          auto key = atoi(cmd[1].c_str());
          int data = hash[key];
            //int a=321;
          std::string stringbuf = std::to_string(data);
          // std::string stringbuf = "put success";
          send(new_fd, stringbuf.c_str(), stringbuf.size(), 0);
          
        } else if (cmd.size() == 3 && cmd[0] == "put" && !cmd[1].empty() && !cmd[2].empty()) {
          auto key = atoi(cmd[1].c_str());
          auto val = atoi(cmd[2].c_str());
          hash[key] = val;
          std::string stringbuf;
        //   for (auto i : hash) {
        //     stringbuf+=std::to_string(i.first)
        //   }
          send(new_fd, stringbuf.c_str(), stringbuf.size(), 0);
        }
      }
      //   printf("%s\n", buf);
      close(new_fd);

      exit(0);
    }
    close(new_fd);  // parent 不需要这个
  }

  return 0;
}
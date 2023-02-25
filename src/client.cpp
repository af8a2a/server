
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
#include <string>
#include <thread>
#include <vector>
#include "Conn.hpp"
#include "Socket.hpp"
#define PORT "3490"      // Client 所要连接的 port
#define MAXDATASIZE 100  // 我们一次可以收到的最大字节数量（number of bytes）

//与服务器取得连接，返回描述符

auto main(int argc, char *argv[]) -> int {
  std::vector<std::string> cmd;
  for (int i = 1; i < argc; i++) {
    cmd.emplace_back(argv[i]);
  }
  // put

  Conn con;
  int &fd = con.fd_;
  fd = Connect("127.0.0.1", 3490);
  write(fd, "put 1 1", strlen("put 1 1"));
  char buf[1024];
  int rec = recv(fd, buf, sizeof(buf), 0);
  buf[rec] = '\0';
  std::cout << buf << std::endl;

  // get
  memset(buf, '\0', strlen(buf));
  write(fd, "get 1", strlen("get 1"));
  rec = recv(fd, buf, sizeof(buf), 0);
  buf[rec] = '\0';
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
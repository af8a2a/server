
/*
** client.c -- 一个 stream socket client 的 demo
*/
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <atomic>
#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>
#include <thread>
#include <vector>
#include "Socket.hh"
#include "threadpool.hh"
#define PORT "3490"      // Client 所要连接的 port
#define MAXDATASIZE 100  // 我们一次可以收到的最大字节数量（number of bytes）
//与服务器取得连接，返回描述符


void Print(int a, double b, const char *c, std::string const &d) { std::cout << a << b << c << d << std::endl; }

void Test() { std::cout << "hellp" << std::endl; }

int main(int argc, char const *argv[]) {
  ThreadPool *poll = new ThreadPool();
  std::function<void()> func = std::bind(Print, 1, 3.14, "hello", std::string("world"));
  poll->Add(func);
  func = Test;
  poll->Add(func);
  delete poll;
  return 0;
}
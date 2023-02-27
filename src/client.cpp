
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
void test() {
  Socket con;
  con.Connect("127.0.0.1", 8080);
  con.Write("hell");
}
auto main(int argc, char *argv[]) -> int {
  std::vector<std::string> cmd;
  for (int i = 1; i < argc; i++) {
    cmd.emplace_back(argv[i]);
  }
  // put
// test();
  for (int i = 0; i < 5; i++) {
    std::thread(test).detach();
  }
  //   Conn con;
  // //   int fd;

  //   con.Connect("127.0.0.1",4000);
  //   con.Write("hello");
  //   con.Read();
  //   std::cout<<con.readbuf_<<std::endl;
}
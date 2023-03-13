#include <unistd.h>
#include <cstring>

#include <functional>
#include <iostream>

#include "Connection.hh"
#include "Socket.hh"
#include "threadpool.hh"
#include "timer.hh"

void OneClient(const std::string &host, const std::string &path) {  // NOLINT

  Socket *sock = new Socket();

  // sock->SetNonBlocking();
  sock->Connect("127.0.0.1", 1234);

  Connection *conn = new Connection(nullptr, sock);

  conn->Send("GET " + path + " HTTP/1.1\r\nHost: " + host + "\r\n\r\n");
  conn->Read();
  std::cout << "Message from server: " << conn->ReadBuffer() << std::endl;
}

void Test() { std::cout << "hello" << std::endl; }
int main() {
  HeapTimer timer;
  int tid = 0;
  timer.Add(tid++, 1000, Test);
  //timer.Add(tid++, 2000, Test);
  int timeout=5000;
  while (timeout > 0) {
    timeout=timer.GetNextTick();
    std::cout<<timeout<<std::endl;

  }
}

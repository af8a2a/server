#include <unistd.h>
#include <cstring>

#include <functional>
#include <iostream>

#include "Connection.hh"
#include "Socket.hh"
#include "threadpool.hh"

void OneClient(int msgs, int wait) {  // NOLINT

  Socket *sock = new Socket();
  
  //sock->SetNonBlocking();
  sock->Connect("127.0.0.1", 1234);
  
  Connection *conn = new Connection(nullptr, sock);
 
  conn->Send("hello");
  conn->Read();
  conn->Close();
  std::cout << "Message from server: " << conn->ReadBuffer() << std::endl;
}
void Print(int a, double b, const char *c, std::string const &d) { std::cout << a << b << c << d << std::endl; }

void Test() { std::cout << "hellp" << std::endl; }
int main() {
  int threads = 20000;


  ThreadPool *poll = new ThreadPool(threads);
  std::function<void()> func =  std::bind(OneClient, 100,10);
  for (int i = 0; i < threads; ++i) {
    poll->Add(func);
  }
  delete poll;
  return 0;
}

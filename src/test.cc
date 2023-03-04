#include <unistd.h>
#include <cstring>

#include <functional>
#include <iostream>

#include "Connection.hh"
#include "Socket.hh"
#include "threadpool.hh"

void OneClient(int msgs, int wait) {  // NOLINT
  Socket *sock = new Socket();
  sock->Connect("127.0.0.1", 1234);

  Connection *conn = new Connection(nullptr, sock);

    conn->Send("hello");
    conn->Close();
    conn->Read();
    std::cout << "Message from server: " << conn->ReadBuffer() << std::endl;
  
}

int main(int argc, char *argv[]) {
  int threads = 1000;
  int msgs = 100;
  int wait = 0;

  ThreadPool *poll = new ThreadPool(threads);
  std::function<void()> func = std::bind(OneClient, msgs, wait);
  for (int i = 0; i < threads; ++i) {
    poll->Add(func);
  }
  delete poll;
  return 0;
}

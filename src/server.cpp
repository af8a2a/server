#include "Server.hh"
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
#include "Channel.hh"
#include "Connection.hh"
#include "Eventloop.hh"
#include "InetAddress.hpp"
#include "Socket.hh"
#include "epoll.hh"
#include "threadpool.hh"
#include "SignalHandler.hh"
#define MAX_EVENT_NUMBER 1024
#define BUFFER_SIZE 1024

auto main(int argc, char *argv[]) -> int {//NOLINT

  auto *server = new Server();

  server->NewConnect(
      [](Connection *conn) { std::cout << "New connection fd: " << conn->GetSocket()->GetFd() << std::endl; });

  server->OnMessage([](Connection *conn) {
    std::cout << "Message from client " << conn->ReadBuffer() << std::endl;
    if (conn->GetState() == Connection::State::Connected) {
      conn->Send(conn->ReadBuffer());
    }
  });

  server->Start();
  return 0;
}

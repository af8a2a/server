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
#include "SignalHandler.hh"
#include "Socket.hh"
#include "TcpServer.hh"
#include "epoll.hh"
#include "httpconn.hh"
#include "threadpool.hh"
#define MAX_EVENT_NUMBER 1024
#define BUFFER_SIZE 1024


auto main(int argc, char *argv[]) -> int {  // NOLINT
    auto *server = new TcpServer();

    server->NewConnect(
        [](Connection *conn) { std::cout << "New connection fd: " << conn->GetSocket()->GetFd() << std::endl; });

    server->OnMessage([](Connection *conn) {
      if (conn->GetState() == Connection::State::Closed) {
        std::cout << "connection fd " << conn->GetSocket()->GetFd() << " is Close " << std::endl;
        conn->Close();
        return;
      }
      HTTP* http=new HTTP();
      http->Prase(conn->ReadBuffer());
      std::cout << "Message from client " << conn->ReadBuffer() << std::endl;
      conn->Send(conn->ReadBuffer());
      delete(http);
    });

    server->Start();
    return 0;
}

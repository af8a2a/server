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
      // conn->Close();
      return;
    }
    std::cout << "Message from client " << conn->ReadBuffer() << std::endl;
    std::string req = conn->ReadBuffer();
    // std::string response_body_type = "text/html";
    // std::string err_str = "404";
    // std::string response_body =
    //     "<html>"
    //     "<title>" +
    //     err_str +
    //     "</title>"
    //     "<body>" +
    //     err_str +
    //     "<hr><em> Kiprey's Web Server</em>"
    //     "</body>"
    //     "</html>";

    // if (req.find("GET")!=std::string::npos) {
    //   std::stringstream sstream;
    //   sstream << "HTTP/1.1"
    //           << " " << 200 << " "
    //           << "hello"
    //           << "\r\n";
    //   sstream << "Connection: "
    //           << "Keep-Alive"
    //           << "\r\n";
    //   // Keep-Alive 头中, timeout 表示超时时间(单位s), max表示最多接收请求次数,超过则断开.
    //   sstream << "Keep-Alive: timeout=" << 10 << ", max=" << 2 << "\r\n";
    //   sstream << "Server: WebServer/1.1"
    //           << "\r\n";
    //   sstream << "Content-length: " << response_body_type.size() << "\r\n";
    //   sstream << "Content-type: " << response_body_type << "\r\n";
    //   sstream << "\r\n";

    //   req = sstream.str();
    // }
    conn->Send(req);
    // delete(http);
  });

  server->Start();
  return 0;
}

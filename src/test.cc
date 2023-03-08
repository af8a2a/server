#include <unistd.h>
#include <cstring>

#include <functional>
#include <iostream>

#include "Connection.hh"
#include "Socket.hh"
#include "threadpool.hh"

void OneClient(const std::string &host, const std::string &path) {  // NOLINT

  Socket *sock = new Socket();

  // sock->SetNonBlocking();
  sock->Connect("cs144.keithw.org", "http");
  
  Connection *conn = new Connection(nullptr, sock);
 
  conn->Send("GET "+path+" HTTP/1.1\r\nHost: "+host+"\r\n\r\n");
  conn->Read();
  std::cout << "Message from server: " << conn->ReadBuffer() << std::endl;
}

void Test() { std::cout << "hellp" << std::endl; }
int main() {
OneClient("cs144.keithw.org","/hello");
}

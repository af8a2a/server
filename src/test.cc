#include <sys/socket.h>
#include "Socket.hpp"
auto main() -> int {
  ServerSocket s;
  s.Bind("127.0.0.1", 8080);
  s.Listen(false);
  int con=s.Accept();
  char buf[1024];
  int rec = recv(con, buf, sizeof(buf), 0);
  buf[rec] = '\0';
  std::cout<<buf<<std::endl;
}
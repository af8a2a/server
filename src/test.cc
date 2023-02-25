#include "Socket.hpp"
auto main() -> int {
  int fd = Connect("cs144.keithw.org", "http");
  write(fd, "GET /hello HTTP/1.1\r\nHost: cs144.keithw.org\r\n\r\n",
        strlen("GET /Hello HTTP/1.1\r\nHost: cs144.keithw.org\r\n\r\n"));
  std::string buf;
  char buff[1024];
  int rec = recv(fd, buff, 1024, 0);
  buff[rec]='\0';
  std::cout<<buff;
}
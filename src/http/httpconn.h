#ifndef TASK_H
#define TASK_H

#include <netinet/in.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <cerrno>
#include <csignal>
#include <cstdio>
#include <cstring>

#include "util/threadpool.hpp"

class task_conn {
 public:
  task_conn() = default;
  ~task_conn() = default;
  void init(int epollfd, int sockfd, const sockaddr_in &client_addr) {
    m_epollfd = epollfd;
    m_sockfd = sockfd;
    m_address = client_addr;
    memset(m_buf, '\0', BUFFER_SIZE);
    m_read_idx = 0;
  }

  void accept_request();
  void bad_request(int client);
  void cat(int client, FILE *resource);
  void cannot_execute(int client);
  void error_die(const char *sc);
  void execute_cgi(int client, const char *path, const char *method, const char *query_string);
  auto get_line(int sock, char *buf, int size) -> int;
  void headers(int client, const char *filename);
  void not_found(int client);
  void serve_file(int client, const char *filename);
  void unimplemented(int client);

 private:
  static const int BUFFER_SIZE = 1024;
  int m_epollfd;
  int m_sockfd;
  sockaddr_in m_address;
  char m_buf[BUFFER_SIZE];
  int m_read_idx;
};

#endif
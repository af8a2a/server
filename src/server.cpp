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
#include "epoll.hh"
#include "Eventloop.hh"
#include "Server.hh"
#include "InetAddress.hpp"
#include "threadpool.hh"
#define MAX_EVENT_NUMBER 1024
#define BUFFER_SIZE 1024

auto main(int argc, char *argv[]) -> int {
    auto *loop=new EventLoop();
    auto *server = new Server(loop);
    loop->Loop();
    return 0;
}

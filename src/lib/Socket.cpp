/******************************
*   author: yuesong-feng
*   客户端、服务器共用
*   accept，connect都支持非阻塞式IO，但只是简单处理，如果情况太复杂可能会有意料之外的bug
*
******************************/
#include "Socket.hh"
#include "InetAddress.hpp"
#include "util.hpp"
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <cstring>
#include <cstdio>
#include <cerrno>

Socket::Socket() : fd_(socket(AF_INET, SOCK_STREAM, 0)){
    
    errif(fd_ == -1, "socket create error");
}
Socket::Socket(int _fd) : fd_(_fd){
    errif(fd_ == -1, "socket create error");
}

Socket::~Socket(){
    if(fd_ != -1){
        close(fd_);
        fd_ = -1;
    }
}

void Socket::Bind(InetAddress *_addr){
    struct sockaddr_in addr = _addr->GetAddr();
    errif(::bind(fd_, (sockaddr*)&addr, sizeof(addr)) == -1, "socket bind error");
}

void Socket::Listen(){
    errif(::listen(fd_, SOMAXCONN) == -1, "socket listen error");
}
void Socket::SetNonBlocking(){
    fcntl(fd_, F_SETFL, fcntl(fd_, F_GETFL) | O_NONBLOCK);
}
bool Socket::IsNonBlocking(){
    return (fcntl(fd_, F_GETFL) & O_NONBLOCK) != 0;
}
int Socket::Accept(InetAddress *_addr){
    // for server socket
    int clnt_sockfd = -1;
    struct sockaddr_in addr{};
    bzero(&addr, sizeof(addr));
    socklen_t addr_len = sizeof(addr);
    if(fcntl(fd_, F_GETFL) & O_NONBLOCK){
        while(true){
            clnt_sockfd = ::accept(fd_, (sockaddr*)&addr, &addr_len);
            if(clnt_sockfd == -1 && ((errno == EAGAIN) || (errno == EWOULDBLOCK))){
                // printf("no connection yet\n");
                continue;
            } if(clnt_sockfd == -1){
                errif(true, "socket accept error");
            } else{
                break;
            }
        }
    }
    else{
        clnt_sockfd = ::accept(fd_, (sockaddr*)&addr, &addr_len);
        errif(clnt_sockfd == -1, "socket accept error");
    }
    _addr->SetAddr(addr);
    return clnt_sockfd;
}

void Socket::Connect(InetAddress *_addr){
    // for client socket
    struct sockaddr_in addr = _addr->GetAddr();
    if(fcntl(fd_, F_GETFL) & O_NONBLOCK){
        while(true){
            int ret = ::connect(fd_, (sockaddr*)&addr, sizeof(addr));
            if(ret == 0){
                break;
            } if(ret == -1 && (errno == EINPROGRESS)){
                continue;
            /* 连接非阻塞式sockfd建议的做法：
                The socket is nonblocking and the connection cannot be
              completed immediately.  (UNIX domain sockets failed with
              EAGAIN instead.)  It is possible to select(2) or poll(2)
              for completion by selecting the socket for writing.  After
              select(2) indicates writability, use getsockopt(2) to read
              the SO_ERROR option at level SOL_SOCKET to determine
              whether connect() completed successfully (SO_ERROR is
              zero) or unsuccessfully (SO_ERROR is one of the usual
              error codes listed here, explaining the reason for the
              failure).
              这里为了简单、不断连接直到连接完成，相当于阻塞式
            */
            } if(ret == -1){
                errif(true, "socket connect error");
            }
        }
    } else{
        errif(::connect(fd_, (sockaddr*)&addr, sizeof(addr)) == -1, "socket connect error");
    }
}
void Socket::Connect(const char *ipaddr, uint16_t port) {
  InetAddress *addr = new InetAddress(ipaddr, port);
  Connect(addr);
  delete addr;
}

int Socket::GetFd(){
    return fd_;
}


#include "server/Server.hh"
#include "Socket.hpp"
#include "util/InetAddress.hpp"
#include "channel/Channel.hh"
#include <cerrno>
#include <cstdio>
#include <functional>
#include <cstring>
#include <unistd.h>

#define READ_BUFFER 1024

Server::Server(EventLoop *_loop) : loop(_loop){    
    auto *serv_sock = new ServerSocket();
    serv_sock->Bind("127.0.0.1", 8080);
    serv_sock->Listen(true); 
       
    auto *servChannel = new Channel(loop, serv_sock->GetFd());
    std::function<void()> cb = [this, serv_sock] { newConnection(serv_sock); };
    servChannel->setCallback(cb);
    servChannel->enableReading();

}

void Server::handleReadEvent(int sockfd){
    char buf[READ_BUFFER];
    while(true){    //由于使用非阻塞IO，读取客户端buffer，一次读取buf大小数据，直到全部读取完毕
        bzero(&buf, sizeof(buf));
        ssize_t bytes_read = read(sockfd, buf, sizeof(buf));
        if(bytes_read > 0){
            printf("message from client fd %d: %s\n", sockfd, buf);
            write(sockfd, buf, sizeof(buf));
        } else if(bytes_read == -1 && errno == EINTR){  //客户端正常中断、继续读取
            printf("continue reading");
            continue;
        } else if(bytes_read == -1 && ((errno == EAGAIN) || (errno == EWOULDBLOCK))){//非阻塞IO，这个条件表示数据全部读取完毕
            printf("finish reading once, errno: %d\n", errno);
            break;
        } else if(bytes_read == 0){  //EOF，客户端断开连接
            printf("EOF, client fd %d disconnected\n", sockfd);
            close(sockfd);   //关闭socket会自动将文件描述符从epoll树上移除
            break;
        }
    }
}

void Server::newConnection(ServerSocket *serv_sock){
    auto *clnt_addr = new InetAddress();      //会发生内存泄露！没有delete
    auto *clnt_sock = new ServerSocket(serv_sock->Accept(clnt_addr));       //会发生内存泄露！没有delete
    printf("new client fd %d! IP: %s Port: %d\n", clnt_sock->GetFd(), inet_ntoa(clnt_addr->addr.sin_addr), ntohs(clnt_addr->addr.sin_port));
    clnt_sock->SetNonBlocking();
    auto *clntChannel = new Channel(loop, clnt_sock->GetFd());
    std::function<void()> cb = [this, capture0 = clnt_sock->GetFd()] { handleReadEvent(capture0); };
    clntChannel->setCallback(cb);
    clntChannel->enableReading();
}
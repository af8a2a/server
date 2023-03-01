#include "Server.hh"
#include <arpa/inet.h>
#include "Socket.hpp"
#include "Acceptor.hh"
#include "InetAddress.hpp"
#include "Channel.hh"
#include "Connection.hh"

#define READ_BUFFER 1024


Server::Server(EventLoop *_loop) : loop(_loop) , acceptor(nullptr){
    acceptor=new Acceptor(loop);
    std::function<void(ServerSocket*)> cb = [this](auto && PH1) { newConnection(std::forward<decltype(PH1)>(PH1)); };
    acceptor->setNewConnectionCallback(cb);
}
Server::~Server(){
    delete acceptor;
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
    if(serv_sock->GetFd() != -1){
        auto *conn = new Connection(loop, serv_sock);
        std::function<void(int)> cb = [this](auto && PH1) { deleteConnection(std::forward<decltype(PH1)>(PH1)); };
        conn->setDeleteConnectionCallback(cb);
        connections[serv_sock->GetFd()] = conn;
    }

}

void Server::deleteConnection(int sockfd){
    if(sockfd != -1){
        auto it = connections.find(sockfd);
        if(it != connections.end()){
            Connection *conn = connections[sockfd];
            connections.erase(sockfd);
            // close(sockfd);       //正常
            delete conn;         //会Segmant fault
        }
    }

}
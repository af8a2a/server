#include "Connection.hh"
#include <unistd.h>
#include <cassert>
#include <cstring>
#include <iostream>
#include <utility>
#include "Buffer.h"
#include "Channel.hh"
#include "Socket.hh"
#define READ_BUFFER 1024

#define ASSERT(expr, message) assert((expr) && (message))

Connection::Connection(EventLoop *loop, Socket *sock) {
  sock_ = std::make_unique<Socket>(sock->GetFd());
  if (loop != nullptr) {
    channel_ = std::make_unique<Channel>(loop, sock);
    channel_->EnableRead();
    channel_->UseET();
  }
  read_buf_ = std::make_unique<Buffer>();
  send_buf_ = std::make_unique<Buffer>();
  state_ = State::Connected;
}

Connection::~Connection() {
  Close();
}

void Connection::Read() {
  if (state_ != State::Connected) {
    perror("Connection is not connected, can not read");
    return;
  }
  std::cout<<"run read"<<std::endl;
  assert(state_ == State::Connected && "connection state is disconnected!");
  read_buf_->Clear();
  if (sock_->IsNonBlocking()) {
    ReadNonBlocking();
  } else {
    ReadBlocking();
  }
}
void Connection::Write() {
  if (state_ != State::Connected) {
    perror("Connection is not connected, can not write");
    return;
  }
  if (sock_->IsNonBlocking()) {
    WriteNonBlocking();
  } else {
    WriteBlocking();
  }
  send_buf_->Clear();
}
void Connection::SetOnMessageCallback(std::function<void(Connection *)> const &callback) {
  on_message_callback_ = callback;
  std::function<void()> bus = std::bind(&Connection::Business, this);
  channel_->SetReadCallback(bus);
}

void Connection::ReadNonBlocking() {
  int sockfd = sock_->GetFd();
  char buf[1024];  // 这个buf大小无所谓
  while (true) {   // 使用非阻塞IO，读取客户端buffer，一次读取buf大小数据，直到全部读取完毕
    memset(buf, 0, sizeof(buf));
    ssize_t bytes_read = read(sockfd, buf, sizeof(buf));
    if (bytes_read > 0) {
      read_buf_->Append(buf, static_cast<int>(bytes_read));
    } else if (bytes_read == -1 && errno == EINTR) {  // 程序正常中断、继续读取
      printf("continue reading\n");
      continue;
    } else if (bytes_read == -1 &&
               ((errno == EAGAIN) || (errno == EWOULDBLOCK))) {  // 非阻塞IO，这个条件表示数据全部读取完毕
      break;
    } else if (bytes_read == 0) {  // EOF，客户端断开连接
      printf("read EOF, non-block client fd %d disconnected\n", sockfd);
      state_ = State::Closed;
      //Close();
      break;
    } else {
      printf("Other error on client fd %d\n", sockfd);
      state_ = State::Closed;
      //Close();
      break;
    }
  }
}
void Connection::WriteNonBlocking() {
  int sockfd = sock_->GetFd();
  char buf[send_buf_->Size()];
  memcpy(buf, send_buf_->ToStr(), send_buf_->Size());
  int data_size = static_cast<int>(send_buf_->Size());
  int data_left = data_size;
  while (data_left > 0) {
    ssize_t bytes_write = write(sockfd, buf + data_size - data_left, data_left);
    if (bytes_write == -1 && errno == EINTR) {
      printf("continue writing\n");
      continue;
    }
    if (bytes_write == -1 && errno == EAGAIN) {
      break;
    }
    if (bytes_write == -1) {
      printf("Other error on client fd %d\n", sockfd);
      state_ = State::Closed;
      break;
    }
    data_left -= static_cast<int>(bytes_write);
  }
}

/**
 * @brief Never used by server, only for client
 *
 */
void Connection::ReadBlocking() {
  int sockfd = sock_->GetFd();
  unsigned int rcv_size = 0;
  socklen_t len = sizeof(rcv_size);
  getsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, &rcv_size, &len);
  char buf[1024];
  ssize_t bytes_read = read(sockfd, buf, sizeof(buf));
  if (bytes_read > 0) {
    read_buf_->Append(buf, static_cast<int>(bytes_read));
  } else if (bytes_read == 0) {
    printf("read EOF, blocking client fd %d disconnected\n", sockfd);
    state_ = State::Closed;
  } else if (bytes_read == -1) {
    printf("Other error on blocking client fd %d\n", sockfd);
    state_ = State::Closed;
  }
}

/**
 * @brief Never used by server, only for client
 *
 */
void Connection::WriteBlocking() {
  // 没有处理send_buffer_数据大于TCP写缓冲区，的情况，可能会有bug
  int sockfd = sock_->GetFd();
  ssize_t bytes_write = write(sockfd, send_buf_->ToStr(), send_buf_->Size());
  if (bytes_write == -1) {
    printf("Other error on blocking client fd %d\n", sockfd);
    state_ = State::Closed;
  }
}
void Connection::Send(const std::string &msg) {
  SetSendBuffer(msg.c_str());
  Write();
}
void Connection::Close() { delete_connectioin_callback_(sock_.get()); }

void Connection::Business() {
  Read();
  on_message_callback_(this);
}
Connection::State Connection::GetState() { return state_; }
void Connection::SetSendBuffer(const char *str) { send_buf_->SetBuf(str); }
Buffer *Connection::GetReadBuffer() { return read_buf_.get(); }
const char *Connection::ReadBuffer() { return read_buf_->ToStr(); }
Buffer *Connection::GetSendBuffer() { return send_buf_.get(); }
const char *Connection::SendBuffer() { return send_buf_->ToStr(); }

void Connection::SetDeleteConnectionCallback(std::function<void(Socket *)> const &callback) {
  delete_connectioin_callback_ = callback;
}
void Connection::SetOnConnectCallback(std::function<void(Connection *)> const &callback) {
  on_connect_callback_ = callback;
  channel_->SetReadCallback([this]() { on_connect_callback_(this); });
}

void Connection::GetlineSendBuffer() { send_buf_->Getline(); }

Socket *Connection::GetSocket() { return sock_.get(); }

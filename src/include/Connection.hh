#pragma once
#include "Macros.h"

#include <functional>
#include <memory>

class EventLoop;
class Socket;
class Channel;
class Buffer;
class HTTP;
class Connection {
 public:
  enum State {
    Invalid = 1,
    Connecting,
    Connected,
    Closed,
    Failed,
  };
  Connection(EventLoop *loop, Socket *sock);
  ~Connection();
  DISALLOW_COPY_AND_MOVE(Connection);

  void Read();
  void Write();
  void Send(const std::string &msg);

  void SetDeleteConnectionCallback(std::function<void(Socket *)> const &callback);
  void SetOnConnectCallback(std::function<void(Connection *)> const &callback);
  void SetOnMessageCallback(std::function<void(Connection *)> const &callback);

  HTTP* GetHttpConnection();
      
  State GetState();
  void Close();

  void SetSendBuffer(const char *str);
  Buffer *GetReadBuffer();
  const char *ReadBuffer();

  Buffer *GetSendBuffer();
  const char *SendBuffer();
  void GetlineSendBuffer();
  Socket *GetSocket();

  void OnConnect(std::function<void()> func);
  void OnMessage(std::function<void()> func);

 private:
  std::unique_ptr<Socket> sock_;
  std::unique_ptr<Channel> channel_;
  State state_{State::Invalid};
  std::unique_ptr<Buffer> read_buf_;
  std::unique_ptr<Buffer> send_buf_;
  std::unique_ptr<HTTP> http_connection_;

  std::function<void(Socket *)> delete_connectioin_callback_;
  std::function<void(Connection *)> on_connect_callback_;
  std::function<void(Connection *)> on_message_callback_;
  
  void Business();
  void ReadNonBlocking();
  void WriteNonBlocking();
  void ReadBlocking();
  void WriteBlocking();
};
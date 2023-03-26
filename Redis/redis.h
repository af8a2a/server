
#include <memory>
#include "eventloop.h"
#include "tcpserver.h"
using namespace tiny_muduo;
class TinyRedis {
 public:
  TinyRedis(EventLoop *loop, const Address &address);
  ~TinyRedis();
  void Start() {
    server_.Start();
  }

 private:
  EventLoop *loop_;
  TcpServer server_;
  std::function<void (const std::vector<std::string>&)> redis_callback_;
};
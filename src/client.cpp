#include <Connection.hh>
#include <Socket.hh>
#include <iostream>
#include <memory>

int main() {
  Socket *sock = new Socket();
  sock->Connect("127.0.0.1", 1234);

  std::unique_ptr<Connection> conn = std::make_unique<Connection>(nullptr, sock);
  conn->Send("hello");
  while (true) {
    if (conn->GetState() == Connection::State::Closed) {
      
      break;
    }
    conn->GetlineSendBuffer();
    // conn->Send("hello");
    conn->Write();
    conn->Read();
    std::cout << "Message from server: " << conn->ReadBuffer() << std::endl;
   
  }

  return 0;
}

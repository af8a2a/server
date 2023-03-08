#include "httpconn.hh"
#include "Buffer.h"
#include "Connection.hh"

std::string HTTP::Prase(Connection *conn) {
  std::string header = conn->GetReadBuffer()->ToStr();
  return header;
}
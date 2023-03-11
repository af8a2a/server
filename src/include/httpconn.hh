#include <map>
#include <string>
#include "TcpServer.hh"
class Connection;
enum HeaderState { PARSE_HEADER_SUCCESS = 1, PARSE_HEADER_AGAIN, PARSE_HEADER_ERROR };
enum ParseState { H_START = 0, H_KEY, H_COLON, H_SPACES_AFTER_COLON, H_VALUE, H_CR, H_LF, H_END_CR, H_END_LF };
enum HttpMethod { METHOD_POST = 1, METHOD_GET, METHOD_HEAD };
enum HttpVersion { HTTP_10 = 1, HTTP_11 };
enum URIState {
  PARSE_URI_AGAIN = 1,
  PARSE_URI_ERROR,
  PARSE_URI_SUCCESS,
};
class HTTP {
 public:
  HTTP();
  HeaderState Prase(const char *string);
  void Response();

 private:
  HttpVersion http_version_{HTTP_11};
  std::string file_name_{};
  HttpMethod method_{METHOD_GET};
  int now_read_pos_{0};
  URIState ParseUri(const char *string);
  ParseState h_state_{H_START};
  std::map<std::string, std::string> headers_;
  bool keep_alive_{true};
};


#include <cstddef>
#include <string>
enum class STATE { STATE_REQ, STATE_RES, STATE_END };
struct Conn {
  int fd_;
  STATE state_;  // either STATE_REQ or STATE_RES
  // buffer for reading
  std::string readbuffer_;
  std::string writebuffer_;
  size_t write_count_;
};

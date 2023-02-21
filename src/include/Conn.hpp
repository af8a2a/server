
#include <cstddef>
enum class STATE { STATE_REQ, STATE_RES, STATE_END };
struct Conn {
  int fd_;
  STATE state_;  // either STATE_REQ or STATE_RES
  // buffer for reading
  size_t read_buffer_size_;
  size_t write_buffer_size_;
  char read_buffer_[4096];
  char write_buffer_[4096];
  size_t write_buffer_sent_;
};

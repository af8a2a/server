#ifndef TINY_MUDUO_LOGFILE_H_
#define TINY_MUDUO_LOGFILE_H_

#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>

#include <string>

#include "timestamp.h"

namespace tiny_muduo {
struct LogInfo {
  std::string file_dir_;
  time_t lastwrite_;
};
static const time_t kFlushInterval = 3;

class LogFile {
 public:
  LogFile(const char *filepath);
  ~LogFile();

  void Write(const char *data, int len);
  void Flush() { fflush(fp_); }

  int64_t writtenbytes() const { return written_bytes_; }
  LogInfo GetInfo() { return {filedir_, std::max(lastwrite_,time(nullptr))}; }

 private:
  std::string filedir_;
  FILE *fp_;
  int64_t written_bytes_;
  time_t lastwrite_;
  time_t lastflush_;
};

}  // namespace tiny_muduo

#endif

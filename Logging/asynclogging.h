#ifndef TINY_MUDUO_ASYNCLOGGING_H_
#define TINY_MUDUO_ASYNCLOGGING_H_

#include <memory>
#include <vector>
#include <filesystem>

#include "condition.h"
#include "latch.h"
#include "logfile.h"
#include "logging.h"
#include "logstream.h"
#include "mutex.h"
#include "noncopyable.h"
#include "thread.h"
#include "logmanager.h"
namespace tiny_muduo {

static const double kBufferWriteTimeout = 3.0;
static const int64_t kSingleFileMaximumSize = 1024 * 1024 * 1024;

class AsyncLogging : public NonCopyAble {
 public:
  typedef FixedBuffer<kLargeSize> Buffer;
  typedef std::unique_ptr<Buffer> BufferPtr;
  typedef std::vector<BufferPtr> BufferVector;
  typedef std::unique_ptr<LogFile> LogFilePtr;

  AsyncLogging(const char *filepath = nullptr)
      : running_(false),
        filepath_(filepath),
        mutex_(),
        cond_(mutex_),
        latch_(1),
        thread_(std::bind(&AsyncLogging::ThreadFunc, this), "AsyncLogThread"),
        current_(new Buffer()),
        next_(new Buffer()) {
    
        }

  ~AsyncLogging() {
    if (running_) {
      Stop();
    }
  }

  void Stop() {
    running_ = false;
    cond_.Notify();
    thread_.Join();
  }

  void StartAsyncLogging() {
    running_ = true;
    thread_.StartThread();
    latch_.Wait();
  }

  void Append(const char *data, int len);
  void Flush();
  void ThreadFunc();
  void SetRollLog(bool isRool_);
  //滚动间隔/s
  void SetRollinterval_(int interval_);
  void SetAutoDelete(int interval_);
 private:
  bool isroll_log_;
  bool running_;
  const char *filepath_;
  MutexLock mutex_;
  Condition cond_;
  Latch latch_;
  Thread thread_;
  BufferPtr current_;
  BufferPtr next_;
  BufferVector buffers_to_write_;
  LogManager logmanager_;
  time_t last_roll_;
  int kRollPerSeconds_ = 60 * 60 * 24;
};

}  // namespace tiny_muduo

#endif

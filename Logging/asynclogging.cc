#include "asynclogging.h"

#include <chrono>
#include <ctime>
#include <functional>
#include <memory>
#include <utility>
#include "logging.h"

using namespace tiny_muduo;

void AsyncLogging::Append(const char *data, int len) {
  MutexLockGuard guard(mutex_);
  if (current_->writablebytes() >= len) {
    current_->Append(data, len);
  } else {
    buffers_to_write_.emplace_back(std::move(current_));
    if (next_) {
      current_ = std::move(next_);
    } else {
      current_.reset(new Buffer());
    }
    cond_.Notify();
  }
}

void AsyncLogging::Flush() { fflush(stdout); }

void AsyncLogging::ThreadFunc() {
  latch_.CountDown();
  BufferPtr newbuffer_current(new Buffer());
  BufferPtr newbuffer_next(new Buffer());
  LogFilePtr log(new LogFile(filepath_));
  logmanager_.Add(log->GetInfo());
  newbuffer_current->SetBufferZero();
  newbuffer_next->SetBufferZero();
  BufferVector buffers;
  timeval tv;

  while (running_) {
    {
      MutexLockGuard guard(mutex_);
      if (buffers_to_write_.empty()) {
        cond_.WaitForFewSeconds(kBufferWriteTimeout);
      }

      buffers_to_write_.emplace_back(std::move(current_));
      buffers.swap(buffers_to_write_);
      current_ = std::move(newbuffer_current);
      if (!next_) {
        next_ = std::move(newbuffer_next);
      }
    }  // locker

    for (const auto &buffer : buffers) {
      log->Write(buffer->data(), buffer->len());
    }
    //滚动日志
    gettimeofday(&tv, nullptr);
    time_t now = tv.tv_sec;
    LOG_DEBUG << "countdown:" << now - last_roll_;
    
    if (( log->writtenbytes() >= kSingleFileMaximumSize || now - last_roll_ >= kRollPerSeconds_)&&isroll_log_) {
      last_roll_ = now;
      logmanager_.RemoveOldLog();
      log.reset(new LogFile(filepath_));
      logmanager_.Add(log->GetInfo());
    }

    if (buffers.size() > 2) {
      buffers.resize(2);
    }

    if (!newbuffer_current) {
      newbuffer_current = std::move(buffers.back());
      buffers.pop_back();
      newbuffer_current->SetBufferZero();
    }

    if (!newbuffer_next) {
      newbuffer_current = std::move(buffers.back());
      buffers.pop_back();
      newbuffer_current->SetBufferZero();
    }
    buffers.clear();
    
  }
}
void AsyncLogging::SetRollLog(bool isRool_) { isroll_log_ = isRool_; }
void AsyncLogging::SetRollinterval_(int interval_) { kRollPerSeconds_ = interval_; }
void AsyncLogging::SetAutoDelete(int interval_) {
  logmanager_.Setinterval(interval_);
}
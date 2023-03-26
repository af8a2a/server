#include <logmanager.h>
#include <filesystem>
#include <memory>
#include "logging.h"
using namespace tiny_muduo;

LogManager::LogManager() : interval_(60 * 60 * 24), histroy_(std::make_unique<std::queue<LogInfo>>()) {}

void LogManager::RemoveOldLog() {
  if (histroy_->empty()) return;
  LogInfo log = histroy_->front();
  timeval tv;
  gettimeofday(&tv, nullptr);
  time_t now = tv.tv_sec;
  LOG_DEBUG << now;
  LOG_DEBUG<<"lastwrite"<<log.lastwrite_;
  if (now - log.lastwrite_ > interval_) {
    std::filesystem::remove(log.file_dir_);
    histroy_->pop();
  }
}
void LogManager::Add(LogInfo info) { histroy_->emplace(std::move(info)); }

void LogManager::Setinterval(std::time_t _interval) { interval_ = _interval; }